#ifndef FQPARSER_H
#define FQPARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "kroundup.h"
#include "nthash.h"

#define BUFFER_SIZE 4096

typedef struct _fqstring {
    size_t l, m;
    char *s;
} fqstring_t;


typedef struct _fqstream {
    gzFile fp;
    unsigned char *uncompressed_block;
    size_t block_offset, block_length;
} fqstream_t;


typedef struct _fqrecord {
    fqstring_t *name;
    fqstring_t *seq;
    fqstring_t *opt;
    fqstring_t *qual;
    fqstream_t *stream;
} fqrecord_t;

fqstring_t *fqstr_init(void);
void fqstr_destroy(fqstring_t *fqstr);

fqstream_t *fqs_init(const char *file_fqgz);
void fqs_destroy(fqstream_t *fqs);
int fqs_getline(fqstream_t *fqs, fqstring_t *fqstr, int delim);

fqrecord_t *fqr_open(const char *file_fqgz);
void fqr_close(fqrecord_t *fqr);
int fqr_read(fqrecord_t *fqr);
void fqr_print(fqrecord_t *fqr);
void fqr_write(fqrecord_t *fqr, gzFile fo);
float fqr_qscore(fqstring_t *qual);
uint64_t fqr_hash(fqstring_t *seq);

#endif // FQPARSER_H
