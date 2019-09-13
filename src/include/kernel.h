#ifndef __KERNEL_H
#define __KERNEL_H

#include <stdint.h>

void kputs(char *string);
void kprintf(const char *fmt, ...);
void kprintf_xy(int x, int y, const char *fmt, ...);
void PANIC(const char *fmt, ...);

#endif
