#include "bitseq.h"

static const uint8_t BITS_PER_BYTE = __CHAR_BIT__;
static const uint8_t BITS_PER_BLOCK = sizeof(uint8_t) * __CHAR_BIT__;
static const uint8_t BITS_PER_CHAR = 2;
static const uint8_t MASK_TWOBIT = 3;
static const uint8_t MASK_MSB = 128;


bitseq_t *bs_init(size_t size_unpacked)
{
    bitseq_t *bs = (bitseq_t *)malloc(sizeof(bitseq_t));
    if (bs == NULL) return NULL;

    bs->size_unpacked = size_unpacked;
    bs->size_packed = bs_sizepacked(size_unpacked);
    bs->bitseq = (uint8_t *)calloc(bs->size_packed, sizeof(uint8_t));
    if (bs->bitseq == NULL) {
        bs_destroy(bs);
        return NULL;
    }

    return bs;
}


void bs_destroy(bitseq_t *bs)
{
    if (bs->bitseq)
        free(bs->bitseq);

    if (bs)
        free(bs);
}


size_t bs_sizepacked(size_t size_unpacked)
{
    return (size_unpacked + MASK_TWOBIT) >> BITS_PER_CHAR;
}


uint8_t bs_dnatwobit(const char base)
{
    return (base >> 1) & MASK_TWOBIT;
}


bitseq_t *bs_pack(const char *seq)
{
    size_t size_unpack = strlen(seq);
    bitseq_t *bs = bs_init(size_unpack);

    uint8_t mask = BITS_PER_BLOCK;
    size_t idx_block = 0;
    for (size_t idx_base = 0; idx_base < size_unpack; idx_base++) {
        uint8_t block_val = bs_dnatwobit(seq[idx_base]);
        mask -= BITS_PER_CHAR;
        bs->bitseq[idx_block] |= (block_val << mask);

        if (mask == 0) {
            mask = BITS_PER_BLOCK;
            idx_block++;
        }
    }

    return bs;
}


void bs_set(bitseq_t *bs, size_t index)
{
    bs->bitseq[index / BITS_PER_BLOCK] |= (1 << (index % BITS_PER_BLOCK));
}


void bs_clear(bitseq_t *bs, size_t index)
{
    bs->bitseq[index / BITS_PER_BLOCK] &= ~(1 << (index % BITS_PER_BLOCK));
}


int bs_exist(bitseq_t *bs, size_t index)
{
    return (bs->bitseq[index / BITS_PER_BLOCK] & (1 << (index % BITS_PER_BLOCK))) ? 1 : 0;
}


void bs_print(bitseq_t *bs)
{
    bitseq_iter_t iter;
    bs_iter_begin(&iter);
    while (bs_iter_next(&iter, bs) > 0) {
        (bs_iter_value(&iter) == 1) ? printf("1") : printf("0");
    }
    printf("\n");
}


void bs_iter_begin(bitseq_iter_t *iter)
{
    iter->idx_byte = 0;
    iter->idx_bit = 0;
    iter->value = 0;
}


int bs_iter_next(bitseq_iter_t *iter, bitseq_t *bs)
{
    // check current bit
    if (iter->idx_bit >= BITS_PER_BLOCK) {
        iter->idx_bit = 0;
        iter->idx_byte++; // next block
    }

    // check for last byte
    if (iter->idx_byte >= bs->size_packed)
        return 0;

    // update mask
    uint8_t mask = MASK_MSB >> iter->idx_bit;
    iter->value = (bs->bitseq[iter->idx_byte] & mask) ? 1 : 0;

    // increament bit
    iter->idx_bit++;

    return 1;
}


int bs_iter_value(bitseq_iter_t *iter)
{
    return iter->value;
}
