#ifndef __VESA_H
#define __VESA_H

#include <multiboot.h>
#include <kernel.h>

#define VESA_FRAMEBUFFER_8 ((uint8_t*)KERNEL_VIDEO_MEMORY.addr)
#define VESA_FRAMEBUFFER_32 ((uint32_t*)KERNEL_VIDEO_MEMORY)
#define VESA_FRAMEBUFFER_64 ((uint64_t*)KERNEL_VIDEO_MEMORY)

struct video_info_struct {
  uint64_t addr;
  uint32_t width;
  uint32_t height;
  uint8_t  bits;
  uint32_t pitch;
  uint8_t  type;
};

extern struct video_info_struct video_info;

void init_kernel_vesa(multiboot_info_t *mbi);

#endif
