#ifndef __KERNEL_H
#define __KERNEL_H

#include <stdint.h>

#define KERNEL_START_MEMORY 0xFFFF800000000000
#define KERNEL_VMA ((char*)KERNEL_START_MEMORY)

#define KERNEL_VIDEO_MEMORY ((unsigned char*)0xFFFFC00000000000)

#define TO_VMA_U64(ptr) ((uint64_t)ptr + (uint64_t)KERNEL_START_MEMORY)
#define TO_VMA_PTR(type, ptr) ((type)TO_VMA_U64(ptr))
#define TO_PHYS_U64(ptr) ((uint64_t)ptr ^ (uint64_t)KERNEL_START_MEMORY)
#define TO_PHYS_PTR(type, ptr) ((type)TO_PHYS_U64(ptr))

#define GDT_NULL        0x00
#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10
#define GDT_USER_CODE   0x18
#define GDT_USER_DATA   0x20
#define GDT_TSS         0x28 

void kputs(char *string);
void kprintf(const char *fmt, ...);
void kprintf_xy(int x, int y, const char *fmt, ...);
void PANIC(const char *fmt, ...);
void _kdebug(const char *fmt, ...);

#define DEBUG _kdebug

#endif
