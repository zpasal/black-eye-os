#ifndef __STRING_H
#define __STRING_H

#include <stdint.h>
#include <types.h>

void *memset(void* dest, register int value, register uint64_t len);
void *memcpy(void *dest, void *src, register uint64_t len);

size_t strlen(const char *str);

char *strcpy_check(char *dest, char *src, size_t slen);
char *strcpy(char *dest, char *src);

#endif