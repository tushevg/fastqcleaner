#include "bitset.h"

static const size_t BITS_PER_WORD = sizeof(uint64_t) * __CHAR_BIT__;
//static const uint32_t BITS_PER_WORD = __CHAR_BIT__;

static inline size_t bs_bitstowords(size_t bits)
{
    return (bits + (BITS_PER_WORD - 1)) >> 6;
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
    if (idx_bit >= bs->size_bits)
        return;
    size_t idx_word = idx_bit >> 6;
    bs->words[idx_word] |= ((uint64_t)1) << (idx_bit % BITS_PER_WORD);
}


int bs_get(bitset_t *bs, size_t idx_bit)
{
    if (idx_bit >= bs->size_bits)
        return 0;
    size_t idx_word = idx_bit >> 6;
    return (bs->words[idx_word] & (((uint64_t)1) << (idx_bit % BITS_PER_WORD))) ? 1 : 0;
}


void bs_clear(bitset_t *bs, size_t idx_bit)
{
    size_t idx_word = idx_bit / BITS_PER_WORD;
    if (idx_word >= bs->size_words)
        return;
    bs->words[idx_word] &= ~(1 << (idx_bit % BITS_PER_WORD));
}


void bs_print(bitset_t *bs)
{
    for (size_t idx_word = 0; idx_word < bs->size_words; ++idx_word)
        printf("%ld\t%llu\n", idx_word, bs->words[idx_word]);
}




