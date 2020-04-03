#include "fqparser.h"

fqstring_t *ks_init(void)
{
    fqstring_t *fqstr = (fqstring_t *)calloc(1, sizeof(fqstring_t));
    if(!fqstr) return NULL;
    return fqstr;
}


void ks_destroy(fqstring_t *fqstr)
{
    if (!fqstr) return;
    if (fqstr->s) free(fqstr->s);
    free(fqstr);
}


fqstream_t *fqs_init(const char *file_fqgz)
{
    fqstream_t *fqs = (fqstream_t *)calloc(1, sizeof(fqstream_t));
    if (!fqs) return 0;
    fqs->uncompressed_block = (unsigned char *)calloc(BUFFER_SIZE, sizeof(unsigned char));
    fqs->block_offset = 0;
    fqs->block_length = 0;
    fqs->fp = gzopen(file_fqgz, "r");
    if (!fqs->fp) {
        fqs_destroy(fqs);
        return 0;
    }
    return fqs;
}


void fqs_destroy(fqstream_t *fqs)
{
    if (!fqs) return;
    gzclose(fqs->fp);
    free(fqs->uncompressed_block);
    free(fqs);
}


int fqs_getline(fqstream_t *fqs, fqstring_t *fqstr, int delim)
{
    int state = 0;
    size_t l = 0;
    fqstr->l = 0;

    do {
        // read buffer
        if (fqs->block_offset >= fqs->block_length) {
            int bytes_read = gzread(fqs->fp, fqs->uncompressed_block, BUFFER_SIZE - 1);
            if (bytes_read == 0) {state = -1; break;} // eof reached
            if (bytes_read == -1) {state = -2; break;} // error decompressing
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

    if (fqstr->l == 0 && state < 0) return state;
    if ( delim=='\n' && fqstr->l>0 && fqstr->s[fqstr->l-1]=='\r' ) fqstr->l--;
    fqstr->s[fqstr->l] = 0;
    return (int)fqstr->l;
}


fqrecord_t *fqr_open(const char *file_fqgz)
{
    fqrecord_t *fqr = (fqrecord_t *)calloc(1, sizeof(fqrecord_t));
    if (!fqr) return NULL;
    fqr->name = ks_init();
    fqr->seq = ks_init();
    fqr->opt = ks_init();
    fqr->qual = ks_init();
    fqr->stream = fqs_init(file_fqgz);
    return fqr;
}


void fqr_close(fqrecord_t *fqr)
{
    if (!fqr) return;
    ks_destroy(fqr->name);
    ks_destroy(fqr->seq);
    ks_destroy(fqr->opt);
    ks_destroy(fqr->qual);
    fqs_destroy(fqr->stream);
    free(fqr);
}


int fqr_read(fqrecord_t *fqr)
{
    if (fqs_getline(fqr->stream, fqr->name, '\n') <= 0) return -4;
    if (fqs_getline(fqr->stream, fqr->seq, '\n') <= 0) return -3;
    if (fqs_getline(fqr->stream, fqr->opt, '\n') <= 0) return -2;
    if (fqs_getline(fqr->stream, fqr->qual, '\n') < 0) return -1;
    return 0;
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
    while(*qp)
        score += (*qp++) - 33;
    return (float)score / qual->l;
}


inline uint64_t fqr_hash(fqstring_t *seq)
{
    return NTC64_Base(seq->s, (uint32_t)seq->l);
}
