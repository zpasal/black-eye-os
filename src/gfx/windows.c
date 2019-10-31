#include <string.h>
#include <assert.h>
#include <types.h>
#include <gfx.h>
#include <windows.h>

void window_draw(component_t *win) {
	int frame_height = 8 + 2*3;

	int x1 = win->x;
	int y1 = win->y;
	int x2 = win->x + win->width;
	int y2 = win->y + win->height;
	DEBUG("Draw W: (%i,%i)-(%i,%i)\n\r", x1, y1, x2, y2);

	// Main background
	gfx_fillrect(x1, y1, x2, y2, GRAY);

	// Frame tob
	gfx_fillrect(x1 + 1, y1 + 1, x2 - 1, y1 + frame_height, BLUE);

	// Frame label
	gfx_puts(x1 + 1 + 3, y1 + 1 + 3, WHITE, BLUE, win->title);
}