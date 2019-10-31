#ifndef __GFX_H
#define __GFX_H

#include <stdint.h>

void gfx_putpixel(int x, int y, uint32_t color);
void gfx_hline(int x1, int x2, int y, uint32_t color);
void gfx_fillrect(int x1, int y1, int x2, int y2, uint32_t color);
void gfx_putchar(int x, int y, uint32_t fgcolor, uint32_t bgcolor, const char c);
void gfx_puts(int x, int y, uint32_t fgcolor, uint32_t bgcolor, const char *c);

#endif 