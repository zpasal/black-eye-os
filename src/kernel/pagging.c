#include <pagging.h>
#include <kernel.h>
#include <kheap.h>
#include <memory.h>
#include <stdint.h>
#include <string.h>
#include <kheap.h>
#include <bitset.h>
#include <x86.h>

bitset_t frame_bitset;


#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

typedef union pml4e_struct {
  uint64_t all;
  struct {
    uint64_t present         : 1;   // Page present in memory
    uint64_t writable        : 1;   // Read-only if clear, readwrite if set
    uint64_t user            : 1;   // Supervisor level only if clear
    uint64_t write_through   : 1;   // Write through flag
    uint64_t disable_cache   : 1;   // Disable cache 
    uint64_t accessed        : 1;   // Has the page been accessed since last refresh?
    uint64_t ignore_1        : 1;   // Ignored
    uint64_t mbz_1           : 1;   // must be zero
    uint64_t mbz_2           : 1;   // must be zero
    uint64_t avail_1         : 3;   // Available for OS to use
    uint64_t address         : 40;  // Address
    uint64_t avail_2         : 11;  // Available for OS to use
    uint64_t no_exec         : 1;   // Frame address (shifted right 12 bits)
  } fields;
} __attribute__((packed)) pml4e_t;

typedef union pdpe_struct {
  uint64_t all;
  struct {
    uint64_t present         : 1;   // Page present in memory
    uint64_t writable        : 1;   // Read-only if clear, readwrite if set
    uint64_t user            : 1;   // Supervisor level only if clear
    uint64_t write_through   : 1;   // Write through flag
    uint64_t disable_cache   : 1;   // Disable cache 
    uint64_t accessed        : 1;   // Has the page been accessed since last refresh?
    uint64_t ignore_1        : 1;   // Ignored
    uint64_t mbz_1           : 1;   // must be zero
    uint64_t ignore_2        : 1;   // Ignored
    uint64_t avail_1         : 3;   // Available for OS to use
    uint64_t address         : 40;  // Address
    uint64_t avail_2         : 11;  // Available for OS to use
    uint64_t no_exec         : 1;   // Frame address (shifted right 12 bits)
  } fields;
} __attribute__((packed)) pdpe_t;

typedef union pde_struct {
  uint64_t all;
  struct {
    uint64_t present         : 1;   // Page present in memory
    uint64_t writable        : 1;   // Read-only if clear, readwrite if set
    uint64_t user            : 1;   // Supervisor level only if clear
    uint64_t write_through   : 1;   // Write through flag
    uint64_t disable_cache   : 1;   // Disable cache 
    uint64_t accessed        : 1;   // Has the page been accessed since last refresh?
    uint64_t dirty           : 1;   // Ignored
    uint64_t mbo_1           : 1;   // Must be one
    uint64_t global          : 1;   // Global
    uint64_t avail_1         : 3;   // Available for OS to use
    uint64_t pat             : 1;   // PAT translation mode
    uint64_t mbz_1           : 8;   // Must be zero
    uint64_t address         : 31;  // Address
    uint64_t avail_2         : 11;  // Available for OS to use
    uint64_t no_exec         : 1;   // Frame address (shifted right 12 bits)
  } fields;
} __attribute__((packed)) pde_t;

static inline void bitset_set(bitset_t* bitset, uint64_t frame_addr)
{
    uint64_t frame = frame_addr / 0x1000;
    uint64_t idx = INDEX_FROM_BIT(frame);
    uint64_t off = OFFSET_FROM_BIT(frame);
    bitset->frames[idx] |= (0x1 << off);
}

// Static function to clear a bit in the frames bitset
static inline void bitset_clear(bitset_t* bitset, uint64_t frame_addr)
{
    uint64_t frame = frame_addr / 0x1000;
    uint64_t idx = INDEX_FROM_BIT(frame);
    uint64_t off = OFFSET_FROM_BIT(frame);
    bitset->frames[idx] &= ~(0x1 << off);
}

