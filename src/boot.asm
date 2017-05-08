extern long_mode_start

%define KERNEL_VMA 0xFFFF800000000000

section .texts
bits 32
global start
start:
    ; setup stack
    mov esp, (stack_top - KERNEL_VMA)

    ; setup page entries to map kernel from KERNEL_VMA up to 2MB
    call set_up_page_tables
    ; setup cr4 and cr3 to enable pagging
    call enable_paging

    ; ; load the 64-bit GDT
    lgdt [(gdt64.pointer - KERNEL_VMA)]


    jmp gdt64.code:(long_mode_start - KERNEL_VMA)


; Maps memory from 0x0000 -> 2MB and KERNEL_VMA ->2MB
set_up_page_tables:
    ; map 1th and 0x100th (KERNEL_VMA) P4 entry to P3 table
    mov eax, (pdpe_table - KERNEL_VMA)
    or eax, 0b11 ; present + writable
    mov [(pml4e_table - KERNEL_VMA)], eax

    mov ecx, 0x100
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
    times 512 dq 0 ; 0x100 points to pdpe_table
pdpe_table:
    times 512 dq 0 ; 0th point to pde_table
pde_table: 
    times 512 dq 0 ; 0th points to pte_table
pte_table:
    times 512 dq 0 ; points incrementaly to all 4k pages

section .rodata
gdt64:
    dq 0 ; zero entry
.code: equ $ - gdt64 ; new
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) ; code segment
.pointer:
    dw $ - gdt64 - 1
    dq gdt64


section .bss
align 4096
stack_bottom:
    resb 512
global stack_top
stack_top:
