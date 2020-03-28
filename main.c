#include <stdio.h>
#include "auxilary.h"
#include "htsutils.h"
#include "tests.h"


int main(int argc, const char *argv[])
{
    aux_config_t aux;
    htsu_config_t hts;

    if (run_tests() > 0)
        return 0;

    // parse auxiliary configuration
    if (aux_parse(&aux, argc, argv) != 0)
        return 1;

    // open FASTQ files
    if (htsu_open(&hts, aux.fh_input, aux.fh_pair) != 0)
        return aux_help();

    htsu_parse(&hts, aux.qual);
    //
    htsu_destroy(&hts);

    return 0;
}
