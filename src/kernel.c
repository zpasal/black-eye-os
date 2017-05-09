#include <multiboot.h>
#include <console.h>
#include <stdint.h>

void krnl_prepare_console() {
	__krnl_console.buffer = (char*)0xFFFF8000000B8000;
	console_init(&__krnl_console);
	puts("BlackEye OS main()\n");
}


void kmain(const multiboot_info_t* mbinfo) {
	krnl_prepare_console();
	printf("Kernel loaded  @ %p\n", KERNEL_VMA);
	printf("Multiboot Info @ %p\n", mbinfo);
	puts("Kernel initialized.");
}
