#include <stdio.h>
#include "auxilary.h"
#include "htsutils.h"
#include "tests.h"



int main(int argc, const char *argv[])
{
    aux_config_t aux;
    htsu_config_t hts;

    if (run_tests() > 0) {
        return 0;
    }

    // parse auxiliary configuration
    if (aux_parse(&aux, argc, argv) != 0) {
        fprintf(stderr, "fastqcleaner::error, failed to parse input");
        return 1;
    }

    if (htsu_open(&hts, aux.fh_input, aux.fh_pair) != 0) {
        fprintf(stderr, "fastqcleaner::error, failed to open FASTQ file");
        return 1;
    }

    htsu_parse(&hts, aux.qual);
    //
    htsu_destroy(&hts);

    return 0;
}
