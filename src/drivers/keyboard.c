#include <keyboard.h>
#include <isr.h>
#include <ports.h>
#include <x86.h>
#include <kernel.h>

#define SCANCODE_MAX 57

static void keyboard_callback(isr_ctx_t *ctx __attribute__((unused))) {
  uint8_t scancode = inp(KEYBOARD_DATA_PORT);

  if (scancode > SCANCODE_MAX) {
    return;
  }

  kprintf("Received scancode: %d\n", scancode);
}

void init_kernel_keyboard() {
  register_interrupt_handler(IRQ1, keyboard_callback);
}
