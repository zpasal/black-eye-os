#include <keyboard.h>
#include <isr.h>
#include <x86.h>
#include <pic.h>
#include <kernel.h>

static void keyboard_callback(isr_ctx_t *ctx __attribute__((unused))) {
  uint8_t scancode = inp(KEYBOARD_DATA_PORT);
  kprintf_xy(20, 0, "P:%d", scancode);

  pic_acknowledge(PIC_IRQ1);
}

void init_kernel_keyboard() {	
  register_interrupt_handler(ISR_IRQ1, keyboard_callback);
	irq_enable(PIC_IRQ1);
}
