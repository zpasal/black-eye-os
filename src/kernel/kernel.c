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
#include <timer.h>
#include <process.h>
#include <pci.h>
#include <pic.h>
#include <serial.h>
#include <pagging.h>
#include <vesa.h>
#include <mouse.h>

#define __UNUSED__ __attribute__((unused))

#define GDT_NULL        0x00
#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10
#define GDT_USER_CODE   0x18
#define GDT_USER_DATA   0x20
#define GDT_TSS         0x28 

console_t __krnl_console;

int a = 10;

int serial_printf_help(unsigned c, void *ptr __UNUSED__) {
  serial_write_com(1, c);
  return 0;
}

void kputs(char *string) {
  console_puts(&__krnl_console, string);
}

void kprintf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, cprintf_help, &__krnl_console);
  va_end(args);
}

void kprintf_xy(int x, int y, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int old_index = __krnl_console.current_index;
  __krnl_console.current_index = x*2 + y*25;
  (void)_printf(fmt, args, cprintf_help, &__krnl_console);
  __krnl_console.current_index = old_index;
  va_end(args);
}

void PANIC(const char *fmt, ...) {
  __krnl_console.color = LIGHTRED;
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, serial_printf_help, NULL);
  va_end(args);
  x86_hlt(); 
}

void _kdebug(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  (void)_printf(fmt, args, serial_printf_help, NULL);
  va_end(args);
}

void init_kernel_console() {
  __krnl_console.buffer = (char*)0xFFFF8000000B8000;
  console_init(&__krnl_console);
}

void init_kernel_malloc() {
  DEBUG("KHEAP: init ...\n");
  init_kmalloc();
  DEBUG("KHEAP: Kernel heap starts at: 0x%X\n", __kheap_start);
}

task_t tasks[3];
#define TASK_STACK_SIZE 512
int current_task_index;

#include <../apps/dummy_array.c>

void setup_task(task_t* task, void* app_data, uint32_t app_size) {

  // setup task MMU
  uint8_t* task_memory = kmalloc_aligned(PAGE_SIZE);
  memcpy(task_memory, app_data, app_size);
  uint64_t* rsp = (uint64_t*)task_memory;
  rsp--;
  /*
  +------------+
  +40 | %SS        |
  +32 | %RSP       |
  +24 | %RFLAGS    |
  +16 | %CS        |
  +8 | %RIP       | <-- %RSP
  +------------+
  * */
  *rsp-- = GDT_USER_DATA + 3;	             // SS
  *rsp-- = 0x200000 - 8; 		   // RSP  2MB - 8
  *rsp-- = 0x286; 		         // RFLAGS
  *rsp-- = GDT_USER_CODE + 3;		           // CS
  *rsp-- = 0x0000000000000000; // entry point is always 0
  *rsp-- = 1;
  *rsp-- = 2;
  *rsp-- = 3;
  *rsp-- = 4;
  *rsp-- = 5;
  *rsp-- = 6;
  *rsp-- = 7;
  *rsp-- = 8;
  *rsp-- = 9;
  *rsp-- = 10;
  *rsp-- = 11;
  *rsp-- = 12;
  *rsp-- = 13;
  *rsp-- = 14;
  *rsp-- = 15;
  // rsp -= 14; // rax,rbx,rcx,rdx,rsi,rdi,rbp,r8,r9,r10,r11,r12,r13,r14,r15 - switch.asm irq0

  task->rsp = (uint64_t)(rsp+1); //- switch will do it

  task->pde.all = TO_PHYS_U64(task_memory) | 0x87; // Present + Write + CPL3
 }

task_t* next_task() {
	current_task_index++;
	current_task_index = current_task_index % 3;

	return &tasks[current_task_index];
}

task_t* current_task() {
	return &tasks[current_task_index];
}

void __switch_to(task_t* next __UNUSED__) {
  pde_user[0] = next->pde;
  x86_tlb_flush_all();
}

void kmain(unsigned long magic __UNUSED__, multiboot_info_t *mbi_phys) {

  //BgaSetVideoMode(1024, 768, 24, 1, 1);

  init_kernel_console();
  init_kernel_serial();

  kprintf("\nBlackEye OS\n");
  current_task_index = 0;

  init_kernel_vesa(TO_VMA_PTR(multiboot_info_t *, mbi_phys));
  init_kernel_malloc();
  init_kernel_pagging();
  init_kernel_pic();
  init_kernel_isr();
  init_kernel_timer();
  init_kernel_keyboard();
  init_kernel_mouse();

  setup_task(&tasks[0], dummy_app, dummy_app_len);
  setup_task(&tasks[1], dummy_app, dummy_app_len);
  setup_task(&tasks[2], dummy_app, dummy_app_len);

  memset(KERNEL_VIDEO_MEMORY, 0xFFFFFF, 1024 * 4);

  // Jump into task switcher and start first task - after this - kernel main will not continue
  do_first_task_jump();

  while(1) { }
}
