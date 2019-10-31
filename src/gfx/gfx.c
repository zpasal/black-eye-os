#include <gfx.h>
#include <vesa.h>
#include <string.h>
#include "./font_8x8.h"

#define FIRST_PIXEL(x, y) ((x) + ((y) * video_info.width))

void gfx_putpixel(int x, int y, uint32_t color) {
	VESA_FRAMEBUFFER_32[FIRST_PIXEL(x, y)] = color;
}

void gfx_hline(int x1, int x2, int y, uint32_t color) {
	uint32_t *vptr = VESA_FRAMEBUFFER_32 + FIRST_PIXEL(x1, y);
	for (int i=x1; i<=x2; i++) {
		*vptr++ = color;
	}
}

void gfx_fillrect(int x1, int y1, int x2, int y2, uint32_t color) {
	for (int i=y1; i<=y2; i++) {
		gfx_hline(x1, x2, i, color);
	}
}

void gfx_putchar(int x, int y, uint32_t fgcolor, uint32_t bgcolor, const char c) {
    uint8_t i, j;
    for(i = 0; i < 8; i++) {
      for(j = 0; j < 8; j++) {
    		VESA_FRAMEBUFFER_32[FIRST_PIXEL(x+i, y+j)] = ((font8x8_basic[c & 0x7F][j] >> i ) & 1) ? fgcolor : bgcolor;
      }
    }
}

void gfx_puts(int x, int y, uint32_t fgcolor, uint32_t bgcolor, const char *c) {
	while(*c) {
		gfx_putchar(x, y, fgcolor, bgcolor, *c++);
		x += 8;
	}
}

void gfx_copy(int x, int y, int width, int height, uint32_t *src) {
	for (; y <= y + height; x++) {
		for (; x <= x + width; x++) {
			*src++ = VESA_FRAMEBUFFER_32[FIRST_PIXEL(x, y)];
		}
	}
}

void gfx_blit(int x, int y, int width, int height, uint32_t *src) {
	for (; y <= y + height; x++) {
		for (; x <= x + width; x++) {
			VESA_FRAMEBUFFER_32[FIRST_PIXEL(x, y)] = *src++;
		}
	}
}
