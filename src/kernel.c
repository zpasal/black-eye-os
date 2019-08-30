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

typedef struct {
  uint64_t rsp;
  uint32_t id;
  uint32_t attrib;
}  __attribute__((packed)) task_t;


task_t tasks[3];
#define TASK_STACK_SIZE 512
uint8_t stack0[TASK_STACK_SIZE];
uint8_t stack1[TASK_STACK_SIZE];
uint8_t stack2[TASK_STACK_SIZE];
int current_task_index;

__attribute__((naked)) void task1() {
	while(1) {
    *((char*)0xFFFF800000000000 + 0xE0000000) += 1;
  	  *(CONSOLE_VIDEO_MEMORY+5) += 1;
	}
}

__attribute__((naked)) void task2() {
	while(1) {
		*(CONSOLE_VIDEO_MEMORY+3) += 1;
	}
}
__attribute__((naked)) void task3() {
	while(1) {
		*(CONSOLE_VIDEO_MEMORY+7) += 1;
	}
}

void setup_task(task_t* task, void* stack_end, void* entry) {
   uint64_t* rsp = (uint64_t*)stack_end;
   rsp--;
   uint64_t jump_rsp = (uint64_t)rsp;
   /*
		+------------+
	+40 | %SS        |
	+32 | %RSP       |
	+24 | %RFLAGS    |
	+16 | %CS        |
	 +8 | %RIP       | <-- %RSP
		+------------+
    * */
   *rsp-- = 0x0;	// SS
   *rsp-- = jump_rsp; 		// RSP
   *rsp-- = 0x286; 		// RFLAGS
   *rsp-- = 0x08;		// CS
   *rsp-- = (uint64_t)entry;
   rsp -= 14; // rax,rbx,rcx,rdx,rsi,rdi,rbp,r8,r9,r10,r11,r12,r13,r14,r15 - switch.asm irq0

   task->attrib = 0;
   task->id = 0;
   task->rsp = (uint64_t)rsp; //- switch will do it
 }

task_t* next_task() {
	current_task_index++;
	current_task_index = current_task_index % 3;

	return &tasks[current_task_index];
}

task_t* current_task() {
	return &tasks[current_task_index];
}

#define VBE_DISPI_INDEX_ID (0)
#define VBE_DISPI_INDEX_XRES (1)
#define VBE_DISPI_INDEX_YRES (2)
#define VBE_DISPI_INDEX_BPP (3)
#define VBE_DISPI_INDEX_ENABLE (4)
#define VBE_DISPI_INDEX_BANK (5)
#define VBE_DISPI_INDEX_VIRT_WIDTH (6)
#define VBE_DISPI_INDEX_VIRT_HEIGHT (7)
#define VBE_DISPI_INDEX_X_OFFSET (8)
#define VBE_DISPI_INDEX_Y_OFFSET (9)
#define VBE_DISPI_IOPORT_INDEX (0x01CE)
#define VBE_DISPI_IOPORT_DATA (0x01CF)
#define VBE_DISPI_DISABLED (0x00) 
#define BE_DISPI_INDEX_ENABLE (4)
#define VBE_DISPI_ENABLED (0x01) 
#define VBE_DISPI_LFB_ENABLED (0x40)
#define VBE_DISPI_NOCLEARMEM (0x80)

void BgaWriteRegister(unsigned short IndexValue, unsigned short DataValue)
{
    outpw(VBE_DISPI_IOPORT_INDEX, IndexValue);
    outpw(VBE_DISPI_IOPORT_DATA, DataValue);
}
 
unsigned short BgaReadRegister(unsigned short IndexValue)
{
    outpw(VBE_DISPI_IOPORT_INDEX, IndexValue);
    return inpw(VBE_DISPI_IOPORT_DATA);
}
 
int BgaIsAvailable(void)
{
    return 1; //(BgaReadRegister(VBE_DISPI_INDEX_ID) == VBE_DISPI_ID4);
}
 
void BgaSetVideoMode(unsigned int Width, unsigned int Height, unsigned int BitDepth, int UseLinearFrameBuffer, int ClearVideoMemory)
{
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    BgaWriteRegister(VBE_DISPI_INDEX_XRES, Width);
    BgaWriteRegister(VBE_DISPI_INDEX_YRES, Height);
    BgaWriteRegister(VBE_DISPI_INDEX_BPP, BitDepth);
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED |
        (UseLinearFrameBuffer ? VBE_DISPI_LFB_ENABLED : 0) |
        (ClearVideoMemory ? 0 : VBE_DISPI_NOCLEARMEM));
}
 
void BgaSetBank(unsigned short BankNumber)
{
    BgaWriteRegister(VBE_DISPI_INDEX_BANK, BankNumber);
}

void kmain(/*unsigned long magic, unsigned long addr*/) {

  BgaSetVideoMode(1024, 768, 24, 1, 1);
    *((char*)0xFFFF800000000000 + 0xE0000000) = 200;
    *((char*)0xFFFF800000000000 + 0xE0000001) = 200;
    *((char*)0xFFFF800000000000 + 0xE0000002) = 200;

  init_kernel_console();
  kprintf("\nBlackEye OS\n");
  current_task_index = 0;

  init_kernel_isr();
  init_kernel_malloc();
  init_kernel_frames();
  init_kernel_keyboard();
  init_kernel_timer();

  setup_task(&tasks[0], stack0 + TASK_STACK_SIZE - sizeof(uint64_t), task1);
  tasks[0].id = 0;
  setup_task(&tasks[1], stack1 + TASK_STACK_SIZE - sizeof(uint64_t), task2);
  tasks[1].id = 1;
  setup_task(&tasks[2], stack2 + TASK_STACK_SIZE - sizeof(uint64_t), task3);
  tasks[2].id = 2;

  // Jump into task switcher and start first task - after this - kernel main will not continue
  do_first_task_jump();
  while(1) { }
}
