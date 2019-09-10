# Black Eye OS


Exploring new concepts of OS design with 64bit processors.


# Prerequirments
* nasm
* quemu (quemu-system-x86)
* grub-mkrescue
* git :)

       
876543210987 654332109 876543210 987654321 098765432 109876543210

# LOADER BIG PAGE 2MB

               P4(9)   v  P3(9)    P2(9)          A(21)
|           |         |         |         |         |            |
| 63 - 48   | 47 - 39 | 38 - 30 | 29 - 21 |       20 - 0         |
876543210987 654332109 876543210 987654321 098765432109876543210

P4 @ 0x00000000 4K - 512 entries 1
P3 @ 0x00001000 4K - 512 entries 1
P2 @ 0x00002000 4K - 512 entries 512


1 P4 -> 1 P3 -> 512 2MiB P2 tables


# INTERRUPT STACK

    +------------+
+40 | %SS        |
+32 | %RSP       |
+24 | %RFLAGS    |
+16 | %CS        |
 +8 | %RIP       |
  0 | ERROR CODE | <-- %RSP
    +------------+

| STACK 0xffff80000010e2b0 [0xffff8000:0x00101c2c]
| STACK 0xffff80000010e2b8 [0x00000000:0x00000008]
| STACK 0xffff80000010e2c0 [0x00000000:0x00000286]
| STACK 0xffff80000010e2c8 [0xffff8000:0x0010e2d0]
| STACK 0xffff80000010e2d0 [0x00000000:0x00000010]

CPU0:
rax: 00000000_00000001 rcx: 00000000_00000003
rdx: 00000000_00000004 rbx: 00000000_00000002
rsp: ffff8000_0010e2d0 rbp: 00000000_00000007
rsi: 00000000_00000005 rdi: 00000000_00000006
r8 : 00000000_00000008 r9 : 00000000_00000009
r10: 00000000_0000000a r11: 00000000_0000000b
r12: 00000000_0000000c r13: 00000000_0000000d
r14: 00000000_0000000e r15: 00000000_0000000f
rip: ffff8000_00101c2c
eflags 0x00000286: id vip vif ac vm rf nt IOPL=0 of df IF tf SF zf af PF cf
<bochs:7> sreg
es:0x0010, dh=0x00009300, dl=0x00000000, valid=1
	Data segment, base=0x00000000, limit=0x00000000, Read/Write, Accessed
cs:0x0008, dh=0x00209900, dl=0x00000000, valid=1
	Code segment, base=0x00000000, limit=0x00000000, Execute-Only, Non-Conforming, Accessed, 64-bit
ss:0x0010, dh=0x00009300, dl=0x00000000, valid=1
	Data segment, base=0x00000000, limit=0x00000000, Read/Write, Accessed
ds:0x0010, dh=0x00009300, dl=0x00000000, valid=1
	Data segment, base=0x00000000, limit=0x00000000, Read/Write, Accessed
fs:0x0010, dh=0x00009300, dl=0x00000000, valid=1
	Data segment, base=0x00000000, limit=0x00000000, Read/Write, Accessed
gs:0x0010, dh=0x00009300, dl=0x00000000, valid=1
	Data segment, base=0x00000000, limit=0x00000000, Read/Write, Accessed
ldtr:0x0000, dh=0x00008200, dl=0x0000ffff, valid=1
tr:0x0028, dh=0x00008b10, dl=0x90000067, valid=1
gdtr:base=0x0000000000109068, limit=0x37
idtr:base=0xffff800000113020, limit=0xfff