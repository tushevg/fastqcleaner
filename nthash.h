/*
 * nthash.hpp
 * Author: Hamid Mohamadi
 * Genome Sciences Centre,
 * British Columbia Cancer Agency
 *
 * adapted from C++
 */

#ifndef NT_HASH_H
#define NT_HASH_H

#include <stdint.h>

// canonical ntBase
uint64_t NTC64_Base(const char * kmerSeq, const unsigned k);

#endif // NT_HASH_H
