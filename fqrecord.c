#include "fqrecord.h"

void fq_init(fq_t *record) {
    ks_initialize(&record->hed);
    ks_initialize(&record->seq);
    ks_initialize(&record->opt);
    ks_initialize(&record->qual);
}


void fq_destroy(fq_t *record) {
    ks_free(&record->hed);
    ks_free(&record->seq);
    ks_free(&record->opt);
    ks_free(&record->qual);
}


int fq_read(fq_t *record, htsFile *fh) {

    int ret = 0;

    if (hts_getline(fh, '\n', &record->hed) >= 0) ret++;
    if (hts_getline(fh, '\n', &record->seq) >= 0) ret++;
    if (hts_getline(fh, '\n', &record->opt) >= 0) ret++;
    if (hts_getline(fh, '\n', &record->qual) >= 0) ret++;

    return ret;
}


double fq_score(fq_t *record) {
    double score = 0.0;

    for (size_t b = 0; b < record->qual.l; b++) {
        score += (double)(record->qual.s[b]) - 33.0;
    }
    return (score / record->qual.l);
}


void fq_print(fq_t *record) {
    printf("%s\n", record->hed.s);
    printf("%s\n", record->seq.s);
    printf("%s\n", record->opt.s);
    printf("%s\n", record->qual.s);
}
