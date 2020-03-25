#ifndef FQRECORD_H
#define FQRECORD_H

#include <stdio.h>
#include <stdlib.h>
#include <htslib/kstring.h>
#include <htslib/hts.h>

typedef struct _fq_t {
    kstring_t hed;
    kstring_t seq;
    kstring_t opt;
    kstring_t qual;
} fq_t;


void fq_init(fq_t *);
void fq_destroy(fq_t *);
int fq_read(fq_t *, htsFile *);

void fq_print(fq_t *);


#endif // FQRECORD_H
