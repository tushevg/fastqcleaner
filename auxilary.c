#include "auxilary.h"

static const char VERSION[] = "v0.01";
static const char PROGRAM_NAME[] = "fastqcleaner";
static const char AUX_DEFAULT_OUTPUT[] = "cleanfq";
static const double AUX_DEFAULT_MAPQ = 30.0;


int aux_parse(aux_config_t *aux, int argc, const char **argv) {

    /* initialize default auxiliary values */
    aux->fh_input = NULL;
    aux->fh_pair = NULL;
    aux->fo_tag = AUX_DEFAULT_OUTPUT;
    aux->qual = AUX_DEFAULT_MAPQ;

    if ((argc < 3) || (7 < argc)) {
        fprintf(stderr, "fastqcleaner:error, incorrect number of arguments\n");
        help();
    }

    /* parse input pairs */
    for(int i = 1; i < argc; i++) {

        // current paramter length
        size_t parameterLength = strlen(argv[i]);

        // check each parameter and its value
        if((PARAMETER_CHECK("-h", 2, parameterLength)) ||
           (PARAMETER_CHECK("--help", 6, parameterLength))) {
            return help();
        }
        else if((PARAMETER_CHECK("-v", 2, parameterLength)) ||
                (PARAMETER_CHECK("--version", 9, parameterLength))) {
            return version();
        }
        else if((PARAMETER_CHECK("-i", 2, parameterLength)) ||
                (PARAMETER_CHECK("--input", 7, parameterLength))) {
            i += 1;
            aux->fh_input = argv[i];
        }
        else if((PARAMETER_CHECK("-p", 2, parameterLength)) ||
                (PARAMETER_CHECK("--pair", 6, parameterLength))) {
            i += 1;
            aux->fh_pair = argv[i];
        }
        else if((PARAMETER_CHECK("-o", 2, parameterLength)) ||
                (PARAMETER_CHECK("--output", 8, parameterLength))) {
            i += 1;
            aux->fo_tag = argv[i];
        }
        else if((PARAMETER_CHECK("-q", 2, parameterLength)) ||
                (PARAMETER_CHECK("--quality", 9, parameterLength))) {
            i += 1;
            aux->qual = strtod(argv[i], NULL);
        }
        else {
            fprintf(stderr, "Error:: Unknown parameter %s\n", argv[i]);
            return help();
        }

    }

    return 0;
}


int version(void) {
    fprintf(stderr, "PASSFinder %s\n", VERSION);
    return 1;
}


int help(void) {

    fprintf(stderr, "\nTool: fastqcleaner\n");
    fprintf(stderr, "Version: %s\n", VERSION);
    fprintf(stderr, "Summary:\n");
    fprintf(stderr, "\tclean FASTQ file for low quality reads and PCR duplicates\n");
    fprintf(stderr, "\nUsage:\n");
    fprintf(stderr, "\t%s [Options] -i/--input <fastq>\n", PROGRAM_NAME);
    fprintf(stderr, "Parameters:\n");
    fprintf(stderr, "\t-i/--input\tinput FASTQ file\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-p/--pair\tinput paired FASTQ file\n");
    fprintf(stderr, "\t-o/--output\toutput file tag\n");
    fprintf(stderr, "\t-q/--quality\tminimum average read quality\n");
    fprintf(stderr, "\t-h/--help\tprint help message\n");
    fprintf(stderr, "\t-v/--version\tprint current version\n");
    return 1;
}


int chomp(char *line_buf)
{
    size_t  line_length;
    line_length = strlen(line_buf) - 1;
    if(line_buf[line_length] == '\n')
        line_buf[line_length] = '\0';

    return 0;
}
