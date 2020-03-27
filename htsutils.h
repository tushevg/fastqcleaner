#ifndef HTSUTILS_H
#define HTSUTILS_H

#include "htslib/hts.h"
#include "htslib/khash.h"
#include "fqrecord.h"
#include "bitseq.h"

typedef struct _htsu_config_t
{
    htsFile *fh_input;
    htsFile *fh_pair;
    htsFile *fo_input;
    htsFile *fo_pair;
} htsu_config_t;

int htsu_open(htsu_config_t *, const char *file_input, const char *file_pair);
//int htsu_writer(htsu_config_t *, const char *file_tag);
void htsu_destroy(htsu_config_t *);
void htsu_parse(htsu_config_t *, double score);

//void hts_dumphash(khash_t(pileup_h) *, bam_hdr_t *, const char);
//void hts_freehash(khash_t(pileup_h) **);
//void hts_printout(hts_config_t *);

#endif // HTSUTILS_H
