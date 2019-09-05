#ifndef __PROCESS_H
#define __PROCESS_H

#include <stdint.h>

typedef struct {
  uint64_t rsp;
  uint32_t id;
  uint32_t attrib;
}  __attribute__((packed)) task_t;

#endif