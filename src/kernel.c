#include <multiboot.h>
#include <console.h>
#include <stdint.h>
#include <string.h>
#include <interrupts.h>
#include <kernel.h>
#include <memory.h>
#include <elf.h>
#include <x86.h>

#define __UNUSED__ __attribute__((unused))



#define MEM_BITMAP_SIZE (4096*8)

extern uint8_t* tmp_mem_bitmap;

void init_phys_allocator(uint64_t rbx, __UNUSED__ uint64_t rax) {
	// Allign to KERNEL memory space
	rbx = TO_VMA_U64(rbx);

	unsigned size = *(unsigned *)rbx;
	printf("Announced mbi size 0x%x\n", size);
	struct multiboot_tag *tag;

	for (tag = (struct multiboot_tag *)(rbx + 8);
		tag->type != MULTIBOOT_TAG_TYPE_END;
		tag = (struct multiboot_tag *) ((multiboot_uint8_t *) tag + ((tag->size + 7) & ~7)))
	{

		// We are interested only in ELF Sections Tag 
		if (tag->type == MULTIBOOT_TAG_TYPE_ELF_SECTIONS) {
			struct multiboot_tag_elf_sections *sec = ((struct multiboot_tag_elf_sections *) tag);
			printf("%i %i %i %i %i %s\n", sec->type, sec->size, sec->num, sec->entsize, sec->shndx, sec->sections);			
			
			//dump_bytes(sec->sections, 160);

			Elf64_Shdr* s = (Elf64_Shdr*)sec->sections;
			uint64_t max = 0;
			for (uint32_t i=0; i<sec->num; i++, s++) {
				// printf("s %i %i %p %X\n", s->sh_name, s->sh_type, s->sh_addr, s->sh_size);

				uint64_t addr = s->sh_addr + s->sh_size;
				if (addr < KERNEL_START_MEMORY) {
					addr += KERNEL_START_MEMORY ;
				}
				if (max < addr) {
					max = addr;
				}
			}
			//printf("End of kernel @ %X\n", max);
		}
		else if (tag->type == MULTIBOOT_TAG_TYPE_MMAP) {
			multiboot_memory_map_t *mmap;
			for (mmap = ((struct multiboot_tag_mmap *) tag)->entries;
				(multiboot_uint8_t *) mmap < (multiboot_uint8_t *) tag + tag->size;
				mmap = (multiboot_memory_map_t *)((unsigned long) mmap + ((struct multiboot_tag_mmap *) tag)->entry_size)) {
	
				printf (" base_addr = 0x%x%x,"
					" length = 0x%x%x, type = 0x%x\n",
					(unsigned) (mmap->addr >> 32),
					(unsigned) (mmap->addr & 0xffffffff),
					(unsigned) (mmap->len >> 32),
					(unsigned) (mmap->len & 0xffffffff),
					(unsigned) mmap->type);
			}

		}

	}

}

void membmp_set_bmp(uint64_t i) {
    tmp_mem_bitmap[i/8] = tmp_mem_bitmap[i/8] | (1<<(uint8_t)(i%8));
}

void membmp_clear_bmp(uint64_t i) {
    tmp_mem_bitmap[i/8] = tmp_mem_bitmap[i/8] & ~( 1<<(uint8_t)(i%8) );
}

int get_mbmp_bit(uint64_t i) {
	return tmp_mem_bitmap[i/8] & (1<<(uint8_t)(i % 8)) ? 1 : 0;
}

void* phys_alloc() {
    for(uint64_t i=0; i<MEM_BITMAP_SIZE; i++) {
        if (!get_mbmp_bit(i)) {
            membmp_set_bmp(i);
            return (void*)(i<<12);
        }
    }
	return NULL;
}

void phys_afree(void *ptr) {
	uint64_t i = (uint64_t)ptr;
    if ((i % 4096) || (!i)) {
        printf("Bad pointer passed to free_page((void *)(%08X)).\n", i);
        return ;
    }
    membmp_clear_bmp(i / 4096);
}


// From loader.asm - frst pages for mapping
extern uint64_t* pml4e_table;
extern uint64_t* pdpe_table;
extern uint64_t* pde_table;
extern uint64_t* pte_table;

/*

0xfffffffffffff000 - PML4
0xffffffffffe00000 - PDPs
0xffffffffc0000000 - page directories
0xffffff8000000000 - page tables
*/


#define PAGE_MASK 0x000ffffffffff000

#define PML4_INDEX(virt) ((((uint64_t)(virt)) >> 39) & 511)
#define PML3_INDEX(virt) ((((uint64_t)(virt)) >> 30) & 511)
#define PML2_INDEX(virt) ((((uint64_t)(virt)) >> 21) & 511)
#define PML1_INDEX(virt) ((((uint64_t)(virt)) >> 12) & 511)

#define PML4_TABLE       ((uint64_t*)(0xFFFFFFFFFFFFF000ull))
#define PML3_TABLE(virt) ((uint64_t*)(0xFFFFFFFFFFE00000ull + ((((uint64_t)(virt)) >> 27) & 0x00001FF000ull)))
#define PML2_TABLE(virt) ((uint64_t*)(0xFFFFFFFFC0000000ull + ((((uint64_t)(virt)) >> 18) & 0x003FFFF000ull)))
#define PML1_TABLE(virt) ((uint64_t*)(0xFFFFFF8000000000ull + ((((uint64_t)(virt)) >> 9) &  0x7FFFFFF000ull)))

#define PML4_ENTRY(virt) PML4_TABLE[PML4_INDEX(virt)]
#define PML3_ENTRY(virt) PML3_TABLE(virt)[PML3_INDEX(virt)]
#define PML2_ENTRY(virt) PML2_TABLE(virt)[PML2_INDEX(virt)]
#define PML1_ENTRY(virt) PML1_TABLE(virt)[PML1_INDEX(virt)]



/*
uint64_t* pm4_table() {
	return pml4e_table;
}

uint64_t* pm3_table(void* virt) {
	return pml4e_table;
}

uint64_t pml3_table(void* virt) {
	return 
}


void vmm_page_map(uint64_t virt, uint64_t phys, uint64_t flags) {

}*/


void interrupt_handler(int no, uint64_t cr2) {
  	printf("Interrupt (0x%X) @ (0x%X)\nKernel execution paused.", no, cr2);
  	x86_hlt();
}

void init_console() {
	__krnl_console.buffer = (char*)0xFFFF8000000B8000;
	console_init(&__krnl_console);
}

void kmain(uint64_t rbx, uint64_t rax) {
	init_console();
	init_idt();
	init_phys_allocator(rbx, rax);

	printf("BlackEye OS main()\n");

}
