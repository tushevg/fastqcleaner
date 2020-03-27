#ifndef FQRECORD_H
#define FQRECORD_H

#include <stdio.h>
#include <stdlib.h>
#include <htslib/hts.h>
#include <htslib/kstring.h>

typedef struct _fq_t {
    kstring_t hed;
    kstring_t seq;
    kstring_t opt;
    kstring_t qual;
} fq_t;


void fq_init(fq_t *);
void fq_destroy(fq_t *);
int fq_read(fq_t *, htsFile *);
double fq_score(fq_t *);
void fq_print(fq_t *);

#endif // FQRECORD_H
