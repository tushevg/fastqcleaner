#include <stdio.h>
#include <time.h>
#include "auxilary.h"

clock_t tic(void)
{
    return clock();
}


void toc(clock_t tic, const char *tag)
{
    clock_t toc = clock() - tic;
    double cpu_time_used = ((double)toc)/CLOCKS_PER_SEC;
    fprintf(stderr, "%s %f sec.\n", tag, cpu_time_used);

}


int main(const int argc, const char *argv[])
{
    int code = 0;
    clock_t t;

    aux_config_t *aux = aux_init();
    if (!aux) {
        fprintf(stderr, "fastqcleaner::error, failed to allocate engine.\n");
        return AUX_CODE_EXIT;
    }


    // parse auxiliary configuration
    if ((code = aux_parse(aux, argc, argv)) != AUX_CODE_OK)
        goto clean;


    // parse FASTQ file
    t = tic();
    if (aux->file_paired) {

        if ((code = aux_parse_pe(aux)) < AUX_CODE_EOF)
            goto clean;

    } else {

        if ((code = aux_parse_se(aux)) < AUX_CODE_EOF)
            goto clean;

    }
    toc(t, "parse FASTQ");


    // print counters
    if ((code = aux_write_counters(aux)) != AUX_CODE_OK)
        goto clean;


    // create bitmask
    t = tic();
    if ((code = aux_bitmask(aux)) != AUX_CODE_OK)
        goto clean;
    toc(t, "create bitmask");


    // print FASTQ
    t = tic();
    if (aux->file_paired) {

        if ((code = aux_write_pe(aux)) < AUX_CODE_EOF)
            goto clean;

    }
    else {

        if ((code = aux_write_se(aux)) < AUX_CODE_EOF)
            goto clean;

    }
    toc(t, "print FASTQ");


clean:
    aux_check_error(aux, code);
    aux_destroy(aux);

    return 0;
}
