#ifndef __PROCESS_H
#define __PROCESS_H

#include <stdint.h>
#include <pagging.h>

typedef struct {
  uint64_t rsp;
  uint32_t id;
  uint32_t attrib;
  // For now we are supporting only 2MB programs :) 
  // We keep track of one entry in PDE that will be mapped to 0x0000000 (user program space)
  pde_t pde; 
}  __attribute__((packed)) task_t;

#endif