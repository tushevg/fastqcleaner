/*
 * nthash.h
 * Author: Hamid Mohamadi
 * Genome Sciences Centre,
 * British Columbia Cancer Agency
 * https://github.com/bcgsc/ntHash
 *
 * adapted from C++
 * Author: Georgi Tushev
 * Scientific Computing Facility
 * Max-Planck Institute for Brain Research
 * Frankfurt am Main, Germany
 * Bug reports: sciclist@brain.mpg.de
 *
 */

#ifndef NT_HASH_H
#define NT_HASH_H

#include <stdint.h>

/*!
 * \brief NTC64_Base, calculates nucleotides hash value
 * \param kmerSeq, base sequence
 * \param k, size of sequence
 * \return strand independent hash value
 */
uint64_t NTC64_Base(const char * kmerSeq, const unsigned k);

#endif // NT_HASH_H
