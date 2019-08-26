extern long_mode_start

%define KERNEL_VMA          0xFFFF800000000000
%define KERNEL_STACK_SIZE   4096*2

section .text
bits 32
global start
start:
    ; Note: All addressing before long jump to long_mode_start MUST be
    ; relative - substract KERNEL_VMA from all adresses
    
    ; save eax and ebx (grub magic and mb info) into esi and edi
    ; per GCC C ABI param specs 
    ; as it will be passed down to kernel main as arguments
    mov esi, ebx
    mov edi, eax

    ; setup stack
    mov esp, (kernel_stack_top - KERNEL_VMA)

    ; setup page entries to map kernel from KERNEL_VMA up to 2MB
    call set_up_page_tables
    ; setup cr4 and cr3 to enable pagging
    call enable_paging

    ; load the 64-bit GDT
    lgdt [(gdt64.pointer - KERNEL_VMA)]

    ; jump to refresh RIP and new GDT
    jmp gdt64.code:(long_mode_start - KERNEL_VMA)


; Maps memory from 0x0000 -> 2MB and KERNEL_VMA ->2MB
set_up_page_tables:
    ; map 0th P4 entry to P3 table
    mov eax, (pdpe_table - KERNEL_VMA)
    or eax, 0b11 ; present + writable
    mov [(pml4e_table - KERNEL_VMA)], eax

    ; and 0x100th to KERNEL_VMA
    mov ecx, 0x100
    mov [(pml4e_table - KERNEL_VMA) + ecx * 8], eax

    ; and 511 (last) to itself (recursive mappings : see paging.c)
    mov eax, (pml4e_table - KERNEL_VMA)
    or eax, 0b11
    mov ecx, 511
    mov [(pml4e_table - KERNEL_VMA) + ecx * 8], eax

    ; map first P3 entry to P2 table
    mov eax, (pde_table - KERNEL_VMA)
    or eax, 0b11 ; present + writable
    mov [(pdpe_table - KERNEL_VMA)], eax

    ; map first P2 entry to P1 table
    mov eax, (pte_table - KERNEL_VMA)
    or eax, 0b11 ; present + writable
    mov [(pde_table - KERNEL_VMA)], eax

    ; coutner or 4K tables
    mov ecx, 0
.map_pte_table:
    ; map ecx-th P2 entry to a huge page that starts at address 2MiB*ecx
    mov eax, 4096      ; 4K page
    mul ecx            ; start address of ecx-th page
    or eax, 0b00000011 ; writable(0x02) present (0x01)
    mov [(pte_table - KERNEL_VMA) + ecx * 8], eax ; map ecx-th entry

    inc ecx            ; increase counter
    cmp ecx, 512       ; if counter == 512, the whole P2 table is mapped
    jne .map_pte_table  ; else map the next entry

    ret

enable_paging:
    ; load P4 to cr3 register (cpu uses this to access the P4 table)
    mov eax, (pml4e_table - KERNEL_VMA)
    mov cr3, eax

    ; enable Physical Address Extension (PAE-flag) in cr4
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; set the long mode bit in the EFER MSR (model specific register)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; enable paging in the cr0 register
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ret

section .data
align 4096

global pml4e_table
pml4e_table:
    times 512 dq 0 ; 0 temporary to pdpe, 0x100 points to pdpe_table, last points to itself
global pdpe_table
pdpe_table:
    times 512 dq 0 ; 0th point to pde_table
global pde_table
pde_table: 
    times 512 dq 0 ; 0th points to pte_table
global pte_table
pte_table:
    times 512 dq 0 ; points incrementaly to all 4k pages
global tmp_mem_bitmap
tmp_mem_bitmap:
    times 512 dq 0 ; temporary mem table 1bit = 4K total of 128MB

section .data
gdt64:                           ; Global Descriptor Table (64-bit).
    .Null: equ $ - gdt64         ; The null descriptor.
    dw 0xFFFF                    ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0                         ; Access.
    db 1                         ; Granularity.
    db 0                         ; Base (high).
    .code: equ $ - gdt64         ; The code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10011010b                 ; Access (exec/read).
    db 10101111b                 ; Granularity, 64 bits flag, limit19:16.
    db 0                         ; Base (high).
    .data: equ $ - gdt64         ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10010010b                 ; Access (read/write).
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
    .pointer:                    ; The GDT-pointer.
    dw $ - gdt64 - 1             ; Limit.
    dq gdt64                     ; Base.


section .bss
align 4
kernel_stack_bottom:
    resb KERNEL_STACK_SIZE
global kernel_stack_top
kernel_stack_top:

align 4096
kernel_end:
    resb 4096
global kernel_end

