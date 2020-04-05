#include "auxilary.h"

static const char VERSION[] = "v0.01";
static const char PROJECT_NAME[] = "fastqcleaner";
static const char AUX_DEFAULT_OUTPUT[] = "fqclean";
static const double AUX_DEFAULT_MAPQ = 30.0;


static int aux_param_check(const char *param, const char *param_expected)
{
    size_t size_param = strlen(param);
    size_t size_param_expected = strlen(param_expected);
    return ((size_param == size_param_expected) &&
            (strncmp(param, param_expected, size_param_expected) == 0)) ? 1 : 0;
}


int aux_help(void)
{
    fprintf(stderr, "\nTool: %s\n", PROJECT_NAME);
    fprintf(stderr, "Version: %s\n", VERSION);
    fprintf(stderr, "Summary:\n");
    fprintf(stderr, "\tclean FASTQ file for low quality reads and PCR duplicates\n");
    fprintf(stderr, "\nUsage:\n");
    fprintf(stderr, "\t%s [Options] -i/--input <fastq>\n", PROJECT_NAME);
    fprintf(stderr, "Parameters:\n");
    fprintf(stderr, "\t-i/--input\tinput FASTQ file\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "\t-p/--pair\tinput paired FASTQ file\n");
    fprintf(stderr, "\t-o/--output\toutput file tag\n");
    fprintf(stderr, "\t-q/--quality\tminimum average read quality\n");
    fprintf(stderr, "\t-h/--help\tprint help message\n");
    return 1;
}


int aux_parse(aux_config_t *aux, int argc, const char **argv)
{
    aux->file_single = NULL;
    aux->file_paired = NULL;
    aux->tag_output = AUX_DEFAULT_OUTPUT;
    aux->quality_threshold = AUX_DEFAULT_MAPQ;

    if ((argc < 3) || (9 < argc)) {
        fprintf(stderr, "%s:error, incorrect number of arguments\n", PROJECT_NAME);
        return aux_help();
    }

    for (int i = 1; i < argc; i++) {

        if (aux_param_check(argv[i], "-h") ||
            aux_param_check(argv[i], "--help")) {
            return aux_help();
        }
        else if (aux_param_check(argv[i], "-i") ||
                aux_param_check(argv[i], "--input")) {
            i += 1;
            aux->file_single = argv[i];
        }
        else if (aux_param_check(argv[i], "-p") ||
                aux_param_check(argv[i], "--pair")) {
            i += 1;
            aux->file_paired = argv[i];
        }
        else if (aux_param_check(argv[i], "-o")||
                aux_param_check(argv[i], "--output")) {
            i += 1;
            aux->tag_output = argv[i];
        }
        else if (aux_param_check(argv[i], "-q") ||
                aux_param_check(argv[i], "--quality")) {
            i += 1;
            aux->quality_threshold = strtof(argv[i], NULL);
        }
        else {
            fprintf(stderr, "%s::error, unknown parameter %s\n", PROJECT_NAME, argv[i]);
            return aux_help();
        }
    }

    return 0;
}
