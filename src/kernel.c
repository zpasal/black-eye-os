#include <multiboot.h>
#include <console.h>
#include <stdint.h>
#include <string.h>
#include <kernel.h>
#include <memory.h>
#include <elf.h>
#include <x86.h>
#include <stdarg.h>
#include <bitset.h>
#include <_printf.h>
#include <kheap.h>
#include <isr.h>
#include <timer.h>
#include <keyboard.h>
#include <frame.h>
#include <timer.h>

#define __UNUSED__ __attribute__((unused))

console_t __krnl_console;

int a = 10;

void kputs(char *string) {
  console_puts(&__krnl_console, string);
}

void kprintf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, cprintf_help, &__krnl_console);
  va_end(args);
}

void PANIC(const char *fmt, ...) {
  __krnl_console.color = LIGHTRED;
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, cprintf_help, &__krnl_console);
  va_end(args);
  x86_hlt(); 
}

void init_kernel_console() {
  __krnl_console.buffer = (char*)0xFFFF8000000B8000;
  console_init(&__krnl_console);
}

void init_kernel_malloc() {
  kputs("KHEAP: init ...");
  init_kmalloc();
  kprintf("KHEAP: Kernel heap starts at: 0x%X\n", __kheap_start);
}


extern uint64_t __tick ;

const char *__krnl_drive_type[] = { 
  "no floppy drive", 
  "360kb 5.25in floppy drive", 
  "1.2mb 5.25in floppy drive", 
  "720kb 3.5in", 
  "1.44mb 3.5in", 
  "2.88mb 3.5in"
};


typedef struct {
  uint64_t rsp;
  int id;
  uint8_t attrib;
} task_t;



task_t tasks[2];
#define TASK_STACK_SIZE 512
uint8_t stack1[TASK_STACK_SIZE];
uint8_t stack2[TASK_STACK_SIZE];
task_t* current_task;

void task1() {
  x86_cli();
  *(CONSOLE_VIDEO_MEMORY+1) += 1;
  x86_sti();
}

void task2() {
  x86_cli();
  *(CONSOLE_VIDEO_MEMORY+3) += 1;
  x86_sti();
}

// void setup_task(task_t* task, void* stack_end) {
//   uint64_t* rsp = (uint64_t*)stack_end;
//   rsp--;

// }


void kmain(/*unsigned long magic, unsigned long addr*/) {
  init_kernel_console();
  kprintf("\nBlackEye OS\n");

  init_kernel_isr();
  init_kernel_malloc();
  init_kernel_frames();
  init_kernel_keyboard();
  init_kernel_timer();

  current_task = tasks;
  


  timer_enable();

  while(1) {
    // x86_cli();

    // __krnl_console.current_index = 0;
    // kprintf("%i", __tick % 10000);
    // x86_sti();
  }

  // if (multiboot_is_valid(magic, addr) == false) {
  //   kprintf("invalid multiboot\n");
  //   while(1) {}
  // }
  // multiboot_info_t *mbi = TO_VMA_PTR(multiboot_info_t*, addr);
  // reserved_areas_t reserved = read_multiboot_info(mbi);
  // kprintf("- multiboot_start = 0x%X, multiboot_end = 0x%X\n", reserved.multiboot_start, reserved.multiboot_end);
  // kprintf("- kernel_start    = 0x%X, kernel_end    = 0x%X\n", reserved.kernel_start, reserved.kernel_end);
}
