#include <stdio.h>
#include "auxilary.h"
#include "htsutils.h"

int main(int argc, const char *argv[])
{
    aux_config_t aux;
    hts_config_t hts;

    // parse auxiliary configuration
    aux_parse(&aux, argc, argv);

    hts_init(&hts, &aux);
    hts_parse(&hts, &aux);
    hts_destroy(&hts);

    return 0;
}
