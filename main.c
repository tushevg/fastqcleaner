#include <stdio.h>
#include <time.h>
#include "auxilary.h"
#include "fqparser.h"
#include "khash.h"
#include "bitset.h"


typedef struct _fqvalue {
    uint32_t id;
    float score;
} fqvalue_t;

KHASH_MAP_INIT_INT64(khStr, fqvalue_t)


uint32_t test_fqparser(const char *file_fqgz, khash_t(khStr) *kshash)
{
    uint32_t count_raw = 0;
    uint32_t count_qual = 0;
    uint32_t count_dupl = 0;
    uint32_t count_clean = 0;
    khint_t kiter;
    fqrecord_t *fqr = fqr_open(file_fqgz);
    while (fqr_read(fqr) == 0) {

        count_raw++;
        float qscore = fqr_qscore(fqr->qual);

        if (qscore < 30.0f) {
            count_qual++;
            continue;
        }

        uint64_t key = fqr_hash(fqr->seq);
        fqvalue_t val = {count_raw, qscore};
        int absent;
        kiter = kh_put(khStr, kshash, key, &absent);
        if (absent) {
            count_clean++;
            kh_key(kshash, kiter) = key;
            kh_value(kshash, kiter) = val;
        }
        else {
            count_dupl++;
            fqvalue_t val_old = kh_value(kshash, kiter);
            if (val_old.score < val.score)
                kh_value(kshash, kiter) = val;
        }


    }

    int val_test = (count_raw == (count_dupl + count_qual + count_clean)) ? 1 : 0;
    fprintf(stderr, "READS\tCOUNT\n");
    fprintf(stderr, "raw\t%d\n", count_raw);
    fprintf(stderr, "qual\t%d\n", count_qual);
    fprintf(stderr, "dupl\t%d\n", count_dupl);
    fprintf(stderr, "clean\t%d\n", count_clean);
    fprintf(stderr, "test\t%d\n", val_test);

    fqr_close(fqr);
    return count_raw;
}

int main(const int argc, const char *argv[])
{
    //aux_config_t aux;

    // parse auxiliary configuration
    //if (aux_parse(&aux, argc, argv) != 0)
    //    return 1;

    const char *file_fqgz = "/Users/tushevg/Desktop/SepiaTranscriptome/test/SingleEnd.fastq.gz";
    clock_t tic;
    double cpu_time_used;
    bitset_t *bs;
    khash_t(khStr) *kshash = kh_init(khStr);

    tic = clock();
    uint32_t count_raw = test_fqparser(file_fqgz, kshash);
    tic = clock() - tic;
    cpu_time_used = ((double)tic)/CLOCKS_PER_SEC;
    fprintf(stderr, "fqparser took %f sec.\n", cpu_time_used);

    bs = bs_init((size_t)count_raw);

    tic = clock();
    for (khint_t k = kh_begin(kshash); k != kh_end(kshash); ++k) {
        if (kh_exist(kshash, k)) {
            fqvalue_t value = kh_value(kshash, k);
            bs_set(bs, value.id - 1);
        }
    }
    tic = clock() - tic;
    cpu_time_used = ((double)tic)/CLOCKS_PER_SEC;
    fprintf(stderr, "hash to bitset took %f sec.\n", cpu_time_used);

    if (bs)
        bs_destroy(bs);

    if (kshash)
        kh_destroy(khStr, kshash);


    return 0;
}
