#ifndef __WINDOWS_H
#define __WINDOWS_H

#define MAX_COMPONENTS 512

#define COMPONENT_TYPE_WINDOW 1 

#define COMPONENT_STATE_CREATED 1

#define GRAY 	0x7F7F7F
#define BLUE 	0x0000AA
#define WHITE 0xFFFFFF

struct component_struct {
	short x, y;
	short width, height;
	char title[128];
};
typedef struct component_struct component_t;

void window_draw(component_t *win);

#endif