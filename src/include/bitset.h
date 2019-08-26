#ifndef __BITSET_H
#define __BITSET_H

#include <stdint.h>
#include <string.h>

#define DIV_ROUND_UP(a,b) ((((a) - 1) / (b)) + 1)

// Macros used in implementation
#define INDEX_FROM_BIT(a) ((a) / 32)
#define OFFSET_FROM_BIT(a) ((a) % 32)

struct bitset {
  uint64_t length; // Number of entries in bitset
  uint32_t *start; // Start address of bitset
};
typedef struct bitset bitset_t;

/**
* Create a bitset
* size must be cleanly divisible by sizeof(uint32_t)
*/
static inline void bitset_init(bitset_t *bitset, void *start, uint64_t length) {
	bitset->start = start;
	bitset->length = length;
	memset(start, 0, DIV_ROUND_UP(length, 32)*sizeof(uint32_t));
}

/**
* Set a bit in a bitset
* @param bitset pointer to bitset to act on
* @param n      bit to set
*/
static inline void bitset_set_bit(bitset_t *bitset, uint64_t n) {
	bitset->start[INDEX_FROM_BIT(n)] |= (1 << OFFSET_FROM_BIT(n));
}

/**
* Get the value of a bit in a bitset
* @param  bitset pointer to bitset to act on
* @param  n      bit to read
* @return        value of requested bit
*/
static inline uint32_t bitset_get_bit(bitset_t *bitset, uint64_t n) {
	return bitset->start[INDEX_FROM_BIT(n)] & (1 << OFFSET_FROM_BIT(n));
}

/**
* Clear a bit in a bitset
* @param bitset pointer to bitset to act on
* @param n      bit to clear
*/
static inline void bitset_clear_bit(bitset_t *bitset, uint64_t n) {
	bitset->start[INDEX_FROM_BIT(n)] &= ~(1 << OFFSET_FROM_BIT(n));
}

#endif