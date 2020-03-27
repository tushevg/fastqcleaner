#include "htsutils.h"

int htsu_open(htsu_config_t *hts, const char *file_input, const char *file_pair) {

    hts->fh_input = NULL;
    hts->fh_pair = NULL;
    hts->fo_input = NULL;
    hts->fo_pair = NULL;

    // open input file
    if (!file_input) {
        fprintf(stderr, "fastqcleaner::error, input file not provided.");
        return 1;
    }

    hts->fh_input = hts_open(file_input, "r");
    if (!hts->fh_input) {
        fprintf(stderr, "fastqcleaner::error, could not open input FASTQ file %s\n", file_input);
        return 1;
    }

    // open pair file if requested
    if (file_pair) {
        hts->fh_pair = hts_open(file_pair, "r");
        if (!hts->fh_pair) {
            fprintf(stderr, "fastqcleaner::error, could not open paired FASTQ file %s\n", file_pair);
            return 1;
        }
    }

    return 0;
}


void htsu_destroy(htsu_config_t *hts) {
    if (hts->fh_input)
        hts_close(hts->fh_input);

    if (hts->fh_pair)
        hts_close(hts->fh_pair);

    if (hts->fo_input)
        hts_close(hts->fo_input);

    if (hts->fo_pair)
        hts_close(hts->fo_pair);
}


void htsu_parse(htsu_config_t *hts, double score_qual) {

    size_t record_id = 0;
    fq_t record;

    fq_init(&record);
    while(fq_read(&record, hts->fh_input) == 4) {

        if (fq_score(&record) < score_qual)
            continue;

        // encode sequence to two bit
        bitseq_t *bs = bs_pack(record.seq.s);
        bs_destroy(bs);
        record_id++;
    }

    fq_print(&record);
    fq_destroy(&record);
    printf("lines %ld\n", record_id);
}
