#include "auxilary.h"

static const char VERSION[] = "v0.01";
static const char PROJECT_NAME[] = "fastqcleaner";
static const char AUX_DEFAULT_OUTPUT[] = "fqclean";
static const char AUX_WRITE_EXT_COUNTERS[] = "_counts.txt";
static const char AUX_WRITE_EXT_SE[] = ".fastq.gz";
static const char AUX_WRITE_EXT_PE1[] = "_1.fastq.gz";
static const char AUX_WRITE_EXT_PE2[] = "_2.fastq.gz";
static const float AUX_DEFAULT_QTHRESH = 30.0;

/* --- allocate / clean --- */

aux_data_t *aux_init_data(void)
{
    aux_data_t *data = (aux_data_t *)malloc(sizeof(aux_data_t));
    if (!data) return 0;
    data->kshash = kh_init(khStr);
    if (!data->kshash) {
        free(data);
        return 0;
    }
    return data;
}


void aux_destroy_data(aux_data_t *data)
{
    if (!data) return;
    if (data->kshash) kh_destroy(khStr, data->kshash);
    if (data->bs) bs_destroy(data->bs);
    free(data);
}


aux_counters_t *aux_init_counters(void)
{
    aux_counters_t *counters = (aux_counters_t *)calloc(1, sizeof(aux_counters_t));
    if (!counters) return 0;
    return counters;
}


void aux_destory_counters(aux_counters_t *counters)
{
    if (!counters) return;
    free(counters);
}


aux_config_t *aux_init(void)
{
    aux_config_t *aux = (aux_config_t*)calloc(1, sizeof(aux_config_t));
    if (!aux) return 0;

    aux->counters = aux_init_counters();
    if (!aux->counters) {
        free(aux);
        return 0;
    }

    aux->data = aux_init_data();
    if (!aux->data) {
        aux_destory_counters(aux->counters);
        free(aux);
        return 0;
    }

    aux->tag_output = AUX_DEFAULT_OUTPUT;
    aux->qthresh = AUX_DEFAULT_QTHRESH;

    return aux;
}


void aux_destroy(aux_config_t *aux)
{
    if (!aux) return;
    if (aux->data) aux_destroy_data(aux->data);
    if (aux->counters) aux_destory_counters(aux->counters);
    free(aux);
}


/* --- engine body --- */

void aux_parse_fqrecord(khash_t(khStr) *kshash, aux_counters_t *counters, fqrecord_t *fqr, float qthresh)
{
    counters->raw++;
    float qscore = fqr_qscore(fqr->qual);
    if (qscore < qthresh) {
        counters->qual++;
        return;
    }

    uint64_t key = fqr_hash(fqr->seq);
    aux_value_t val = {counters->raw, qscore};
    int absent;
    khint_t kiter = kh_put(khStr, kshash, key, &absent);
    if (absent) {
        counters->clean++;
        kh_key(kshash, kiter) = key;
        kh_value(kshash, kiter) = val;
    }
    else {
        counters->dupl++;
        aux_value_t val_old = kh_value(kshash, kiter);
        if (val_old.score < val.score)
            kh_value(kshash, kiter) = val;
    }
}


int aux_parse_se(aux_config_t *aux)
{
    int code = AUX_CODE_OK;

    fqstream_t *fqs = fqs_open(aux->file_single);
    if (!fqs) return AUX_CODE_ERROR_READ_SE;

    fqrecord_t *fqr = fqr_init();
    while ((code = fqr_read_se(fqr, fqs)) >= 0)
        aux_parse_fqrecord(aux->data->kshash, aux->counters, fqr, aux->qthresh);

    fqr_destroy(fqr);
    fqs_close(fqs);

    return code;
}


int aux_parse_pe(aux_config_t *aux)
{
    int code = AUX_CODE_OK;

    fqstream_t *fqs_1 = fqs_open(aux->file_single);
    if (!fqs_1) return AUX_CODE_ERROR_READ_SE;

    fqstream_t *fqs_2 = fqs_open(aux->file_paired);
    if (!fqs_2) {
        fqs_close(fqs_1);
        return AUX_CODE_ERROR_READ_PE;
    }

    fqrecord_t *fqr = fqr_init();
    while ((code = fqr_read_pe(fqr, fqs_1, fqs_2)) >= 0)
        aux_parse_fqrecord(aux->data->kshash, aux->counters, fqr, aux->qthresh);

    fqr_destroy(fqr);
    fqs_close(fqs_1);
    fqs_close(fqs_2);
    return code;
}


int aux_bitmask(aux_config_t *aux)
{
    aux->data->bs = bs_init(aux->counters->raw);
    for (khint_t k = kh_begin(aux->data->kshash); k != kh_end(aux->data->kshash); ++k) {
        if (kh_exist(aux->data->kshash, k)) {
            aux_value_t value = kh_value(aux->data->kshash, k);
            bs_set(aux->data->bs, value.id - 1);
        }
    }

    return AUX_CODE_OK;
}


