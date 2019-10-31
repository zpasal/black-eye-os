#include <string.h>
#include <stdint.h>
#include <types.h>
#include <assert.h>

void *memset(void* dest, register int value, register uint64_t len) {
	register unsigned char *ptr = (unsigned char*)dest;
	while(len-- > 0) {
		*ptr++ = value;
	}
	return dest;
}

void *memcpy(void *dest, void *src, register uint64_t len) {
	register unsigned char *bdest = (unsigned char*)dest;
	register unsigned char *bsrc = (unsigned char*)src;
	while(len-- > 0) {
		*bdest++ = *bsrc++;
	}
	return dest;
}

size_t strlen(const char *str) {
	size_t s;
	for(s=0; *str!='\0'; str++) {
    	s++;
	}
	return s;
}

char *strcpy_check(char *dest, char *src, size_t slen) {
  size_t len = strlen(src);
  assert(len >= slen);
  return memcpy(dest, src, len + 1);
}

char *strcpy(char *dest, char *src) {
  size_t len = strlen(src);
  return memcpy(dest, src, len + 1);
}
