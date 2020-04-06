/*
 *
 * bitset.h
 * Author: Georgi Tushev
 * Scientific Computing Facility
 * Max-Planck Institute for Brain Research
 * Frankfurt am Main, Germany
 * Bug reports: sciclist@brain.mpg.de
 *
 */

#ifndef BITSET_H
#define BITSET_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*!
 * bitset_t data type
 */
typedef struct _bitset {
    uint64_t *words;
    size_t size_words;
    size_t size_bits;
} bitset_t;


/*!
 * \brief bs_init, allocates memory for bitset data structure
 *        all bits are initialized 0
 * \param size_bits, is the size of the containuer in bits
 * \return a pointer to bitset_t type
 */
bitset_t *bs_init(size_t size_bits);


/*!
 * \brief bs_destroy, deallocates memory from bitset data pointer
 * \param bs, a pointer to bitset_t type
 */
void bs_destroy(bitset_t *bs);


/*!
 * \brief bs_set, set a given bit to one
 * \param bs, a pointer to bitset_t type
 * \param idx_bit, index of bit to rise
 */
void bs_set(bitset_t *bs, size_t idx_bit);


/*!
 * \brief bs_get, check if bit is up
 * \param bs, a pointer to bitset_t type
 * \param idx_bit, index of bit to check
 * \return 1 bit is up
 *         0 bit is down
 */
int bs_get(bitset_t *bs, size_t idx_bit);


/*!
 * \brief bs_clear, set a given bit to zero
 * \param bs, a pointer to bitset_t type
 * \param idx_bit, index of bit to clear
 */
void bs_clear(bitset_t *bs, size_t idx_bit);


/*!
 * \brief bs_print, prints each word in bitset
 * \param bs, a pointer to bitset_t type
 */
void bs_print(bitset_t *bs);

#endif // BITSET_H
