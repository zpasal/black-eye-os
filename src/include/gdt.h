#ifndef __GDT_H
#define __GDT_H

#define GDT_NULL 				0
#define GDT_KERNEL_CODE 8
#define GDT_KERNEL_DATA 16
#define GDT_USER_CODE   24
#define GDT_USER_DATA   32
#define GDT_TSS         40  

// // #define KERNEL_NULL_SEGMENT_INDEX 0x00
// // #define KERNAL_GDT_SEGMENT_INDEX  0x08

// /* Defines a GDT entry. We say packed, because it prevents the
// *  compiler from doing things that it thinks is best: Prevent
// *  compiler "optimization" by packing */
// struct gdt_entry
// {
//     unsigned short limit_low;
//     unsigned short base_low;
//     unsigned char base_middle;
//     unsigned char access;
//     unsigned char granularity;
//     unsigned char base_high;
// } __attribute__((packed));


// /* Special pointer which includes the limit: The max bytes
// *  taken up by the GDT, minus 1. Again, this NEEDS to be packed */
// struct gdt_ptr
// {
//     unsigned short limit;
//     unsigned int base;
// } __attribute__((packed));
// // definitions of gate attributes

// #define LDT       0x200    // ldt segment
// #define TASK      0x500    // task gate
// #define TSS       0x900    // tss
// #define CALL      0x0C00   // 386 call gate
// #define INT       0x0E00   // 386 interrupt gate
// #define TRAP      0x0F00   // 386 trap gate
// #define DATA      0x1000   // data segment
// #define CODE      0x1800   // code segment

// #define DPL3      0x6000   // dpl3
// #define DLP2      0x4000   // dpl2
// #define DPL1      0x2000   // dpl1
// #define DPL0      0x0000   // dpl0
// #define PRESENT   0x8000   // present
// #define NPRESENT  0x8000   // not present
//                      			 // present is set by default, non-present to turn it off
// 			                     // present does the same

// #define ACC 			0x100    // accessed (ds/cs)
// #define WRITE   	0x200    // writable (cs)
// #define READ      0x200    // readable (cs)
// #define BUSY      0x200    // busy (cs)
// #define EXDOWN    0x400    // expand down (ds)
// #define CONFORM   0x400    // conforming (cs)
// #define BIG       0x40     // default to 32bit
// #define BIG_LIM   0x80     // limit in 4k units

// struct gdt_entry gdt[15];
// struct gdt_ptr gp;

// void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
// void gdt_install();
// void load_tss( TSS_t* tss_table )


#endif