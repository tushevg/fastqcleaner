/*
 *
 * auxilary.h
 * Author: Georgi Tushev
 * Scientific Computing Facility
 * Max-Planck Institute for Brain Research
 * Frankfurt am Main, Germany
 * Bug reports: sciclist@brain.mpg.de
 *
 */

#ifndef AUXILARY_H
#define AUXILARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fqparser.h"
#include "khash.h"
#include "bitset.h"

// define codes
enum _aux_codes {
  AUX_CODE_OK = 0,
  AUX_CODE_EXIT = -1,
  AUX_CODE_EOF = -1,
  AUX_CODE_ERROR_GZFAILED = -2,
  AUX_CODE_ERROR_FQTRUNCATED = -3,
  AUX_CODE_ERROR_READ_SE = -4,
  AUX_CODE_ERROR_READ_PE = -5,
  AUX_CODE_ERROR_WRITE_COUNTS = -6,
  AUX_CODE_ERROR_WRITE_SE = -7,
  AUX_CODE_ERROR_WRITE_PE = -8
};


// define parameters
typedef struct _aux_value {
    uint32_t id;
    float score;
} aux_value_t;


KHASH_MAP_INIT_INT64(khStr, aux_value_t)


typedef struct _aux_counters {
    uint32_t raw;
    uint32_t qual;
    uint32_t dupl;
    uint32_t clean;
} aux_counters_t;


typedef struct _aux_data {
    bitset_t *bs;
    khash_t(khStr) *kshash;
} aux_data_t;


typedef struct _aux_config_t
{
    const char *file_single;
    const char *file_paired;
    const char *tag_output;
    aux_counters_t *counters;
    aux_data_t *data;
    float qthresh;
} aux_config_t;


// allocate / clean
aux_data_t *aux_init_data(void);
void aux_destroy_data(aux_data_t *data);
aux_counters_t *aux_init_counters(void);
void aux_destory_counters(aux_counters_t *counters);
aux_config_t *aux_init(void);
void aux_destroy(aux_config_t *aux);


// engine body
void aux_parse_fqrecord(khash_t(khStr) *kshash,
                        aux_counters_t *counters,
                        fqrecord_t *fqr,
                        float qthresh);
int aux_parse_se(aux_config_t *aux);
int aux_parse_pe(aux_config_t *aux);
int aux_bitmask(aux_config_t *aux);

int aux_write_counters(aux_config_t *aux);
int aux_write_se(aux_config_t *aux);
int aux_write_pe(aux_config_t *aux);
int aux_write_fastq(const char *tag_out,
                    const char *ext_out,
                    const char *file_fqgz,
                    bitset_t *bs,
                    int aux_error_write,
                    int aux_error_read);


// auxilary fucntions
char *aux_filename(const char *path, const char *name);
int aux_param_check(const char *param, const char *param_expected);
int aux_help(void);
int aux_parse(aux_config_t *aux, int argc, const char **argv);
void aux_check_error(aux_config_t *aux, int code);

#endif // AUXILARY_H
