#include <frame.h>
#include <kernel.h>
#include <kheap.h>

bitset_t frame_bitset;

void init_kernel_frames() {
  kputs("MMU FRAME: init ...");
  uint64_t memory_size = 128 * 1024 * 1024; // 128Megs for now - TODO : autodetect

  uint64_t nframes = memory_size / PAGE_SIZE;
  bitset_init(&frame_bitset, 
    (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes)), 
    memory_size / PAGE_SIZE);
  kprintf("MMU FRAME: Table size %iKB\n", INDEX_FROM_BIT(frame_bitset.length) / 1024);
  kprintf("MMU FRAME: Start 0x%X\n", frame_bitset.start);
}

void set_frame(uint64_t frame_addr) {
  uint64_t frame = frame_addr / PAGE_SIZE;
  bitset_set_bit(&frame_bitset, frame);
}

void clear_frame(uint64_t frame_addr) {
  uint64_t frame = frame_addr / PAGE_SIZE;
  bitset_clear_bit(&frame_bitset, frame);
}

uint32_t test_frame(uint64_t frame_addr) {
  uint64_t frame = frame_addr / PAGE_SIZE;
  return bitset_get_bit(&frame_bitset, frame);
}

uint64_t find_first_frame() {
  uint64_t i, j;
  for (i = 0; i < INDEX_FROM_BIT(frame_bitset.length); i++) {
    // nothing free, exit early.
    if (frame_bitset.start[i] != 0xFFFFFFFF) {
      // at least one bit is free here.
      for (j = 0; j < 32; j++) {
        uint32_t toTest = 0x1 << j;
        if (!(frame_bitset.start[i] & toTest)) {
          return i*4*8+j;
        }
      }
    }
  }
  return EMPTY_FRAME;
}