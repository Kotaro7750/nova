#include "include/acpi.h"
#include "include/fb.h"
#include "include/fbcon.h"
#include "include/fs.h"
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
  fs_init(pi->fs_start);

  syscall_init();

  enable_cpu_intr();

  // shsh();

  struct file *hello = open("HELLO.TXT");
  if (hello) {
    puts((char *)hello->name);
    putc(' ');
    puts((char *)hello->data);
  } else {
    puts("HELLO.TXT IS NOT FOUND.");
  }
  puts("\r\n");

  struct file *foo = open("FOO.TXT");
  if (foo) {
    puts((char *)foo->name);
    putc(' ');
    puts((char *)foo->data);
  } else {
    puts("FOO.TXT IS NOT FOUND.");
  }
  puts("\r\n");

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
