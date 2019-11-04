#include <multiboot.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <console.h>
#include <kernel.h>
#include <memory.h>
#include <x86.h>
#include <stdarg.h>
#include <_printf.h>
#include <isr.h>
#include <timer.h>
#include <keyboard.h>
#include <timer.h>
#include <process.h>
#include <pic.h>
#include <pagging.h>
#include <vesa.h>
#include <mouse.h>
#include <serial.h>
#include <process.h>
#include <gfx.h>
#include <windows.h>

#define __UNUSED__ __attribute__((unused))

console_t __krnl_console;

// For now include app applications as binary arrays ... see src/app/compile.sh
#include <../apps/dummy_array.c>


void init_win_manager() {
  component_t w;
  w.x = 100;
  w.y = 100;
  w.height = 400;
  w.width = 600;
  strcpy(w.title, "Window [Kernel log console]");
  window_draw(&w);

  gfx_puts(110, 180, WHITE, GRAY, "Black-Eye-OS v0.1");
  gfx_puts(110, 140, WHITE, GRAY, "Init kernel console    ... DONE");
  gfx_puts(110, 150, WHITE, GRAY, "Init driver [serial]   ... DONE");
  gfx_puts(110, 160, WHITE, GRAY, "Init driver [vesa]     ... DONE");
  gfx_puts(110, 170, WHITE, GRAY, "Init driver [mmu]      ... DONE");
  gfx_puts(110, 180, WHITE, GRAY, "Init driver [pic]      ... DONE");
  gfx_puts(110, 190, WHITE, GRAY, "Init driver [isr]      ... DONE");
  gfx_puts(110, 200, WHITE, GRAY, "Init driver [timer]    ... DONE");
  gfx_puts(110, 210, WHITE, GRAY, "Init driver [keybaord] ... DONE");
  gfx_puts(110, 220, WHITE, GRAY, "Init kernel [mouse]    ... DONE");
}

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

void kmain(unsigned long magic __UNUSED__, multiboot_info_t *mbi_phys __UNUSED__) {
  init_kernel_console();
  init_kernel_serial();
  // init_kernel_vesa(TO_VMA_PTR(multiboot_info_t *, mbi_phys));
  init_kernel_pagging();
  init_kernel_pic();
  init_kernel_isr();
  init_kernel_timer();
  // init_kernel_keyboard();
  // init_kernel_mouse();

  // init_win_manager();

  current_task_index = 0;
  create_process_user(&tasks[0], dummy_app, dummy_app_len);
  create_process_user(&tasks[1], dummy_app, dummy_app_len);
  // create_process_user(&tasks[2], dummy_app, dummy_app_len);

  // Jump into task switcher and start first task - after this - kernel main will not continue
  do_first_task_jump();

  while(1) { 
  }
}