// Static function to test if a bit is set.
static inline uint64_t bitset_test(bitset_t* bitset, uint64_t frame_addr)
{
    uint64_t frame = frame_addr / 0x1000;
    uint64_t idx = INDEX_FROM_BIT(frame);
    uint64_t off = OFFSET_FROM_BIT(frame);
    return (bitset->frames[idx] & (0x1 << off));
}

static inline uint64_t bitset_find_first(bitset_t* bitset)
{
    uint64_t i, j;
    for (i = 0; i < INDEX_FROM_BIT(bitset->nframes); i++)
    {
        if (bitset->frames[i] != 0xFFFFFFFF) // nothing free, exit early.
        {
            // at least one bit is free here.
            for (j = 0; j < 32; j++) {
                uint32_t toTest = 0x1 << j;
                if (!(bitset->frames[i] & toTest)) {
                    return i*4*8+j;
                }
            }
        }
    }
    return 0;
}

static inline void bitset_init(bitset_t* bitset, uint64_t size) {
  bitset->nframes = size;
  bitset->frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(bitset->nframes));
  memset(bitset->frames, 0, INDEX_FROM_BIT(bitset->nframes));
}

uint64_t new_cr3;
pml4e_t pml4e[512] __attribute__((aligned(4096)));
pdpe_t  pdpe[512] __attribute__((aligned(4096)));
pde_t   pde[512] __attribute__((aligned(4096)));

void init_kernel_pagging() {
  kputs("MMU PAGGING: init ...");

  // Setup new PAGGING DIRECTORY
  memset(&pml4e, 0, sizeof(pml4e_t) * 512);
  memset(&pdpe, 0, sizeof(pdpe_t) * 512);
  memset(&pde, 0, sizeof(pde_t) * 512);

  // pml4e[0].all = TO_PHYS_U64(&pdpe) | 3;     // 0 mem    : Presetn + Write
  pml4e[0x100].all = TO_PHYS_U64(&pdpe) | 3; // KERNEL_VMA: Presetn + Write
  pdpe[0].all = TO_PHYS_U64(&pde) | 3; 

  for (uint64_t i=0; i<512; i++) {
    pde[i].all = (i * PAGE_SIZE) | 0x83; // Presetn + Write + Large (2MB)
  }

  kprintf("MMU FRAME: P4[0]: 0x%X\n", pml4e[0].all);
  kprintf("MMU FRAME: P4[0x100]: 0x%X\n", pml4e[0x100].all);
  kprintf("MMU FRAME: P3: 0x%X\n", pdpe[0].all);
  kprintf("MMU FRAME: P2: 0x%X\n", pde[0].all);
  new_cr3 = TO_PHYS_U64(pml4e);
  __asm__ __volatile__("mov %0, %%cr3":: "r"(new_cr3));
  // x86_write_cr3(TO_PHYS_U64(pml4e));


  uint64_t memory_size = 128 * 1024 * 1024; // 128Megs for now - TODO : autodetect

  bitset_init(&frame_bitset, memory_size / PAGE_SIZE);
  kprintf("MMU FRAME: Bitset used mem: %i\n", INDEX_FROM_BIT(frame_bitset.nframes));

  // mark used memory
  // Dummy way : mark everything from 0x0000 - KERNEL_END
  uint64_t kernel_end_p = TO_PHYS_U64(__kheap_placement_address);
  uint64_t num_pages = kernel_end_p / PAGE_SIZE + 1;
  kprintf("MMU FRAME: End of kernel mem: 0x%X PAGES=%i\n", kernel_end_p, num_pages);

  for (uint64_t i=0; i<num_pages; i++) {
    bitset_set(&frame_bitset, i * PAGE_SIZE);
  }
  uint64_t testaddr = bitset_find_first(&frame_bitset);
  kprintf("MMU FRAME: New page: 0x%X\n", testaddr);
}