int aux_write_counters(aux_config_t *aux)
{
    char *file_out = aux_filename(aux->tag_output, AUX_WRITE_EXT_COUNTERS);

    FILE *fo = fopen(file_out, "w");
    if (!fo) {
        free(file_out);
        return AUX_CODE_ERROR_WRITE_COUNTS;
    }

    int check_sum = (aux->counters->raw == (aux->counters->dupl +
                                            aux->counters->qual +
                                            aux->counters->clean)) ? 1 : 0;

    fprintf(fo, "READS\tCOUNT\n");
    fprintf(fo, "raw\t%d\n", aux->counters->raw);
    fprintf(fo, "qual\t%d\n", aux->counters->qual);
    fprintf(fo, "dupl\t%d\n", aux->counters->dupl);
    fprintf(fo, "clean\t%d\n", aux->counters->clean);
    fprintf(fo, "check\t%d\n", check_sum);
    fclose(fo);
    free(file_out);

    return AUX_CODE_OK;
}


int aux_write_se(aux_config_t *aux)
{
    int code;
    code = aux_write_fastq(aux->tag_output,
                           AUX_WRITE_EXT_SE,
                           aux->file_single,
                           aux->data->bs,
                           AUX_CODE_ERROR_WRITE_SE,
                           AUX_CODE_ERROR_READ_SE);
    return code;
}


int aux_write_pe(aux_config_t *aux)
{
    int code;
    code = aux_write_fastq(aux->tag_output,
                           AUX_WRITE_EXT_PE1,
                           aux->file_single,
                           aux->data->bs,
                           AUX_CODE_ERROR_WRITE_PE,
                           AUX_CODE_ERROR_READ_PE);

    if (code < AUX_CODE_EOF) return code;

    code = aux_write_fastq(aux->tag_output,
                           AUX_WRITE_EXT_PE2,
                           aux->file_paired,
                           aux->data->bs,
                           AUX_CODE_ERROR_WRITE_PE,
                           AUX_CODE_ERROR_READ_PE);

    return code;
}


int aux_write_fastq(const char *tag_out,
                    const char *ext_out,
                    const char *file_fqgz,
                    bitset_t *bs,
                    int aux_error_write,
                    int aux_error_read)
{
    int code = AUX_CODE_OK;

    char *file_out = aux_filename(tag_out, ext_out);
    if (!file_out) return aux_error_write;

    gzFile fo = gzopen(file_out, "wb");
    if (!fo) return aux_error_write;

    fqstream_t *fqs = fqs_open(file_fqgz);
    if (!fqs) return aux_error_read;

    fqrecord_t *fqr = fqr_init();
    size_t counter = 0;
    while ((code = fqr_read_se(fqr, fqs)) >= 0) {

        if (bs_get(bs, counter++))
            fqr_write(fqr, fo);

    }

    fqr_destroy(fqr);
    fqs_close(fqs);
    gzclose(fo);
    free(file_out);

    return code;
}

/* --- auxilary functions --- */

char *aux_filename(const char *path, const char *name)
{
    size_t size_path = strlen(path);
    size_t size_name = strlen(name);
    size_t size_file = size_path + size_name + 1;

    char *file_out = (char *)calloc(size_file, sizeof(char));
    if (!file_out) return 0;
    memccpy(file_out, path, '\0', size_path);
    memccpy(file_out + size_path, name, '\0', size_name);
    file_out[size_file - 1] = 0;
    return file_out;
}


int aux_param_check(const char *param, const char *param_expected)
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
    return AUX_CODE_EXIT;
}


int aux_parse(aux_config_t *aux, int argc, const char **argv)
{
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
            aux->qthresh = strtof(argv[i], NULL);
        }
        else {
            fprintf(stderr, "%s::error, unknown parameter %s\n", PROJECT_NAME, argv[i]);
            return aux_help();
        }
    }

    return 0;
}


void aux_check_error(aux_config_t *aux, int code)
{
    switch(code) {
    case AUX_CODE_ERROR_GZFAILED:
        fprintf(stderr, "fastqcleaner::error, decompression error.\n");
        break;
    case AUX_CODE_ERROR_FQTRUNCATED:
        fprintf(stderr, "fastqcleaner::error, FASTQ record is truncated.\n");
        break;
    case AUX_CODE_ERROR_READ_SE:
        fprintf(stderr, "fastqcleaner::error, failed to read file %s\n", aux->file_single);
        break;
    case AUX_CODE_ERROR_READ_PE:
        fprintf(stderr, "fastqcleaner::error, failed to read file %s\n", aux->file_paired);
        break;
    case AUX_CODE_ERROR_WRITE_COUNTS:
        fprintf(stderr, "fastqcleaner::error, failed to write counts\n%s_counts.txt\n", aux->tag_output);
        break;
    case AUX_CODE_ERROR_WRITE_SE:
        fprintf(stderr, "fastqcleaner::error, failed to write FASTQ\n%s.fastq.gz\n", aux->tag_output);
        break;
    case AUX_CODE_ERROR_WRITE_PE:
        fprintf(stderr, "fastqcleaner::error, failed to write FASTQ\n%s.fastq.gz\n%s\n", aux->tag_output, aux->tag_output);
        break;
    }
}
