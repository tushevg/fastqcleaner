#include <stdio.h>
#include <time.h>
#include "auxilary.h"
#include "fqparser.h"
#include "khash.h"
#include "bitset.h"


typedef struct _fqcounters {
    uint32_t raw;
    uint32_t qual;
    uint32_t dupl;
    uint32_t clean;
} fqcounters_t;


typedef struct _fqvalue {
    uint32_t id;
    float score;
} fqvalue_t;


KHASH_MAP_INIT_INT64(khStr, fqvalue_t)


static inline char *filename(const char *path, const char *name) {
    size_t size_path = strlen(path);
    size_t size_name = strlen(name);
    size_t size_file = size_path + size_name + 1;
    char *file_out = (char *)malloc(size_file * sizeof(char));
    if (!file_out) return 0;
    strncpy(file_out, path, size_path);
    strncat(file_out + size_path, name, size_name);
    file_out[size_file - 1] = 0;
    return file_out;
}




void parse_fqrecord(khash_t(khStr) *kshash, fqcounters_t *counters, fqrecord_t *fqr, float qthresh)
{
    counters->raw++;
    float qscore = fqr_qscore(fqr->qual);
    if (qscore < qthresh) {
        counters->qual++;
        return;
    }

    uint64_t key = fqr_hash(fqr->seq);
    fqvalue_t val = {counters->raw, qscore};
    int absent;
    khint_t kiter = kh_put(khStr, kshash, key, &absent);
    if (absent) {
        counters->clean++;
        kh_key(kshash, kiter) = key;
        kh_value(kshash, kiter) = val;
    }
    else {
        counters->dupl++;
        fqvalue_t val_old = kh_value(kshash, kiter);
        if (val_old.score < val.score)
            kh_value(kshash, kiter) = val;
    }
}

/*
 * >= 0 (normal) record size
 * -1 eof reached
 * -2 truncated error
 * -3 failed to read file
 */
int parse_single_end(khash_t(khStr) *kshash, fqcounters_t *counters, const char *file_fqgz, float qthresh)
{
    int state = 0;
    fqstream_t *fqs = fqs_open(file_fqgz);
    if (!fqs) {
        fprintf(stderr, "fastqcleaner::error, failed to read %s\n", file_fqgz);
        return -3;
    }

    fqrecord_t *fqr = fqr_init();
    while ((state = fqr_read_se(fqr, fqs)) >= 0)
        parse_fqrecord(kshash, counters, fqr, qthresh);

    fqr_destroy(fqr);
    fqs_close(fqs);

    return state;
}

/*
 * >= 0 (normal) record size
 * -1 eof reached
 * -2 truncated error
 * -3 failed to read file
 */
int parse_paired_end(khash_t(khStr) *kshash, fqcounters_t *counters, const char *file_fqgz_1, const char *file_fqgz_2, float qthresh)
{
    int state = 0;

    fqstream_t *fqs_1 = fqs_open(file_fqgz_1);
    if (!fqs_1) {
        fprintf(stderr, "fastqcleaner::error, failed to read %s\n", file_fqgz_1);
        return -3;
    }

    fqstream_t *fqs_2 = fqs_open(file_fqgz_2);
    if (!fqs_1) {
        fprintf(stderr, "fastqcleaner::error, failed to read %s\n", file_fqgz_1);
        fqs_close(fqs_1);
        return -3;
    }

    fqrecord_t *fqr = fqr_init();
    while ((state = fqr_read_pe(fqr, fqs_1, fqs_2)) >= 0)
        parse_fqrecord(kshash, counters, fqr, qthresh);

    fqr_destroy(fqr);
    fqs_close(fqs_1);
    fqs_close(fqs_2);
    return state;
}


int print_counters(const char *tag_output, const char *ext_output, fqcounters_t *counters)
{
    char file_out[256];
    memset(file_out, '\0', sizeof(file_out));
    size_t size_tag = strlen(tag_output);
    size_t size_ext = strlen(ext_output);
    strncpy(file_out, tag_output, size_tag);
    strncpy(file_out + size_tag, ext_output, size_ext);


    FILE *fo = fopen(file_out, "w");
    if (!fo) {
        free(file_out);
        return -1;
    }
    int check_sum = (counters->raw == (counters->dupl + counters->qual + counters->clean)) ? 1 : 0;

    fprintf(fo, "READS\tCOUNT\n");
    fprintf(fo, "raw\t%d\n", counters->raw);
    fprintf(fo, "qual\t%d\n", counters->qual);
    fprintf(fo, "dupl\t%d\n", counters->dupl);
    fprintf(fo, "clean\t%d\n", counters->clean);
    fprintf(fo, "check\t%d\n", check_sum);

    fclose(fo);
    return 0;
}


