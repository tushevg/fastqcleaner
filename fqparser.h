#ifndef FQPARSER_H
#define FQPARSER_H

#include <zlib.h>
#include "kseq.h"
#include "khash.h"
#include "bitseq.h"

#include <stdio.h>

typedef struct _fqvalue {
    uint32_t id;
    float score;
} fqvalue_t;

KSEQ_INIT(gzFile, gzread)
KHASH_MAP_INIT_STR(khStr, fqvalue_t)

typedef struct _fqparser {
    gzFile fp_se;
    gzFile fp_pe;
    kseq_t *ks_se;
    kseq_t *ks_pe;
    khash_t(khStr) *kshash;
    bitseq_t *bitset;
    uint32_t count_raw;
    uint32_t count_qual;
    uint32_t count_duplicates;
    uint32_t count_clean;
} fqparser_t;

int fq_open(fqparser_t *fq, const char *file_fq_se, const char *file_fq_pe);
void fq_close(fqparser_t *fq);
void fq_parse(fqparser_t *fq, float qs_thresh);
void fq_mask(fqparser_t *fq);
void fq_write(fqparser_t *fq, const char *file_tag);

#endif // FQPARSER_H
