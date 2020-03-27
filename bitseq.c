#include "bitseq.h"


bitseq_t *bs_init(size_t size_unpacked) {

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


void bs_destroy(bitseq_t *bs) {

    if (bs->bitseq)
        free(bs->bitseq);

    if (bs)
        free(bs);
}


size_t bs_sizepacked(size_t size_unpacked) {
    return ((size_unpacked + 3) >> 2);
}


uint8_t bs_twobit(const char base) {
    uint8_t twobit;
    switch (base) {
    case 'C':
    case 'c':
        twobit = C_BASE_VAL;
        break;
    case 'G':
    case 'g':
        twobit = G_BASE_VAL;
        break;
    case 'T':
    case 't':
        twobit = T_BASE_VAL;
        break;
    default:
        twobit = A_BASE_VAL;
    }
    return twobit;
}


bitseq_t *bs_pack(const char *seq) {
    size_t size_unpack = strlen(seq);
    bitseq_t *bs = bs_init(size_unpack);

    uint8_t mask = BLOCK_MASK;
    size_t idx_block = 0;
    for (size_t idx_base = 0; idx_base < size_unpack; idx_base++) {
        uint8_t block_val = bs_twobit(seq[idx_base]);
        mask -= BLOCK_SIZE;
        bs->bitseq[idx_block] |= (block_val << mask);

        if (mask == 0) {
            mask = BLOCK_MASK;
            idx_block++;
        }
    }

    return bs;
}
