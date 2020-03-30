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
    gzFile fh_se;
    gzFile fh_pd;
    khash_t(khStr) *kshash;
    bitseq_t *bitset;
    uint32_t count_raw;
    uint32_t count_qual;
    uint32_t count_duplicates;
    uint32_t count_clean;
} fqparser_t;

int fq_open(fqparser_t *fq, const char *file_fq_se, const char *file_fq_pd);
void fq_close(fqparser_t *fq);

int fq_parser(fqparser_t *fq, float qthresh);
int fq_parser_se(fqparser_t *fq, float qthresh);
int fq_parser_pd(fqparser_t *fq, float qthresh);

int fq_hash_to_bitset(fqparser_t *fq);
int fq_stats(fqparser_t *fq, const char *file_tag);

int fq_writer(fqparser_t *fq, const char *file_tag);
int fq_writer_se(fqparser_t *fq, const char *file_tag);
int fq_writer_pd(fqparser_t *fq, const char *file_tag);


#endif // FQPARSER_H
