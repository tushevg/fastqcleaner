#include <stdio.h>
#include "auxilary.h"
#include "htsutils.h"

/*
//const char *dna = "ACGTTGCAT";
const char *dna = "CCGGCCCTTTCCGCCGGTGTCAACCGCTTGTACATCCGCCACGCGGTCCTG";

bitseq_t *bs = bs_pack(dna);

for(size_t i = 0; i < bs->size_packed; i++) {
    printf("%ld - %d\n", i, bs->bitseq[i]);
}

printf("size dna %ld\n", bs->size_unpacked * sizeof(char));
printf("size twobit %ld\n", bs->size_packed * sizeof(uint8_t));


bs_destroy(bs);
*/


int main(int argc, const char *argv[])
{
    aux_config_t aux;
    htsu_config_t hts;

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
