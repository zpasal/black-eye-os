#include <timer.h>
#include <isr.h>
#include <ports.h>
#include <memory.h>
#include <console.h>
#include <x86.h>

uint64_t __tick = 0;

void timer_callback(stack_t *stack) {
    __tick++;

    // Change color - top-right char
    *(CONSOLE_VIDEO_MEMORY+159) += 1;

    (void)(*stack); // dummy - disable unused warning
}

uint64_t timer_tick() {
    return __tick;
}

void init_kernel_timer() {
    // register_interrupt_handler(IRQ0, timer_callback);
    int divisor = 1193180 / TIMER_HZ;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xff);
    outb(0x40, divisor >> 8);
}

void timer_enable() {
    x86_sti();
}

void timer_disable() {
    x86_cli();
}

void krnl_delay(unsigned int delay) {
    uint64_t start_pit, end_pit, gap;

    start_pit = __tick;
    gap = delay / (1000 / TIMER_HZ);
    end_pit = start_pit + gap;

    while (__tick < end_pit);
}
