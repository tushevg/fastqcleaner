#ifndef BITSEQ_H
#define BITSEQ_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// 2bit representation of DNA letters
// A:00 C:01 G:10 T:11
// U:11 for same representation by DNA / RNA
// N:00 masked as A
// complementary sequence is achived by '~' operator

static const uint8_t A_BASE_VAL = 0;
static const uint8_t C_BASE_VAL = 1;
static const uint8_t G_BASE_VAL = 2;
static const uint8_t T_BASE_VAL = 3;
static const uint8_t N_BASE_VAL = 0;
static const uint8_t BLOCK_MASK = 8;
static const uint8_t BLOCK_SIZE = 2;

typedef struct _bitseq {
    size_t size_unpacked;
    size_t size_packed;
    uint8_t *bitseq;
} bitseq_t;

bitseq_t *bs_init(size_t);
void bs_destroy(bitseq_t *);
size_t bs_sizepacked(size_t);
uint8_t bs_twobit(const char);
bitseq_t *bs_pack(const char *);

#endif // BITSEQ_H
