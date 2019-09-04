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

// // definitions of gate attributes

// #define LDT         0x200   // ldt segment
// #define TASK      0x500   // task gate
// #define TSS         0x900   // tss
// #define CALL      0x0C00   // 386 call gate
// #define INT         0x0E00   // 386 interrupt gate
// #define TRAP      0x0F00   // 386 trap gate
// #define DATA      0x1000   // data segment
// #define CODE      0x1800   // code segment

// #define DPL3      0x6000   // dpl3
// #define DLP2      0x4000   // dpl2
// #define DPL1      0x2000   // dpl1
// #define DPL0      0x0000   // dpl0
// #define PRESENT      0x8000   // present
// #define NPRESENT   0x8000   // not present
//                      // present is set by default, non-present to turn it off
//                      // present does the same