#ifndef TESTS_H
#define TESTS_H

#include <stdio.h>
#include <zlib.h>
#include "kseq.h"
#include "bitseq.h"
#include "fqparser.h"

int run_tests(void);

// bitseq
int test_bs_packseq(void);
int test_bs_bitset(void);
int test_kseq_writer(void);
int test_kseq_qscore(void);

#endif // TESTS_H
