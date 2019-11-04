#include <kernel.h>
#include <isr.h>

#define __UNUSED__ __attribute__((unused))


void system_call_handler(isr_ctx_t *regs __UNUSED__) {
  //kprintf_xy(10, 0, "SYSCALL: 0x%X", regs->rax);
}

