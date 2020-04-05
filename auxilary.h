#ifndef AUXILARY_H
#define AUXILARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define parameters
typedef struct _aux_config_t
{
    const char *file_single;
    const char *file_paired;
    const char *tag_output;
    float quality_threshold;
} aux_config_t;

int aux_help(void);
int aux_parse(aux_config_t *aux, int argc, const char **argv);

#endif // AUXILARY_H
