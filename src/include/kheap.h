#ifndef __KHEAP_H
#define __KHEAP_H

#include <stdbool.h>

extern void* kernel_end;

extern uint64_t __kheap_start;
extern uint64_t __kheap_placement_address;

void init_kmalloc();
void* kmalloc_aligned(uint64_t sz);
void* kmalloc_p(uint64_t sz, uint64_t *phys);
void* kmalloc_ap(uint64_t sz, uint64_t *phys);
void* kmalloc(uint64_t sz);
uint64_t _kmalloc(uint64_t sz, bool align, uint64_t *phys);

#endif