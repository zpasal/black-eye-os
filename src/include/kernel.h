#ifndef __KERNEL_H
#define __KERNEL_H

#include <stdint.h>

void interrupt_handler();

void load_idt(void* idt);

#endif