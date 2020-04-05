#ifndef BITSET_H
#define BITSET_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct _bitset {
    uint64_t *words;
    size_t size_words;
    size_t size_bits;
} bitset_t;

bitset_t *bs_init(size_t size_bits);
void bs_destroy(bitset_t *bs);
void bs_set(bitset_t *bs, size_t idx_bit);
int bs_get(bitset_t *bs, size_t idx_bit);
void bs_clear(bitset_t *bs, size_t idx_bit);
void bs_print(bitset_t *bs);

#endif // BITSET_H
