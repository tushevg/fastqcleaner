#include <stdio.h>
#include "auxilary.h"
#include "tests.h"
#include "fqparser.h"

// http://attractivechaos.github.io/klib/#Kseq%3A%20stream%20buffer%20and%20FASTA%2FQ%20parser


int main(int argc, const char *argv[])
{
    aux_config_t aux;
    fqparser_t fq;

    //if (run_tests() > 0)
    //    return 0;

    // parse auxiliary configuration
    if (aux_parse(&aux, argc, argv) != 0)
        return 1;

    if (fq_open(&fq, aux.fh_input, aux.fh_pair) != 0) {
        fq_close(&fq);
        return 1;
    }


    fq_parse(&fq, aux.qual);

    fq_mask(&fq);

    fq_write(&fq, aux.fo_tag);

    fq_close(&fq);


    return 0;
}
