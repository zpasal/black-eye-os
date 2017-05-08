#include <console.h>

void krnl_prepare_console() {
	__krnl_console.buffer = (char*)0xFFFF8000000B8000;
	console_init(&__krnl_console);
	puts("OS main()");
	puts("Kernel console initialized.");
}

int main() {
	krnl_prepare_console();
	
	puts("Kernel done.");
	return 0;
}

