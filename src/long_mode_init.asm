global long_mode_start
extern main

extern pml4e_table
extern stack_top

section .text
bits 64
long_mode_start:
    ; load 0 into all data segment registers
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Clear p4_table entry 0 (leave only 0xFFFF800000000000 address space)
    mov rax, pml4e_table
    mov dword [rax], 0
    invlpg [0]

    ; setup new stack
    mov rsp, stack_top

    ; call the kernel main
    call main
    hlt