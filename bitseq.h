#ifndef BITSEQ_H
#define BITSEQ_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* 2bit representation of DNA letters
 * A :  65 : 1000 |00| 1
 * a :  97 : 1100 |00| 1
 * C :  67 : 1000 |01| 1
 * c :  99 : 1100 |01| 1
 * G :  71 : 1000 |11| 1
 * g : 103 : 1100 |11| 1
 * T :  84 : 1010 |10| 0
 * t : 116 : 1110 |10| 0
 * U :  85 : 1010 |10| 1
 * u : 117 : 1110 |10| 1
 * N :  78 : 1001 |11| 0
 * n : 110 : 1101 |11| 0
 * DNA/RNA conversion between T and U is compatible
 * N is masked as G
 * complementary sequence
*/

static const uint8_t TWO_BIT_MASK = 3;
static const uint8_t BITS_PER_BYTE = 8;
static const uint8_t BITS_PER_BLOCK = 2;


typedef struct _bitseq {
    size_t size_unpacked;
    size_t size_packed;
    uint8_t *bitseq;
} bitseq_t;

bitseq_t *bs_init(size_t);
void bs_destroy(bitseq_t *);
size_t bs_sizepacked(size_t);
bitseq_t *bs_pack(const char *);
uint8_t bs_dnatwobit(const char);
void bs_set(bitseq_t *, size_t, int);
int bs_exist(bitseq_t *, size_t);

#endif // BITSEQ_H
