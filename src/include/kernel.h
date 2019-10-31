#ifndef __KERNEL_H
#define __KERNEL_H

#include <stdint.h>


#define KERNEL_START_MEMORY 0xFFFF800000000000
#define KERNEL_VMA ((char*)KERNEL_START_MEMORY)
#define KERNEL_VIDEO_MEMORY ((unsigned char*)0xFFFFC00000000000)

void kputs(char *string);
void kprintf(const char *fmt, ...);
void kprintf_xy(int x, int y, const char *fmt, ...);
void PANIC(const char *fmt, ...);
void _kdebug(const char *fmt, ...);

#define DEBUG _kdebug

#endif
