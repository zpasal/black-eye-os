#ifndef __PAGGING_H
#define __PAGGING_H

#include <stdint.h>

// 2MB Pages
#define PAGE_SIZE 0x200000

#define EMPTY_FRAME 0xFFFFFFFFFFFFFFFF

extern void* kernel_end;
extern uint64_t __kheap_start;
extern uint64_t __kheap_placement_address;

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

extern pml4e_t pml4e[512];
extern pdpe_t  pdpe[512];
extern pde_t   pde[512];
extern pdpe_t  pdpe_user[512];
extern pde_t   pde_user[512];

#define MBLOCK_MAGIC 0xDEAD 

struct mblock_struct {
  uint32_t  size;  
  uint16_t  magic;
  uint16_t  free;
  struct mblock_struct*  next;
};
typedef struct mblock_struct mblock_t;

#define HEAP_WALKER(name) for(mblock_t *name = root_mblock; name != NULL; name = name->next) 

#define HEAP_SPLIT_TRESHOLD (1024)

// not mapped - must be added to PDE if used by kernel.
void* alloc_frame(); 
// mapped temporary - next call will overwrite it 
void* alloc_frame_temp(uint64_t *phys_out);
void *malloc(uint32_t size);
void free(void* size);
void init_kernel_pagging();

#endif