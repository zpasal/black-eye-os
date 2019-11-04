#include <pagging.h>
#include <kernel.h>
#include <memory.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <x86.h>
#include <isr.h>
#include <vesa.h>

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

uint64_t mmu_nframes; // Number of entries in bitset
uint32_t *mmu_frames; // Start address of bitset

uint64_t heap_start;
uint64_t heap_end;

pml4e_t pml4e[512] __attribute__((aligned(4096)));
pdpe_t  pdpe[512] __attribute__((aligned(4096)));
pde_t   pde[512] __attribute__((aligned(4096)));
pdpe_t  pdpe_video[512] __attribute__((aligned(4096)));
pde_t   pde_video[512] __attribute__((aligned(4096)));
pdpe_t  pdpe_user[512] __attribute__((aligned(4096)));
pde_t   pde_user[512] __attribute__((aligned(4096)));

////////////////////////////////////////////////////////////////////////
//
//  BIT MEMORY MAPPING (FRAMES)
//

static uint64_t round_up(uint64_t num, uint64_t multiple) {
  return ((num + multiple - 1) / multiple) * multiple;
}

static inline void mmu_frame_set(uint64_t frame_addr) {
  uint64_t frame = frame_addr / PAGE_SIZE;
  uint64_t idx = INDEX_FROM_BIT(frame);
  uint64_t off = OFFSET_FROM_BIT(frame);
  DEBUG("MMU: Frame set bit for 0x%X %i %i\n\r", frame_addr, idx, off);
  mmu_frames[idx] |= (0x1 << off);
}

// Static function to clear a bit in the frames bitset
static inline void mmu_frame_clear(uint64_t frame_addr) {
  uint64_t frame = frame_addr / PAGE_SIZE;
  uint64_t idx = INDEX_FROM_BIT(frame);
  uint64_t off = OFFSET_FROM_BIT(frame);
  mmu_frames[idx] &= ~(0x1 << off);
}

// Static function to test if a bit is set.
static inline uint64_t mmu_frame_test(uint64_t frame_addr) {
  uint64_t frame = frame_addr / PAGE_SIZE;
  uint64_t idx = INDEX_FROM_BIT(frame);
  uint64_t off = OFFSET_FROM_BIT(frame);
  return (mmu_frames[idx] & (0x1 << off));
}

uint64_t mmu_frame_find_first() {
  DEBUG("MMU: frame mmu_frame_find_first\n\r");
  uint64_t i, j;
  for (i = 0; i < INDEX_FROM_BIT(mmu_nframes); i++)
  {
    DEBUG("MMU: frame testing frame %i\n\r", i);
    if (mmu_frames[i] != 0xFFFFFFFF) // nothing free, exit early.
    {
      // at least one bit is free here.
      for (j = 0; j < 32; j++) {
        DEBUG("MMU: frame testing frame bit %i\n\r", j);
        uint32_t toTest = 0x1 << j;
        if (!(mmu_frames[i] & toTest)) {
          return i*4*8+j;
        }
      }
    }
  }
  return 0;
}

void dump_frames() {
  DEBUG("Frames:\n\r");
  for (uint64_t i=0; i<INDEX_FROM_BIT(mmu_nframes); i++) {
    DEBUG("0x%X ", mmu_frames[i]);
  }
  DEBUG("\n\r");
}
  
void* alloc_frame() {
  uint64_t frame = mmu_frame_find_first(); 
  if (frame == 0) {
    PANIC("------------- NOT ENOUGH MEMORY!!!!! -------------");
    return NULL; // never reached
  }
  mmu_frame_set(frame * PAGE_SIZE);
  return (void*)(frame * PAGE_SIZE);
}

