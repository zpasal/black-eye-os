#ifndef __CONSOLE_H
#define __CONSOLE_H

#define CONSOLE_80_25_SIZE (80*25*2)
#define CONSOLE_BG_FG_COLOR(fg_color, bg_color) ((bg_color << 4) | (fg_color & 0x0F))
#define CONSOLE_VIDEO_MEMORY ((char*)0xB8000)

enum COLORS {
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHTGRAY,
    DARKGRAY,       
    LIGHTBLUE,
    LIGHTGREEN,
    LIGHTCYAN,
    LIGHTRED,
    LIGHTMAGENTA,
    YELLOW,
    WHITE
};

typedef struct {
	int id;
	char color;
	char *buffer;
	int  current_index;
} t_console; 

extern int __CONSOLE_ID;
extern t_console __krnl_console;

t_console* console_init(t_console *console);
t_console* console_reset(t_console *console);
void console_putch(t_console *console, char ch);
void console_puts(t_console *console, char *string);

void puts(char *string);

#endif