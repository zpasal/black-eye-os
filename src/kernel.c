#include <multiboot.h>
#include <console.h>
#include <stdint.h>
#include <string.h>
#include <kernel.h>
#include <memory.h>
#include <elf.h>
#include <x86.h>

#define __UNUSED__ __attribute__((unused))

void interrupt_handler(int no, uint64_t cr2)  {
  	printf("Interrupt (0x%X) @ (0x%X)\nKernel execution paused.", no, cr2);
  	x86_hlt();
}

void init_console() {
	__krnl_console.buffer = (char*)0xFFFF8000000B8000;
	console_init(&__krnl_console);
}

void kmain() {
	init_console();

	printf("BlackEye OS main()\n");

}