void* alloc_frame_temp(uint64_t *phys_out) {
  uint64_t addr = (uint64_t)alloc_frame();
  DEBUG("MMU: alloc_frame_temp frame ... 0x%X\n\r", addr);
  *phys_out = addr;

  for (uint64_t i=255; i<512; i++) {
    if (pde_video[i].all == 0) {
      pde_video[i].all = addr | 0x83;
      x86_set_cr3(TO_PHYS_U64(pml4e));      
      return (void*)(0xFFFFC00000000000 + i * PAGE_SIZE);
    }
  }
  PANIC("NOT ENOUGH MEMORY TO MAP PROCESS SPACE");
  return NULL;
}
////////////////////////////////////////////////////////////////////////
//
//  PAGGING INIT FUNCTIONS (kernel space, user space and video memory)
//

static void init_mmu_frames(uint64_t memory_size) {
  mmu_nframes = memory_size / PAGE_SIZE;
  mmu_frames = (uint32_t*)&kernel_end;
  heap_start = round_up((uint64_t)(mmu_frames + mmu_nframes), 8);
  DEBUG("MMU: Frames %i, %i\n\r", mmu_nframes, INDEX_FROM_BIT(mmu_nframes));
  memset(mmu_frames, 0, INDEX_FROM_BIT(mmu_nframes) * sizeof(uint32_t));
  heap_end = round_up(heap_start, 0x200000); // Round up to end of page

  // MARK USED MEMORY
  // Dummy way : mark everything from 0x0000 - KERNEL_END (heap end)
  uint64_t kernel_end_p = TO_PHYS_U64(heap_end);
  uint64_t num_pages = kernel_end_p / PAGE_SIZE + 1;
  for (uint64_t i=0; i<num_pages; i++) {
    mmu_frame_set(i * PAGE_SIZE);
  }
  DEBUG("MMU: Used Mem %0iKB\n\r", TO_PHYS_U64(heap_end) / 1024 / 1024);
}

static void init_pagging_kernel() {
  // Setup new PAGGING DIRECTORY
  memset(&pml4e, 0, sizeof(pml4e_t) * 512);
  memset(&pdpe, 0, sizeof(pdpe_t) * 512);
  memset(&pde, 0, sizeof(pde_t) * 512);

  // Kernel space
  pml4e[0x100].all = TO_PHYS_U64(&pdpe) | 3; // KERNEL_VMA: Present + Write
  pdpe[0].all = TO_PHYS_U64(&pde) | 3; 
  uint64_t num_pages = TO_PHYS_U64(heap_end) / PAGE_SIZE;
  DEBUG("MMU: Krnl space page needed: %i\n\r", num_pages);

  for (uint64_t i=0; i<=num_pages; i++) {
    pde[i].all = (i * PAGE_SIZE) | 0x83; // Presetn + Write + Large (2MB)
  }
}

static void init_pagging_user() {
  // User space
  memset(&pdpe_user, 0, sizeof(pdpe_t) * 512);
  memset(&pde_user, 0, sizeof(pde_t) * 512);

  pml4e[0].all = TO_PHYS_U64(&pdpe_user) | 0x7; // USER_VMA: Present + Write + CPL3
  pdpe_user[0].all = TO_PHYS_U64(&pde_user) | 0x07; 
  // User PDE (pde_user) will be set during context switch
}

static void init_pagging_video() {
  // video memory (linear frame buffer)
  uint64_t video_framebuffer = video_info.addr;
  DEBUG("MMU: Video memory %0x\n\r", video_framebuffer);
  pml4e[0x180].all = TO_PHYS_U64(&pdpe_video) | 3; // KERNEL_VMA: Present + Write
  memset(&pdpe_video, 0, sizeof(pdpe_t) * 512);
  memset(&pde_video, 0, sizeof(pde_t) * 512);
  pdpe_video[0].all = TO_PHYS_U64(&pde_video) | 3; 
  for (uint64_t i=0; i<256; i++) {
    pde_video[i].all = (video_framebuffer + (i * PAGE_SIZE)) | 0x83; // Presetn + Write + Large (2MB)
  }  
}

////////////////////////////////////////////////////////////////////////
//
//  KERNEL HEAP IMPLEMEMNTATION
//

mblock_t *root_mblock;

