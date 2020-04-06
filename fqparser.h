/*
 *
 * fqparser.h
 * Author: Georgi Tushev
 * Scientific Computing Facility
 * Max-Planck Institute for Brain Research
 * Frankfurt am Main, Germany
 * Bug reports: sciclist@brain.mpg.de
 *
 */

#ifndef FQPARSER_H
#define FQPARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "kroundup.h"
#include "nthash.h"

// buffer of decompressed characters
#define BUFFER_SIZE 4096

// string type that keeps its size and length
typedef struct _fqstring {
    size_t l, m;
    char *s;
} fqstring_t;

// stream type takes care of decompression buffer
typedef struct _fqstream {
    gzFile fp;
    unsigned char *uncompressed_block;
    size_t block_offset, block_length;
} fqstream_t;

// fastq record type
typedef struct _fqrecord {
    fqstring_t *name;
    fqstring_t *seq;
    fqstring_t *opt;
    fqstring_t *qual;
} fqrecord_t;

/*!
 * \brief fqstr_init, allocates fqstring type
 * \return pointer to fqstring_t
 */
fqstring_t *fqstr_init(void);

/*!
 * \brief fqstr_destroy, deallocate fqstring type
 * \param fqstr, pointer to fqstring_t
 */
void fqstr_destroy(fqstring_t *fqstr);

/*!
 * \brief fqs_open, initialize decompression stream
 *        from gzipped file
 * \param file_fqgz, full file name
 * \return fqstream_t pointer to stream structure
 */
fqstream_t *fqs_open(const char *file_fqgz);

/*!
 * \brief fqs_close, deallocates decompression stream
 * \param fqs, pointer to stream structure
 */
void fqs_close(fqstream_t *fqs);

/*!
 * \brief fqs_getline, read line from decompression buffer
 * \param fqs, pointer to stream structure
 * \param fqstr, pointer to custom string
 * \param delim, charecter to stop
 * \param append, 1 append new substring, 0 overwrite new substring
 * \return >= 0 (normal) number of characters copied
 *         -1 eof is reached
 *         -2 error during decompression
 */
int fqs_getline(fqstream_t *fqs, fqstring_t *fqstr, int delim, int append);

/*!
 * \brief fqr_init, allocate fastq record
 * \return pointer to fastq record
 */
fqrecord_t *fqr_init(void);

/*!
 * \brief fqr_destroy, deallocate fastq record
 * \param fqr, pointer to fastq record
 */
void fqr_destroy(fqrecord_t *fqr);

/*!
 * \brief fqr_read_se, read single-end file
 * \param fqr, pointer to fastq record
 * \param fqs, pointer to decompression stream
 * \return >= 0 (normal) number of characters copied
 *         -1 eof is reached
 *         -2 error during decompression
 */
int fqr_read_se(fqrecord_t *fqr, fqstream_t *fqs);

/*!
 * \brief fqr_read_pe, read paired-end file
 * \param fqr, pointer to fastq record
 * \param fqs_1, pointer to left pair decompression stream
 * \param fqs_2, pointer to right pair decompression stream
 * \return >= 0 (normal) number of characters copied
 *         -1 eof is reached
 *         -2 error during decompression
 */
int fqr_read_pe(fqrecord_t *fqr, fqstream_t *fqs_1,  fqstream_t *fqs_2);

/*!
 * \brief fqr_print, print FASTQ record
 * \param fqr, pointer to fastq record
 */
void fqr_print(fqrecord_t *fqr);

/*!
 * \brief fqr_write, writes fqrecord to gzip stream
 * \param fqr, pointer to fastq record
 * \param fo, pointer to gzip write filte
 */
void fqr_write(fqrecord_t *fqr, gzFile fo);

/*!
 * \brief fqr_qscore, calculate average base score
 * \param qual, custom string containing base quality
 * \return average quality score
 */
float fqr_qscore(fqstring_t *qual);

/*!
 * \brief fqr_hash, calculates ntHash per sequence
 * \param seq, custom string containing sequence
 * \return hash value
 */
uint64_t fqr_hash(fqstring_t *seq);

#endif // FQPARSER_H
