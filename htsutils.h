#ifndef HTSUTILS_H
#define HTSUTILS_H

#include "htslib/hts.h"
#include "htslib/khash.h"
#include "auxilary.h"
#include "fqrecord.h"

typedef struct _hts_config_t
{
    htsFile *fh_input;
    htsFile *fh_pair;
    htsFile *fo_input;
    htsFile *fo_pair;
} hts_config_t;

void hts_init(hts_config_t *, aux_config_t *);
void hts_destroy(hts_config_t *);
void hts_parse(hts_config_t *, aux_config_t *);
//void hts_dumphash(khash_t(pileup_h) *, bam_hdr_t *, const char);
//void hts_freehash(khash_t(pileup_h) **);
//void hts_printout(hts_config_t *);

#endif // HTSUTILS_H
