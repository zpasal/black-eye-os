#ifndef __PIC_H_
#define __PIC_H_

#include <stdint.h>

#define IRQ_BASE  0x20

#define PIC_MASTER_CTRL 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_CTRL  0xA0
#define PIC_SLAVE_DATA  0xA1

#define PIC_IRQ0 0x00
#define PIC_IRQ1 0x01
#define PIC_IRQ2 0x02
#define PIC_IRQ3 0x03

void init_kernel_pic();
void pic_acknowledge(uint8_t irq);
void irq_enable(uint8_t irq);

#endif
