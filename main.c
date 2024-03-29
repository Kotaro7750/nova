#include "include/acpi.h"
#include "include/fb.h"
#include "include/fbcon.h"
#include "include/hpet.h"
#include "include/intr.h"
#include "include/kbc.h"
#include "include/pic.h"
#include "include/sched.h"
#include "include/shell.h"
#include "include/syscall.h"
#include "include/x86.h"

struct __attribute__((packed)) platform_info {
  struct framebuffer fb;
  void *rsdp;
  void *fs_start;
};

void handler(void);
void do_taskA(void);

void start_kernel(void *_t __attribute__((unused)), struct platform_info *pi,
                  void *_fs_start) {
  fb_init(&pi->fb);
  set_fg(255, 255, 255);
  set_bg(0, 70, 255);
  clear_screen();
  acpi_init(pi->rsdp);

  gdt_init();
  intr_init();

  pic_init();
  hpet_init();
  kbc_init();
  sched_init();

  syscall_init();

  enable_cpu_intr();

  shsh();

  while (1) {
    cpu_halt();
  }
}

void handler(void) {
  static unsigned char counter = 0;
  if (counter < 10)
    putc('0' + counter++);
  else
    ptimer_stop();
}

void do_taskA(void) {
  while (1) {
    putc('A');
    volatile unsigned long long wait = 10000000;
    while (wait--)
      ;
  }
}
