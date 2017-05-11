#include <multiboot.h>
#include <console.h>
#include <stdint.h>
#include <string.h>
#include <interrupts.h>
#include <kernel.h>
#include <memory.h>
#include <elf.h>

#define __UN__ __attribute__((unused))


typedef struct {
	uint64_t entries[512];
} page_table_t;


extern page_table_t* pml4e_table;


void interrupt_handler(int no, uint64_t cr2) {
  	printf("Interrupt (0x%X) @ (0x%X)\nKernel execution paused.", no, cr2);
  	while(1) {};
}

void init_console() {
	__krnl_console.buffer = (char*)0xFFFF8000000B8000;
	console_init(&__krnl_console);
}

void kmain(__UN__ uint64_t rbx, __UN__ uint64_t rax) {
	init_console();
	printf("BlackEye OS main()\n");

	init_idt();
}
