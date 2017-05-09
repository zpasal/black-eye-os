#ifndef __STRING_H
#define __STRING_H

#include <types.h>

void *memset(void* dest, register int value, register int len);
void *memcpy(void *dest, void *src, register int len);

size_t strlen(const char *str);

#endif