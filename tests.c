#include "tests.h"

int run_tests(void) {
    int result = 0;

    result += test_bs_packseq();
    result += test_bs_bitset();

    return result;
}


int test_bs_packseq(void) {
    int result = 0;
    const char *dna = "ACGT";
    bitseq_t *bs = bs_pack(dna);
    result = (bs->bitseq[0] == 30) ? 1 : 0;
    bs_print(bs);
    bs_destroy(bs);
    return result;
}


int test_bs_bitset(void) {
    int result = 0;


    return result;
}
