#ifndef AUXILARY_H
#define AUXILARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// define parameters
typedef struct _aux_config_t
{
    const char *fh_input;
    const char *fh_pair;
    const char *fo_tag;
    double qual;
} aux_config_t;

int aux_help(void);
int aux_parse(aux_config_t *aux, int argc, const char **argv);

#endif // AUXILARY_H
