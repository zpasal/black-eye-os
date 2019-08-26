#ifndef __FLOPPY_H
#define __FLOPPY_H

#include <types.h>

#define i8259_MASTER_CMD_PORT 0x20
#define i8259_EOI_DATA 0x20

int floppy_init();
void *floppy_read(void *buf, addr_t dev_loc, size_t cnt);

#endif 