#include "fqparser.h"

static inline float fq_qscore(const char *qual)
{
    float score = 0.0f;
    int l = 0;
    while (*qual) {
        l++;
        score += (float)(*qual++) - 33;
    }
    return score / l;
}


static inline char *fq_strcat(const char *seq_se, const char *seq_pd)
{
    char *buffer = malloc(strlen(seq_se) + strlen(seq_pd) + 1);
    char *p_buffer = buffer;

    while (*seq_se)
        *p_buffer++ = *seq_se++;

    while (*seq_pd)
        *p_buffer++ = *seq_pd++;

    *p_buffer = '\0';

    return buffer;
}


static inline void fq_write(gzFile fo, kseq_t *ks)
{
    gzprintf(fo, "@%s %s\n", ks->name.s, ks->comment.s);
    gzprintf(fo, "%s\n", ks->seq.s);
    gzprintf(fo, "+\n");
    gzprintf(fo, "%s\n", ks->qual.s);
}


int fq_open(fqparser_t *fq, const char *file_fq_se, const char *file_fq_pd)
{
    fq->fh_se = gzopen(file_fq_se, "r");
    if (!fq->fh_se) {
        fprintf(stderr, "fastqcleaner::error, failed to open %s\n", file_fq_se);
        return 1;
    }

    if (file_fq_pd) {
        fq->fh_pd = gzopen(file_fq_pd, "r");
        if (!fq->fh_pd) {
            fprintf(stderr, "fastqcleaner::error, failed to open %s\n", file_fq_pd);
            return 1;
        }
    }

    fq->kshash = kh_init(khStr);
    if (!fq->kshash) {
        fprintf(stderr, "fastqcleaner::error, failed to allocate khash\n");
        return 1;
    }

    fq->bitset = NULL;
    fq->count_raw = 0;
    fq->count_qual = 0;
    fq->count_duplicates = 0;
    fq->count_clean = 0;

    return 0;
}


void fq_close(fqparser_t *fq)
{
    if (fq->fh_se)
        gzclose(fq->fh_se);

    if (fq->fh_pd)
        gzclose(fq->fh_pd);

    if (fq->kshash)
        kh_destroy(khStr, fq->kshash);

    if (fq->bitset)
        bs_destroy(fq->bitset);
}


int fq_parser(fqparser_t *fq, float qthresh)
{
    if (fq->fh_pd)
        return fq_parser_pd(fq, qthresh);

    return fq_parser_se(fq, qthresh);
}


int fq_parser_se(fqparser_t *fq, float qthresh)
{
    kseq_t *ks = kseq_init(fq->fh_se);
    khint_t kiter;
    int state = 0;
    while ((state = kseq_read(ks)) >= 0) {
        fq->count_raw++;
        float qscore = fq_qscore(ks->qual.s);

        if (qscore < qthresh) {
            fq->count_qual++;
            continue;
        }

        fqvalue_t value = {fq->count_raw, qscore};
        int absent;
        kiter = kh_put(khStr, fq->kshash, ks->seq.s, &absent);
        if (absent) {
            fq->count_clean++;
            kh_key(fq->kshash, kiter) = strdup(ks->seq.s);
            kh_value(fq->kshash, kiter) = value;
        }
        else {
            fq->count_duplicates++;
            fqvalue_t value_stored = kh_value(fq->kshash, kiter);
            if (value_stored.score < value.score)
                kh_value(fq->kshash, kiter) = value;
        }

    }

    kseq_destroy(ks);
    return 0;
}


int fq_parser_pd(fqparser_t *fq, float qthresh)
{
    kseq_t *ks_se = kseq_init(fq->fh_se);
    kseq_t *ks_pd = kseq_init(fq->fh_pd);

    khint_t kiter;
    int state_se, state_pd = 0;
    while (((state_se = kseq_read(ks_se)) >= 0) &&
           ((state_pd = kseq_read(ks_pd)) >= 0)) {
        fq->count_raw++;
        float qscore = fq_qscore(ks_se->qual.s);
        qscore += fq_qscore(ks_pd->qual.s);
        qscore /= 2.0f;

        if (qscore < qthresh) {
            fq->count_qual++;
            continue;
        }

        fqvalue_t value = {fq->count_raw, qscore};
        char *key = fq_strcat(ks_se->seq.s, ks_pd->seq.s);
        int absent;
        kiter = kh_put(khStr, fq->kshash, key, &absent);
        if (absent) {
            fq->count_clean++;
            kh_key(fq->kshash, kiter) = key;
            kh_value(fq->kshash, kiter) = value;
        }
        else {
            fq->count_duplicates++;
            fqvalue_t value_stored = kh_value(fq->kshash, kiter);
            if (value_stored.score < value.score)
                kh_value(fq->kshash, kiter) = value;
            free(key);
        }

    }

    kseq_destroy(ks_se);
    kseq_destroy(ks_pd);
    return 0;
}


