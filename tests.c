#include "tests.h"

static float test_qscore(const char *qual_se, const char *qual_pd)
{
    float score = 0.0f;
    int l = 0;
    while(*qual_se) {
        l++;
        score += (*qual_se++) - 33;
    }

    while(*qual_pd) {
        l++;
        score += (*qual_pd++) - 33;
    }


    return score / l;
}


int run_tests(void)
{
    int result = 0;
    result += test_bs_packseq();
    result += test_bs_bitset();
    //result += test_kseq_writer();
    result += test_kseq_qscore();
    return result;
}


int test_bs_packseq(void)
{
    int result = 0;
    const char *dna = "ACGT";
    bitseq_t *bs = bs_pack(dna);
    result = (bs->bitseq[0] == 30) ? 1 : 0;
    bs_print(bs);
    printf("bit%d = %d\n", 3, bs_exist(bs, 3));

    bs_clear(bs, 3);
    bs_print(bs);
    printf("bit%d = %d\n", 3, bs_exist(bs, 3));

    bs_set(bs, 3);
    bs_print(bs);
    printf("bit%d = %d\n", 3, bs_exist(bs, 3));

    bs_destroy(bs);
    return result;
}


int test_bs_bitset(void)
{
    int result = 0;
    return result;
}


int test_kseq_writer(void)
{
    const char *file_name = "/Users/tushevg/Desktop/SepiaTranscriptome/test/SingleEnd.fastq.gz";

    gzFile fp = gzopen(file_name, "r");
    if (!fp) {
        fprintf(stderr, "fastqcleaner::error, failed to open %s\n", file_name);
        return 1;
    }

    int l,n = 0;
    kseq_t *ks = kseq_init(fp);
    while((l = kseq_read(ks)) >= 0) {
        n++;
        printf("@%s %s\n", ks->name.s, ks->comment.s);
        printf("%s\n", ks->seq.s);
        printf("+\n");
        printf("%s\n", ks->qual.s);

        if (n == 3)
            break;
    }
    kseq_destroy(ks);

    gzrewind(fp);

    n = 0;

    const char *file_out = "/Users/tushevg/Desktop/temp.fastq.gz";
    gzFile fo = gzopen(file_out, "wb");

    ks = kseq_init(fp);
    while((l = kseq_read(ks)) >= 0) {
        n++;

        gzprintf(fo, "@%s %s\n", ks->name.s, ks->comment.s);
        gzprintf(fo, "%s\n", ks->seq.s);
        gzprintf(fo, "+\n");
        gzprintf(fo, "%s\n", ks->qual.s);

        if (n == 3)
            break;
    }
    kseq_destroy(ks);



    // clean
    gzclose(fp);
    gzclose(fo);

    return 0;
}


int test_kseq_qscore(void)
{
    const char *file_name = "/Users/tushevg/Desktop/SepiaTranscriptome/test/SingleEnd.fastq.gz";

    gzFile fp = gzopen(file_name, "r");
    if (!fp) {
        fprintf(stderr, "fastqcleaner::error, failed to open %s\n", file_name);
        return 1;
    }

    int l,n = 0;
    kseq_t *ks = kseq_init(fp);
    while((l = kseq_read(ks)) >= 0) {
        n++;

        // print record
        printf("@%s %s\n", ks->name.s, ks->comment.s);
        printf("%s\n", ks->seq.s);
        printf("+\n");
        printf("%s\n", ks->qual.s);

        printf("%.2f\n", test_qscore(ks->qual.s, 0));

        if (n == 3)
            break;
    }
    kseq_destroy(ks);
    gzclose(fp);
    return 0;
}
