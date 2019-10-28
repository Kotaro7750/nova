#include "include/syscall.h"
#include "include/fbcon.h"
#include "include/intr.h"
#include "include/pic.h"

#define SYSCALL_INTR_NO 0x80

unsigned long long do_syscall_handler(unsigned long long syscall_id,
                                      unsigned long long arg1,
                                      unsigned long long arg2,
                                      unsigned long long arg3) {

  unsigned long long ret_val = 0xbeefcafe01234567;

  puth(syscall_id, 16);
  puts("\r\n");
  puth(arg1, 16);
  puts("\r\n");
  puth(arg2, 16);
  puts("\r\n");
  puth(arg3, 16);
  puts("\r\n");

  set_pic_eoi(SYSCALL_INTR_NO);

  return ret_val;
}

void syscall_handler(void);

void syscall_init(void) {
  void *handler;
  asm volatile("lea syscall_handler, %[handler]" : [ handler ] "=r"(handler));
  set_intr_desc(SYSCALL_INTR_NO, handler);
  enable_pic_intr(SYSCALL_INTR_NO);
}
