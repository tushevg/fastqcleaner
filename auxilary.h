#ifndef AUXILARY_H
#define AUXILARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdnoreturn.h>

// define MIN & MAX macros
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// define our parameter checking macro
#define PARAMETER_CHECK(param, paramLen, actualLen) (strncmp(argv[i], param, MIN(actualLen, paramLen))== 0) && (actualLen == paramLen)

// define parameters
typedef struct _aux_config_t
{
    const char *fh_input;
    const char *fh_pair;
    const char *fo_tag;
    double qual;
} aux_config_t;


noreturn void help(void);
noreturn void version(void);
int chomp(char *line);

void aux_parse(aux_config_t *aux, int argc, const char **argv);

#endif // AUXILARY_H