int print_fq(const char *tag_output, const char *ext_output, const char *file_fqgz, bitset_t *bs) {

    int state = 0;
    char *file_out = filename(tag_output, ext_output);
    if (!file_out) return -4;

    gzFile fo = gzopen(file_out, "wb");
    if (!fo) return -4;


    fqstream_t *fqs = fqs_open(file_fqgz);
    if (!fqs) {
        fprintf(stderr, "fastqcleaner::error, failed to read %s\n", file_fqgz);
        return -3;
    }

    fqrecord_t *fqr = fqr_init();
    size_t counter = 0;
    while ((state = fqr_read_se(fqr, fqs)) >= 0) {
        if (bs_get(bs, counter++)) {
            gzprintf(fo, "%s\n", fqr->name->s);
            gzprintf(fo, "%s\n", fqr->seq->s);
            gzprintf(fo, "+\n");
            gzprintf(fo, "%s\n", fqr->qual->s);
        }
    }

    fqr_destroy(fqr);
    fqs_close(fqs);
    gzclose(fo);
    free(file_out);
    return state;
}


int main(const int argc, const char *argv[])
{
    int state = 0;
    aux_config_t aux;
    fqcounters_t counters = {0, 0, 0, 0};
    bitset_t *bs = 0;
    khash_t(khStr) *kshash = kh_init(khStr);
    clock_t tic;
    double cpu_time_used;

    // parse auxiliary configuration
    if (aux_parse(&aux, argc, argv) != 0)
        return 1;

    // read files
    tic = clock();
    if (aux.file_paired) {

        if ((state = parse_paired_end(kshash, &counters, aux.file_single, aux.file_paired, aux.quality_threshold)) < -1)
            goto clean;

    } else {

        if ((state = parse_single_end(kshash, &counters, aux.file_single, aux.quality_threshold)) < -1)
            goto clean;

    }
    tic = clock() - tic;
    cpu_time_used = ((double)tic)/CLOCKS_PER_SEC;
    fprintf(stderr, "fqparser %f sec.\n", cpu_time_used);


    // print result
    if (print_counters(aux.tag_output, "_counts.txt", &counters) != 0) {
        fprintf(stderr, "fastqcleaner::error, failed to write out counters.\n");
        goto clean;
    }

    // create bit mask
    tic = clock();
    bs = bs_init(counters.raw);
    for (khint_t k = kh_begin(kshash); k != kh_end(kshash); ++k) {
        if (kh_exist(kshash, k)) {
            fqvalue_t value = kh_value(kshash, k);
            bs_set(bs, value.id - 1);
        }
    }
    tic = clock() - tic;
    cpu_time_used = ((double)tic)/CLOCKS_PER_SEC;
    fprintf(stderr, "bitmask %f sec.\n", cpu_time_used);


    // write results
    tic = clock();
    if (aux.file_paired) {

        if ((state = print_fq(aux.tag_output, "_1.fastq.gz", aux.file_single, bs)) < -1)
            goto clean;

        if ((state = print_fq(aux.tag_output, "_2.fastq.gz", aux.file_paired, bs)) < -1)
            goto clean;

    }
    else {

        if ((state = print_fq(aux.tag_output, ".fastq.gz", aux.file_single, bs)) < -1)
            goto clean;

    }
    tic = clock() - tic;
    cpu_time_used = ((double)tic)/CLOCKS_PER_SEC;
    fprintf(stderr, "writing %f sec.\n", cpu_time_used);

clean:

    switch (state) {
    case -2:
        fprintf(stderr, "fastqcleaner::error, truncated record.\n");
        break;
    case -3:
        fprintf(stderr, "fastqcleaner::error, failed to read file.\n");
        break;
    case -4:
        fprintf(stderr, "fastqcleaner::error, failed to write file.\n");
        break;
    }

    if (bs)
        bs_destroy(bs);

    if (kshash)
        kh_destroy(khStr, kshash);

    return state;
}
