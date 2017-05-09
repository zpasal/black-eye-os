#include <console.h>
#include <string.h>
#include <stdarg.h>
#include <_printf.h>

int __console_id = 0;
t_console __krnl_console;

t_console* console_init(t_console *console) {
	console_reset(console);
	return console;
}

t_console* console_reset(t_console *console) {
	console->id = __console_id++;
	console->color = CONSOLE_BG_FG_COLOR(LIGHTGRAY, BLACK);
	console->current_index = 0;
	memset(console->buffer, 0, CONSOLE_80_25_SIZE);
	return console;
}

void console_putch(t_console *console, char ch) {
	char *buffer = console->buffer + console->current_index*2;

	switch(ch) {
		case '\n':  // backspace
			console->current_index = ((console->current_index + 80) / 80) * 80;
			break;
		default:
			*buffer++ = ch;
			*buffer++ = console->color;
			console->current_index++;
			if (console->current_index >= CONSOLE_80_25_SIZE) {
				console->current_index = 0;
			}
			break;
	}
}

void console_puts(t_console *console, char *string) {
	while(*string) {
		console_putch(console, *string++);
	}
	console_putch(console, '\n');
}


int cprintf_help(unsigned c, void *ptr) {
	t_console *console = (t_console*)ptr;
	console_putch(console, c);
	return 0;
}

void console_printf(t_console *console, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	(void)_printf(fmt, args, cprintf_help, console);
	va_end(args);
}


// MOVE THIS TO KERNEL.C/H
void puts(char *string) {
	console_puts(&__krnl_console, string);
}
 
void printf(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	(void)_printf(fmt, args, cprintf_help, &__krnl_console);
	va_end(args);
}