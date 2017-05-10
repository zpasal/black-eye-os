#ifndef __KERNEL_H
#define __KERNEL_H

#include <stdint.h>

#define TO_VMA(ptr) ((uint64_t)ptr + (uint64_t)0xFFFF800000000000)

#define KERNEL_VMA ((char*)0xFFFF800000000000)


void interrupt_handler();

void load_idt(void* idt);

#endif