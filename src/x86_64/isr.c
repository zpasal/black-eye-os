#include <isr.h>
#include <ports.h>
#include <kernel.h>
#include <x86.h>
#include <string.h>

#define NB_REGISTERS_PUSHED_BEFORE_CALL 15

static void page_fault_handler(isr_ctx_t *regs);


isr_t interrupt_handlers[256];
const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void init_kernel_isr()
{
    memset(interrupt_handlers, 0, 256 * sizeof(isr_t));

    kputs("ISR: init irq ...");

    // start initialization
    outp(PIC1, 0x11);
    outp(PIC2, 0x11);

    // set IRQ base numbers for each PIC
    outp(PIC1_DATA, IRQ_BASE);
    outp(PIC2_DATA, IRQ_BASE + 8);

    // use IRQ number 2 to relay IRQs from the slave PIC
    outp(PIC1_DATA, 0x04);
    outp(PIC2_DATA, 0x02);

    // finish initialization
    outp(PIC1_DATA, 0x01);
    outp(PIC2_DATA, 0x01);

    // mask all IRQs
    outp(PIC1_DATA, 0x00);
    outp(PIC2_DATA, 0x00);

    kputs("ISR: init gates ...");
    set_idt_gate(0, (uint64_t) isr_stub_0);
    set_idt_gate(1, (uint64_t) isr_stub_1);
    set_idt_gate(2, (uint64_t) isr_stub_2);
    set_idt_gate(3, (uint64_t) isr_stub_3);
    set_idt_gate(4, (uint64_t) isr_stub_4);
    set_idt_gate(5, (uint64_t) isr_stub_5);
    set_idt_gate(6, (uint64_t) isr_stub_6);
    set_idt_gate(7, (uint64_t) isr_stub_7);
    set_idt_gate(8, (uint64_t) isr_stub_8);
    set_idt_gate(9, (uint64_t) isr_stub_9);
    set_idt_gate(10, (uint64_t) isr_stub_10);
    set_idt_gate(11, (uint64_t) isr_stub_11);
    set_idt_gate(12, (uint64_t) isr_stub_12);
    set_idt_gate(13, (uint64_t) isr_stub_13);
    set_idt_gate(14, (uint64_t) isr_stub_14);
    set_idt_gate(15, (uint64_t) isr_stub_15);
    set_idt_gate(16, (uint64_t) isr_stub_16);
    set_idt_gate(17, (uint64_t) isr_stub_17);
    set_idt_gate(18, (uint64_t) isr_stub_18);
    set_idt_gate(19, (uint64_t) isr_stub_19);
    set_idt_gate(20, (uint64_t) isr_stub_20);
    set_idt_gate(21, (uint64_t) isr_stub_21);
    set_idt_gate(22, (uint64_t) isr_stub_22);
    set_idt_gate(23, (uint64_t) isr_stub_23);
    set_idt_gate(24, (uint64_t) isr_stub_24);
    set_idt_gate(25, (uint64_t) isr_stub_25);
    set_idt_gate(26, (uint64_t) isr_stub_26);
    set_idt_gate(27, (uint64_t) isr_stub_27);
    set_idt_gate(28, (uint64_t) isr_stub_28);
    set_idt_gate(29, (uint64_t) isr_stub_29);
    set_idt_gate(30, (uint64_t) isr_stub_30);
    set_idt_gate(31, (uint64_t) isr_stub_31);

    set_idt_gate(IRQ0, (uint64_t) isr_stub_32);
    set_idt_gate(IRQ1, (uint64_t) isr_stub_33);
    set_idt_gate(IRQ2, (uint64_t) isr_stub_34);
    set_idt_gate(IRQ3, (uint64_t) isr_stub_35);
    set_idt_gate(IRQ4, (uint64_t) isr_stub_36);

    register_interrupt_handler(14, page_fault_handler);

    kputs("ISR: Setting IDT");
    set_idt();
    kputs("ISR: enabling interrupts");
}

void isr_handler(isr_ctx_t *regs)
{
    uint8_t int_no = regs->int_no;

    if (interrupt_handlers[int_no] != 0) {
        isr_t handler = interrupt_handlers[int_no];
        handler(regs);
        return;
    }

    PANIC(
        "Received interrupt: %d - %s\n"
        "  error_code          = 0x%X\n"
        "  instruction_pointer = 0x%X\n"
        "  code_segment        = 0x%X\n"
        "  cpu_flags           = 0x%X\n"
        "  stack_pointer       = 0x%X\n"
        "  stack_segment       = 0x%X",
        int_no, exception_messages[int_no],
        regs->error_code,
        regs->rip,
        regs->cs,
        regs->rflags,
        regs->rsp,
        regs->ss
    );
    while(1) {}
}

// void irq_handler(uint64_t id, uint64_t stack_addr)
// {
//     if (id >= 40) {
//         outp(PIC2, PIC_EOI);
//     }

//     outp(PIC1, PIC_EOI);

//     if (interrupt_handlers[id] != 0) {
//         isr_t handler = interrupt_handlers[id];
//         handler(get_stack(id, stack_addr));
//     }
// }

void register_interrupt_handler(uint64_t id, isr_t handler) {
    interrupt_handlers[id] = handler;
}

static void page_fault_handler(isr_ctx_t *regs __attribute__((unused)) ) {
    uint64_t faulting_address;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

    PANIC("PAGW FAULT on 0x%X\n", faulting_address);
}

