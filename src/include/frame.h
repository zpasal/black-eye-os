#ifndef __FRAME_H
#define __FRAME_H

#include <stdint.h>
#include <bitset.h>

#define PAGE_SIZE 4096

#define EMPTY_FRAME 0xFFFFFFFFFFFFFFFF

extern bitset_t frame_bitset;

void init_kernel_frames();
void set_frame(uint64_t frame_addr);
void clear_frame(uint64_t frame_addr);
uint32_t test_frame(uint64_t frame_addr);
uint64_t find_first_frame();
void map_used_frames();

#endif