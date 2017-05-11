global long_mode_start
extern kmain

extern pml4e_table
extern kernel_stack_top

%define KERNEL_VMA          0xFFFF800000000000

section .text
bits 64
long_mode_start:
    ; load 0 into all data segment registers
    cli

    ; reset segments registers jun in case (in case of what?)
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Clear  pml4e 0th entry to remove flat memory mapping
    ; That will leave only 0xFFFF800000000000 address space
    mov rax, pml4e_table
    mov dword [rax], 0
    invlpg [0]

    ; setup new stack
    mov rsp, kernel_stack_top

    ; GRUB stores a pointer to a struct in the register ebx that,
    ; among other things, describes at which addresses the modules are loaded.
    ; Push ebx on the stack before calling main to make it an argument for main.
    call kmain
    hlt
