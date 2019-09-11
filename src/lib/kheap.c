#include <stdint.h>
#include <stdbool.h>
#include <memory.h>
#include <kheap.h>

uint64_t __kheap_start;
uint64_t __kheap_placement_address;

void init_kmalloc() {
  __kheap_start = (uint64_t)&kernel_end;
  __kheap_placement_address = __kheap_start;
}

uint64_t _kmalloc(uint64_t sz, bool align, uint64_t *phys) {
  // If the address is not already page-aligned
  if (align && (__kheap_placement_address & 0xFFFFFFFFFFE00000)) {
    // Align it.
    __kheap_placement_address &= 0xFFFFFFFFFFE00000;
    __kheap_placement_address += 0x200000;
  }
  if (phys) {
    *phys = TO_PHYS_U64(__kheap_placement_address);
  }
  uint64_t tmp = __kheap_placement_address;
  __kheap_placement_address += sz;
  return tmp;
}

void* kmalloc_aligned(uint64_t sz) {
  return (void*)_kmalloc(sz, true, NULL);
}

void*  kmalloc_p(uint64_t sz, uint64_t *phys) {
  return (void*)_kmalloc(sz, false, phys);
}

void*  kmalloc_ap(uint64_t sz, uint64_t *phys) {
  return (void*)_kmalloc(sz, true, phys);
}

void*  kmalloc(uint64_t sz) {
  return (void*)_kmalloc(sz, false, NULL);
}