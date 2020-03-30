#include <stdio.h>
#include <time.h>
#include "auxilary.h"
#include "fqparser.h"
#include "tests.h"

int main(int argc, const char *argv[])
{
    clock_t tic;
    double cpu_time_used;

    int code = 0;
    aux_config_t aux;
    fqparser_t fq;

    // parse auxiliary configuration
    if (aux_parse(&aux, argc, argv) != 0)
        return 1;


    if (fq_open(&fq, aux.fh_input, aux.fh_pair) != 0) {
        code = 1;
        goto clean;
    }

    tic = clock();
    if (fq_parser(&fq, aux.qual) != 0) {
        code = 1;
        goto clean;
    }
    tic = clock() - tic;
    cpu_time_used = ((double)tic)/CLOCKS_PER_SEC;
    printf("fq_parser took %f seconds to execute \n", cpu_time_used);

    tic = clock();
    if (fq_hash_to_bitset(&fq) != 0) {
        code = 1;
        goto clean;
    }
    tic = clock() - tic;
    cpu_time_used = ((double)tic)/CLOCKS_PER_SEC;
    printf("fq_hash_to_bitset took %f seconds to execute \n", cpu_time_used);

    if (fq_stats(&fq, aux.fo_tag) != 0) {
        code = 1;
        goto clean;
    }

    tic = clock();
    if (fq_writer(&fq, aux.fo_tag) != 0) {
        code = 1;
        goto clean;
    }
    tic = clock() - tic;
    cpu_time_used = ((double)tic)/CLOCKS_PER_SEC;
    printf("fq_writer took %f seconds to execute \n", cpu_time_used);

clean:
    fq_close(&fq);

    return code;
}
