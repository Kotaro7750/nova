#include "include/fb.h"
#include "include/fbcon.h"
#include "include/intr.h"
#include "include/kbc.h"
#include "include/pic.h"
#include "include/x86.h"

void start_kernel(void *_t __attribute__((unused)), struct framebuffer *_fb,
                  void *_fs_start __attribute__((unused))) {
  fb_init(_fb);
  set_fg(255, 255, 255);
  set_bg(0, 70, 255);
  clear_screen();
  gdt_init();
  intr_init();

  pic_init();
  kbc_init();

  enable_cpu_intr();
  while (1) {
    cpu_halt();
  }
}
