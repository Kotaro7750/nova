#include "include/acpi.h"
#include "include/fb.h"
#include "include/fbcon.h"
#include "include/intr.h"
#include "include/kbc.h"
#include "include/pic.h"
#include "include/x86.h"

struct __attribute__((packed)) platform_info {
  struct framebuffer fb;
  void *rsdp;
};

void start_kernel(void *_t __attribute__((unused)), struct platform_info *pi,
                  void *_fs_start __attribute__((unused))) {
  fb_init(&pi->fb);
  set_fg(255, 255, 255);
  set_bg(0, 70, 255);
  clear_screen();
  acpi_init(pi->rsdp);

  gdt_init();
  intr_init();

  pic_init();
  kbc_init();

  enable_cpu_intr();
  dump_xsdt();
  struct SDTH *hpet_table = get_sdt("HPET");
  dump_sdth_sig(hpet_table);

  while (1) {
    cpu_halt();
  }
}
