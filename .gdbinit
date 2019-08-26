file build/kernel-x86_64.bin
target remote :1234
layout asm
layout regs
break irq0
