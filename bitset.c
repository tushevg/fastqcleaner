#include "bitset.h"

static const int BITS_PER_WORD = sizeof(uint64_t) * __CHAR_BIT__;

static inline size_t bs_bitstowords(size_t bits)
{
    return (bits + 1) / BITS_PER_WORD;
}


bitset_t *bs_init(size_t size_bits)
{
    bitset_t *bs = (bitset_t *)malloc(sizeof(bitset_t));
    if (!bs) return 0;
    bs->size_bits = size_bits;
    bs->size_words = bs_bitstowords(size_bits);
    bs->words = (uint64_t *)calloc(bs->size_words, sizeof(uint64_t));
    if (!bs->words) {
        bs_destroy(bs);
        return 0;
    }
    return bs;
}


void bs_destroy(bitset_t *bs)
{
    if (!bs) return;
    free(bs->words);
    free(bs);
}


void bs_set(bitset_t *bs, size_t idx_bit)
{
    bs->words[idx_bit / BITS_PER_WORD] |= (1 << (idx_bit % BITS_PER_WORD));
}


int bs_exist(bitset_t *bs, size_t idx_bit)
{
    return (bs->words[idx_bit / BITS_PER_WORD] & (1 << (idx_bit % BITS_PER_WORD))) ? 1 : 0;
}


void bs_clear(bitset_t *bs, size_t idx_bit)
{
    bs->words[idx_bit / BITS_PER_WORD] &= ~(1 << (idx_bit % BITS_PER_WORD));
}




