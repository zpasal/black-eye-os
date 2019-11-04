#include <kernel.h>
#include <stdbool.h>
#include <string.h>
#include <vesa.h>
#include <x86.h>

struct video_info_struct video_info;

static void extract_framebuffer_info(multiboot_info_t *mbi) {
  multiboot_tag_t *tag;
  for (
    tag = (multiboot_tag_t *) mbi->tags;
    tag->type != MULTIBOOT_TAG_TYPE_END;
    tag = (multiboot_tag_t *) ((uint8_t *) tag + ((tag->size + 7) & ~7))
    ) {
      if (tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER) {
        struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *) tag;
        void *fb = (void *) (unsigned long) tagfb->common.framebuffer_addr;
            
        video_info.addr = (uint64_t)fb;
        video_info.width = tagfb->common.framebuffer_width;
        video_info.height = tagfb->common.framebuffer_height;
        video_info.bits = tagfb->common.framebuffer_bpp;
        video_info.pitch = tagfb->common.framebuffer_pitch;
        video_info.type = tagfb->common.framebuffer_type;
        DEBUG("VIDEO : %0x\r\n", video_info.addr);
      }
  }
}

void init_kernel_vesa(multiboot_info_t *mbi) {
    extract_framebuffer_info(mbi);
}

