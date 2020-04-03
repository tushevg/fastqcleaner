#include "nthash.h"

// offset for the complement base in the random seeds table
static const uint8_t cpOff = 0x07;

// 64-bit random seeds corresponding to bases and their complements
#define seedA 0x3c8bfbb395c60474
#define seedC 0x3193c18562a02b4c
#define seedG 0x20323ed082572324
#define seedT 0x295549f54be24456
#define seedN 0x0000000000000000

static const uint64_t seedTab[256] = {
    seedN, seedT, seedN, seedG, seedA, seedA, seedN, seedC, // 0..7
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 8..15
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 16..23
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 24..31
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 32..39
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 40..47
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 48..55
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 56..63
    seedN, seedA, seedN, seedC, seedN, seedN, seedN, seedG, // 64..71
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 72..79
    seedN, seedN, seedN, seedN, seedT, seedT, seedN, seedN, // 80..87
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 88..95
    seedN, seedA, seedN, seedC, seedN, seedN, seedN, seedG, // 96..103
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 104..111
    seedN, seedN, seedN, seedN, seedT, seedT, seedN, seedN, // 112..119
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 120..127
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 128..135
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 136..143
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 144..151
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 152..159
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 160..167
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 168..175
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 176..183
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 184..191
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 192..199
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 200..207
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 208..215
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 216..223
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 224..231
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 232..239
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN, // 240..247
    seedN, seedN, seedN, seedN, seedN, seedN, seedN, seedN  // 248..255
};

// rotate "v" to the left 1 position
static inline uint64_t rol1(const uint64_t v) {
    return (v << 1) | (v >> 63);
}


// swap bit 0 with bit 33 in "v"
static inline uint64_t swapbits033(const uint64_t v) {
    uint64_t x = (v ^ (v >> 33)) & 1;
    return v ^ (x | (x << 33));
}


// forward-strand hash value of the base kmer, i.e. fhval(kmer_0)
static inline uint64_t NTF64_Base(const char * kmerSeq, const unsigned k) {
    uint64_t hVal=0;
    for(unsigned i=0; i<k; i++) {
        hVal = rol1(hVal);
        hVal = swapbits033(hVal);
        hVal ^= seedTab[(unsigned char)kmerSeq[i]];
    }
    return hVal;
}

// reverse-strand hash value of the base kmer, i.e. rhval(kmer_0)
static inline uint64_t NTR64_Base(const char * kmerSeq, const unsigned k) {
    uint64_t hVal=0;
    for(unsigned i=0; i<k; i++) {
        hVal = rol1(hVal);
        hVal = swapbits033(hVal);
        hVal ^= seedTab[(unsigned char)kmerSeq[k-1-i]&cpOff];
    }
    return hVal;
}


// canonical ntBase
uint64_t NTC64_Base(const char * kmerSeq, const unsigned k) {
    uint64_t fhVal=0, rhVal=0;
    fhVal=NTF64_Base(kmerSeq, k);
    rhVal=NTR64_Base(kmerSeq, k);
    return (rhVal<fhVal)? rhVal : fhVal;
}
