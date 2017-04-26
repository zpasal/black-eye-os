#include <string.h>

void *memset(void* dest, register int value, register int len) {
	register unsigned char *ptr = (unsigned char*)dest;
	while(len-- > 0) {
		*ptr++ = value;
	}
	return dest;
}

void *memcpy(void *dest, void *src, register int len) {
	register unsigned char *bdest = (unsigned char*)dest;
	register unsigned char *bsrc = (unsigned char*)src;
	while(len-- > 0) {
		*bdest++ = *bsrc++;
	}
	return dest;
}