void init_heap() {
  root_mblock = (mblock_t*)heap_start;
  root_mblock->free = true;
  root_mblock->magic = MBLOCK_MAGIC;
  root_mblock->size = heap_end - heap_start - sizeof(mblock_t);
  root_mblock->next = NULL;
  DEBUG("MMU: Heap init : start:0x%X end:0x%X\n\r", heap_start, heap_end);
  DEBUG("MMU: Heap first block size : %i\n\r", root_mblock->size);
}


mblock_t *find_heap_block(uint32_t size) {
  DEBUG("MMU: Heap find_heap_block %i\n\r", size);
  uint32_t req_size = size + sizeof(mblock_t);
  HEAP_WALKER(mb) {
    if (mb->free && mb->size > req_size) { // not >= ... little hack to not match last block in a byte 
      return mb;
    }
  }
  return NULL;
}

void debug_heap_dump() {
  HEAP_WALKER(mb) {
    DEBUG("MMU: mb: 0x%X mag:%X siz:%i free:%i\n\r", mb, mb->magic, mb->size, mb->free);
  }
}

void sbrk(uint32_t size) {
  DEBUG("MMU: Sbrk called : %i\n\r", size);
  // Find last entry and m ap to it (we are never freeing pages)
  for (uint64_t i=0; i<512; i++) {
    if (pde[i].all == 0) {
      uint64_t frame = (uint64_t)alloc_frame();
      pde[i].all = (frame * PAGE_SIZE) | 0x83;
      DEBUG("MMU: Sbrk new frame : 0x%X (0x%X)\n\r", frame, frame * PAGE_SIZE);
      break;
    }
  }
  x86_set_cr3(TO_PHYS_U64(pml4e));

  //Extend last memory block

  mblock_t *mb = root_mblock;
  while(mb->next != NULL) { mb = mb->next; }
  mb->size += PAGE_SIZE;
  heap_end += PAGE_SIZE;
  DEBUG("MMU: Heap after srbk 0x%X\n\r", TO_PHYS_U64(heap_end));
}

mblock_t *split_heap_block(mblock_t *mb, uint32_t size) {
  uint32_t old_size = mb->size;
  mblock_t *old_next = mb->next;
  uint8_t *ptr = (uint8_t*)mb;

  mb->free = false;
  mb->size = size;

  mblock_t *next_mb = (mblock_t*)(ptr + sizeof(mblock_t) + mb->size);
  mb->next = next_mb;
  next_mb->magic = MBLOCK_MAGIC;
  next_mb->size = old_size - size - sizeof(mblock_t);
  next_mb->next = old_next;
  next_mb->free = true;
  return mb;
}

void *malloc(uint32_t size) {
  DEBUG("HEAP: malloc %i\n\r", size);
  mblock_t *mb = find_heap_block(size);
  if (mb == NULL) {
    DEBUG("HEAP: No more empty blocks ... sbrk-ing ...\n\r");
    sbrk(size);
    return malloc(size);
  } 
  else {
    // If mb is larger than some specific size do split, else use whole memory block
    uint32_t diff = mb->size - size;
    DEBUG("HEAP: Block found: 0x%X mag:%X siz:%i free:%i\n\r", mb, mb->magic, mb->size, mb->free);
    DEBUG("HEAP: Diff %i\n\r", diff);
    if (diff >= HEAP_SPLIT_TRESHOLD) {
      mb = split_heap_block(mb, size);
    }
    else {
      mb->free = false;
    }
  }
  return ((uint8_t*)mb) + sizeof(mblock_t);
}

void free(void *ptr) {
  mblock_t *mb = (mblock_t *)(((uint8_t*)ptr) - sizeof(mblock_t));
  if (mb->magic != MBLOCK_MAGIC) {
    PANIC("HEAP: free(0x%X) - freeing bad block.\n\r");
  }
  mb->free = true;
}

void init_kernel_pagging() {
  init_mmu_frames(128 * 1024 * 1024); // 128Megs for now - TODO : autodetect);
  init_pagging_kernel();
  init_pagging_user();
  init_pagging_video();
  x86_set_cr3(TO_PHYS_U64(pml4e));
  init_heap();
}