int fq_hash_to_bitset(fqparser_t *fq)
{
    fq->bitset = bs_init(fq->count_raw);
    if (!fq->bitset) {
        fprintf(stderr, "fastqcleaner::error, failed to allocate bitset\n");
        return 1;
    }

    for (khint_t k = kh_begin(fq->kshash); k != kh_end(fq->kshash); ++k) {
        if (kh_exist(fq->kshash, k)) {
            fqvalue_t value = kh_value(fq->kshash, k);
            bs_set(fq->bitset, value.id - 1);
            free((char *)kh_key(fq->kshash, k));
        }
    }

    return 0;
}


int fq_stats(fqparser_t *fq, const char *file_tag)
{
    char *file_out = fq_strcat(file_tag, "_counts.txt");
    FILE *fo = fopen(file_out, "w");
    if (!fo) {
        fprintf(stderr, "fastqcleaner::error, failed to create file %s\n", file_out);
        return 1;
    }

    uint32_t test_raw = fq->count_qual + fq->count_duplicates + fq->count_clean;
    int check = (fq->count_raw == test_raw) ? 1 : 0;
    fprintf(fo, "type\tcounts\n");
    fprintf(fo, "raw\t%d\n", fq->count_raw);
    fprintf(fo, "filtered\t%d\n", fq->count_qual);
    fprintf(fo, "duplicated\t%d\n", fq->count_duplicates);
    fprintf(fo, "clean\t%d\n", fq->count_clean);
    fprintf(fo, "check\t%d\n", check);

    fclose(fo);
    free(file_out);
    return 0;
}


int fq_writer(fqparser_t *fq, const char *file_tag)
{
    if (fq->fh_pd)
        return fq_writer_pd(fq, file_tag);

    return fq_writer_se(fq, file_tag);
}


int fq_writer_se(fqparser_t *fq, const char *file_tag)
{
    char *file_out = fq_strcat(file_tag, ".fastq.gz");
    gzFile fo = gzopen(file_out, "wb");

    gzrewind(fq->fh_se);
    kseq_t *ks = kseq_init(fq->fh_se);
    int state = 0;
    size_t key = 0;
    while ((state = kseq_read(ks)) >= 0) {

        if (bs_exist(fq->bitset, key) == 1)
            fq_write(fo, ks);

        key++;
    }

    kseq_destroy(ks);
    gzclose(fo);
    free(file_out);
    return 0;
}


int fq_writer_pd(fqparser_t *fq, const char *file_tag)
{
    char *file_out_se = fq_strcat(file_tag, "_1.fastq.gz");
    char *file_out_pd = fq_strcat(file_tag, "_2.fastq.gz");

    gzFile fo_se = gzopen(file_out_se, "wb");
    gzFile fo_pd = gzopen(file_out_pd, "wb");

    gzrewind(fq->fh_se);
    gzrewind(fq->fh_pd);

    kseq_t *ks_se = kseq_init(fq->fh_se);
    kseq_t *ks_pd = kseq_init(fq->fh_pd);

    int state_se, state_pd = 0;
    size_t key = 0;
    while (((state_se = kseq_read(ks_se)) >= 0) &&
           ((state_pd = kseq_read(ks_pd)) >= 0)) {

        if (bs_exist(fq->bitset, key) == 1) {
            fq_write(fo_se, ks_se);
            fq_write(fo_pd, ks_pd);
        }

        key++;
    }


    gzclose(fo_se);
    gzclose(fo_pd);
    free(file_out_se);
    free(file_out_pd);

    return 0;
}
