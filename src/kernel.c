#include <multiboot.h>
#include <console.h>
#include <stdint.h>
#include <string.h>
#include <interrupts.h>

extern int _interrupt();

void interrupt_handler(int no) {
  printf("Interrupt (0x%X)\nKernel execution paused.", no);
}

void init_console() {
	__krnl_console.buffer = (char*)0xFFFF8000000B8000;
	console_init(&__krnl_console);
	puts("BlackEye OS main()\n");
}

void kmain(__attribute__((unused)) const multiboot_info_t* mbinfo) {
	init_console();
	init_idt();
	puts("Kernel initialized.");
	_interrupt();
}
