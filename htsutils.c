#include "htsutils.h"

void hts_init(hts_config_t *hts, aux_config_t *aux) {

    hts->fh_input = hts_open(aux->fh_input, "r");
    if (!hts->fh_input) {
        fprintf(stderr, "fastqcleaner::error, could not open input FASTQ file %s\n", aux->fh_input);
        exit(EXIT_FAILURE);
    }

    if (aux->fh_pair) {
        hts->fh_pair = hts_open(aux->fh_input, "r");
        if (!hts->fh_pair) {
            fprintf(stderr, "fastqcleaner::error, could not open paired FASTQ file %s\n", aux->fh_pair);
            exit(EXIT_FAILURE);
        }
    }

}


void hts_destroy(hts_config_t *hts) {
    if (hts->fh_input)
        hts_close(hts->fh_input);

    if (hts->fh_pair)
        hts_close(hts->fh_pair);
}


void hts_parse(hts_config_t *hts, aux_config_t *aux) {

    int ret,n = 0;
    fq_t record;

    fq_init(&record);
    while(fq_read(&record, hts->fh_input) == 4) {
        //printf("%ld-%ld-%ld-%ld\n", record.hed.l, record.seq.l, record.opt.l, record.qual.l);
        n++;

    }
    fq_print(&record);


    fq_destroy(&record);

    /*
    kstring_t line = KS_INITIALIZE;
    int ret, line_id = 0;
    while ((ret = hts_getline(hts->fh_input, '\n', &line)) >= 0) {

        hts_getline(hts->fh_input, '\n', &line);
        hts_getline(hts->fh_input, '\n', &line);
        hts_getline(hts->fh_input, '\n', &line);
        n++;
    }

    if (line.s)
        free(line.s);
    */

    printf("lines %d\n", n);

}
