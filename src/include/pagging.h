#ifndef __PAGGING_H
#define __PAGGING_H

#include <stdint.h>
#include <bitset.h>

// 2MB Pages
#define PAGE_SIZE 0x200000

#define EMPTY_FRAME 0xFFFFFFFFFFFFFFFF

struct bitset {
  uint64_t nframes; // Number of entries in bitset
  uint32_t *frames; // Start address of bitset
};
typedef struct bitset bitset_t;

extern bitset_t frame_bitset;

void set_frame(uint64_t frame_addr);
void clear_frame(uint64_t frame_addr);
uint32_t test_frame(uint64_t frame_addr);
uint64_t find_first_frame();

void init_kernel_pagging();

#endif