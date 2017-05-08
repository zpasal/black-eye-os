OUTPUT_FORMAT(elf64-x86-64)

KERNEL_LMA = 0x100000; 
KERNEL_VMA = 0xFFFF800000000000;

ENTRY(start)
SECTIONS
{
    . = KERNEL_LMA;
    _start = .;
    .boot : {
        _code = .;
        *(.multiboot_header)
    }

    . += KERNEL_VMA;
  
    .text : AT(ADDR(.text) - KERNEL_VMA)
    {
        _code = .;
        *(EXCLUDE_FILE(multiboot_header) .text)
        *(.text)
        . = ALIGN(4096);
    }

   .rodata ALIGN (0x1000) : AT(ADDR(.rodata) - KERNEL_VMA)
   {
        _rodata = .;
        *(.rodata)
        . = ALIGN(4096);
   }

   .data ALIGN (0x1000) : AT(ADDR(.data) - KERNEL_VMA)
   {
        _data = .;
        *(.data)
        . = ALIGN(4096);
   }

   .eh_frame ALIGN (0x1000) : AT(ADDR(.eh_frame) - KERNEL_VMA)
   {
       _ehframe = .;
       *(.eh_frame)
        . = ALIGN(4096);
   }

   .bss ALIGN (0x1000) : AT(ADDR(.bss) - KERNEL_VMA)
   {
       _bss = .;
       *(.bss)

       /*
        * You usually need to include generated COMMON symbols
        * under kernel BSS section or use gcc's -fno-common
        */

        *(COMMON)
       . = ALIGN(4096);
   }

   _end = .;

   /DISCARD/ :
   {
        *(.comment)
   }
}
