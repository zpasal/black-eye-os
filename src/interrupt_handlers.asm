section .texts
bits 64

extern interrupt_handler ; the C interrupt handler

; Create entry point for interrupt
; mov rdi interrupt no and jmp to common handler for all interrupts
%macro create_interrupt_handler 1
global interrupt_handler_%1 
interrupt_handler_%1:
    mov rdi, %1
    jmp common_interrupt_handler
%endmacro


; Generate 255 interrupt entries
%assign int_no 0
%rep 256
  create_interrupt_handler int_no
%assign int_no int_no+1
%endrep


common_interrupt_handler:
  ; save the registers
  push r15
  push r14
  push r13
  push r12
  push r11
  push r10
  push r9
  push r8

  push rdi
  push rsi
  push rbp

  push rdx
  push rcx
  push rbx
  push rax

  ; call the C function
  mov rsi, cr2
  call interrupt_handler
  
  pop rax
  pop rbx
  pop rcx
  pop rdx

  pop rbp
  pop rsi
  pop rdi

  pop r8
  pop r9
  pop r10
  pop r11
  pop r12
  pop r13
  pop r14
  pop r15

  ; return to the code that got interrupted
  iretq
