#include "fqparser.h"

static inline char *kscat(const char *read_se, const char *read_pe)
{
    char *buffer = malloc(strlen(read_se) + strlen(read_pe) + 1);
    char *p_buffer = buffer;

    const char *p_read = read_se;
    while (*p_read != '\0')
        *(++p_buffer) = *(++p_read);

    p_read = read_pe;
    while (*p_read != '\0')
        *(++p_buffer) = *(++p_read);

    *p_buffer = '\0';
    return buffer;
}

static inline float fq_qscore(kseq_t *ks)
{
    float score = 0.0f;
    for (size_t b = 0; b < ks->qual.l; b++) {
        score += (float)(ks->qual.s[b]) - 33;
    }
    return (score / ks->qual.l);
}


static void fq_parse_se(fqparser_t *fq, float qs_thresh)
{
    int l = 0;
    khint_t kiter;

    while ((l = kseq_read(fq->ks_se)) >= 0) {

        fq->count_raw++;

        // check read quality
        float qs = fq_qscore(fq->ks_se);
        if (qs < qs_thresh) {
            fq->count_qual++;
            continue;
        }

        fqvalue_t qvalue = {fq->count_raw, qs};

        int absent;
        kiter = kh_put(khStr, fq->kshash, fq->ks_se->seq.s, &absent);
        if (absent) {
            kh_key(fq->kshash, kiter) = strdup(fq->ks_se->seq.s);
            kh_value(fq->kshash, kiter) = qvalue;
        }
        else {
            fq->count_duplicates++;
            fqvalue_t hvalue = kh_value(fq->kshash, kiter);
            if (hvalue.score < qvalue.score)
                kh_value(fq->kshash, kiter) = qvalue;
        }
    }
}


static void fq_parse_pe(fqparser_t *fq, float qs_thresh)
{
    khint_t kiter;
    while ((kseq_read(fq->ks_se) >= 0) && (kseq_read(fq->ks_pe) >= 0)) {

        fq->count_raw++;

        // check read quality
        float qs = (fq_qscore(fq->ks_se) + fq_qscore(fq->ks_pe)) / 2.0f;
        if (qs < qs_thresh) {
            fq->count_qual++;
            continue;
        }

        fqvalue_t qvalue = {fq->count_raw, qs};
        char *read = kscat(fq->ks_se->seq.s, fq->ks_pe->seq.s);
        int absent;
        kiter = kh_put(khStr, fq->kshash, read, &absent);
        if (absent) {
            kh_key(fq->kshash, kiter) = read;
            kh_value(fq->kshash, kiter) = qvalue;
        }
        else {
            fq->count_duplicates++;
            fqvalue_t hvalue = kh_value(fq->kshash, kiter);
            if (hvalue.score < qvalue.score)
                kh_value(fq->kshash, kiter) = qvalue;
            free(read);
        }

    }
}


int fq_open(fqparser_t *fq, const char *file_fq_se, const char *file_fq_pe)
{
    // single-end file
    fq->fp_se = gzopen(file_fq_se, "r");
    if (!fq->fp_se) {
        fprintf(stderr, "fastqcleaner::error, failed to open %s\n", file_fq_se);
        return 1;
    }

    fq->ks_se = kseq_init(fq->fp_se);
    if (!fq->ks_se) {
        fprintf(stderr, "fastqcleaner::error, failed to allocate kseq\n");
        return 1;
    }

    // paired-end
    if (file_fq_pe != NULL) {
        fq->fp_pe = gzopen(file_fq_pe, "r");
        if (!fq->fp_pe) {
            fprintf(stderr, "fastqcleaner::error, failed to open %s\n", file_fq_pe);
            return 1;
        }

        fq->ks_pe = kseq_init(fq->fp_pe);
        if (!fq->ks_pe) {
            fprintf(stderr, "fastqcleaner::error, failed to allocate kseq pair\n");
            return 1;
        }
    }

    // khash with char key
    fq->kshash = kh_init(khStr);
    if (!fq->kshash) {
        fprintf(stderr, "fastqcleaner::error, failed to allocate khash\n");
        return 1;
    }

    // set counters
    fq->count_raw = 0;
    fq->count_qual = 0;
    fq->count_duplicates = 0;
    fq->count_clean = 0;

    return 0;
}


void fq_close(fqparser_t *fq)
{
    if (fq->kshash) {
        for (khint_t k = 0; k < kh_end(fq->kshash); ++k)
                if (kh_exist(fq->kshash, k))
                    free((char*)kh_key(fq->kshash, k));
        kh_destroy(khStr, fq->kshash);
    }

    if (fq->bitset)
        bs_destroy(fq->bitset);

    if (fq->ks_se)
        kseq_destroy(fq->ks_se);

    if (fq->ks_pe)
        kseq_destroy(fq->ks_pe);

    if (fq->fp_pe)
        gzclose(fq->fp_pe);

    if (fq->fp_se)
        gzclose(fq->fp_se);
}


void fq_parse(fqparser_t *fq, float qs_thresh)
{
    if (fq->fp_pe)
        fq_parse_pe(fq, qs_thresh);
    else
        fq_parse_se(fq, qs_thresh);
}


void fq_mask(fqparser_t *fq)
{

}


void fq_write(fqparser_t *fq, const char *file_tag)
{

}
