#include "fqparser.h"

fqstring_t *fqstr_init(void)
{
    fqstring_t *fqstr = (fqstring_t *)calloc(1, sizeof(fqstring_t));
    if (!fqstr) return NULL;
    return fqstr;
}


void fqstr_destroy(fqstring_t *fqstr)
{
    if (!fqstr) return;
    if (fqstr->s) free(fqstr->s);
    free(fqstr);
}


fqstream_t *fqs_open(const char *file_fqgz)
{
    fqstream_t *fqs = (fqstream_t *)calloc(1, sizeof(fqstream_t));
    if (!fqs) return 0;
    fqs->uncompressed_block = (unsigned char *)calloc(BUFFER_SIZE, sizeof(unsigned char));
    fqs->block_offset = 0;
    fqs->block_length = 0;
    fqs->fp = gzopen(file_fqgz, "r");
    if (!fqs->fp) {
        fqs_close(fqs);
        return 0;
    }
    return fqs;
}


void fqs_close(fqstream_t *fqs)
{
    if (!fqs) return;
    gzclose(fqs->fp);
    free(fqs->uncompressed_block);
    free(fqs);
}

/*
 * returns >=0 (normal) number of characters read
 * returns -1 eof reached
 * -2   reading stream
 */

int fqs_getline(fqstream_t *fqs, fqstring_t *fqstr, int delim, int append)
{
    int state = 0;
    size_t l = 0;
    fqstr->l = (append == 1) ? fqstr->l : 0;

    do {
        // read buffer
        if (fqs->block_offset >= fqs->block_length) {
            int bytes_read = gzread(fqs->fp, fqs->uncompressed_block, BUFFER_SIZE - 1);
            if (bytes_read == 0) {state = -1; break;} // eof reached
            if (bytes_read == -1) {state = -2; break;} // error reading stream
            fqs->uncompressed_block[bytes_read] = 0;
            fqs->block_offset = 0;
            fqs->block_length = (size_t)(bytes_read);
        }

        // find delimiter
        unsigned char *buf = fqs->uncompressed_block;
        for (l = fqs->block_offset; (l  < fqs->block_length) && (buf[l] != delim); ++l);
        if (l < fqs->block_length) state = 1; // delimiter is found
        l -= fqs->block_offset; // convert to token length

        if (fqstr->l + l + 1 >= fqstr->m) {
            fqstr->m = fqstr->l + l + 2;
            kroundup32(fqstr->m);
            fqstr->s = (char*)realloc(fqstr->s, fqstr->m);
        }

        // copy buffer
        memcpy(fqstr->s + fqstr->l, buf + fqs->block_offset, l);
        fqstr->l += l;
        fqs->block_offset += l + 1;
        if (fqs->block_offset >= fqs->block_length) {
            fqs->block_offset = 0;
            fqs->block_length = 0;
        }

    } while (state == 0);

    //if (fqstr->l == 0 && state < 0) return (state + 1);
    if (state < 0) return state; // -1 eof or -2 decompression error
    if ( delim=='\n' && fqstr->l>0 && fqstr->s[fqstr->l-1]=='\r' ) fqstr->l--;
    fqstr->s[fqstr->l] = 0;
    return (int)fqstr->l;
}


fqrecord_t *fqr_init(void)
{
    fqrecord_t *fqr = (fqrecord_t *)calloc(1, sizeof(fqrecord_t));
    if (!fqr) return NULL;
    fqr->name = fqstr_init();
    fqr->seq = fqstr_init();
    fqr->opt = fqstr_init();
    fqr->qual = fqstr_init();
    return fqr;
}


void fqr_destroy(fqrecord_t *fqr)
{
    if (!fqr) return;
    fqstr_destroy(fqr->name);
    fqstr_destroy(fqr->seq);
    fqstr_destroy(fqr->opt);
    fqstr_destroy(fqr->qual);
    free(fqr);
}

/*
 * >= 0 record size (normal)
 * -1 end of file
 * -2 truncated record
 */
int fqr_read_se(fqrecord_t *fqr, fqstream_t *fqs)
{
    int state;

    state = fqs_getline(fqs, fqr->name, '\n', 0);
    if (state < 0) return state;

    state = fqs_getline(fqs, fqr->seq, '\n', 0);
    if (state < 0) return state;

    state = fqs_getline(fqs, fqr->opt, '\n', 0);
    if (state < 0) return state;

    state = fqs_getline(fqs, fqr->qual, '\n', 0);
    if (state < 0) return state;

    return (int)(fqr->name->l + fqr->seq->l + fqr->opt->l + fqr->qual->l);
}


int fqr_read_pe(fqrecord_t *fqr, fqstream_t *fqs_1, fqstream_t *fqs_2)
{
    int state;

    state = fqs_getline(fqs_1, fqr->name, '\n', 0);
    if (state < 0) return state;

    state = fqs_getline(fqs_2, fqr->name, '\n', 0); // names should be the same
    if (state < 0) return state;

    state = fqs_getline(fqs_1, fqr->seq, '\n', 0);
    if (state < 0) return state;

    state = fqs_getline(fqs_2, fqr->seq, '\n', 1); // append sequence
    if (state < 0) return state;

    state = fqs_getline(fqs_1, fqr->opt, '\n', 0);
    if (state < 0) return state;

    state = fqs_getline(fqs_2, fqr->opt, '\n', 0); // options should be the same
    if (state < 0) return state;

    state = fqs_getline(fqs_1, fqr->qual, '\n', 0);
    if (state < 0) return state;

    state = fqs_getline(fqs_2, fqr->qual, '\n', 1); // append quality
    if (state < 0) return state;

    return (int)(fqr->name->l + fqr->seq->l + fqr->opt->l + fqr->qual->l);
}


void fqr_print(fqrecord_t *fqr)
{
    if (!fqr) return;
    printf("%s\n", fqr->name->s);
    printf("%s\n", fqr->seq->s);
    printf("%s\n", fqr->opt->s);
    printf("%s\n", fqr->qual->s);
}


void fqr_write(fqrecord_t *fqr, gzFile fo)
{
    if (!fqr) return;
    gzprintf(fo, "%s\n", fqr->name->s);
    gzprintf(fo, "%s\n", fqr->seq->s);
    gzprintf(fo, "%s\n", fqr->opt->s);
    gzprintf(fo, "%s\n", fqr->qual->s);
}


inline float fqr_qscore(fqstring_t *qual)
{
    int score = 0;
    const char *qp = qual->s;
    while (*qp)
        score += (*qp++) - 33;
    return (float)score / qual->l;
}


inline uint64_t fqr_hash(fqstring_t *seq)
{
    return NTC64_Base(seq->s, (uint32_t)seq->l);
}
