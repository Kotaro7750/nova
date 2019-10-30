#include "include/syscall.h"
#include "include/fbcon.h"
#include "include/intr.h"
#include "include/kbc.h"
#include "include/pic.h"

#define SYSCALL_INTR_NO 0x80

unsigned long long do_syscall_handler(unsigned long long syscall_id,
                                      unsigned long long arg1,
                                      unsigned long long arg2,
                                      unsigned long long arg3) {

  unsigned long long ret;
  switch (syscall_id) {
  case SYSCALL_ID_READ:
    return read(arg1, (void *)arg2, arg3);
    break;
  default:
    while (1) {
      ;
    }
    break;
  }

  set_pic_eoi(SYSCALL_INTR_NO);

  return ret;
}

unsigned long long syscall(unsigned long long syscall_id,
                           unsigned long long arg1, unsigned long long arg2,
                           unsigned long long arg3) {
  unsigned long long ret;
  asm volatile("movq %[syscall_id], %%rdi\n"
               "movq %[arg1], %%rsi\n"
               "movq %[arg2], %%rdx\n"
               "movq %[arg3], %%rcx\n"
               "int $0x80\n"
               "movq %%rax, %[ret]"
               : [ ret ] "=r"(ret)
               : [ syscall_id ] "D"(syscall_id), [ arg1 ] "S"(arg1),
                 [ arg2 ] "d"(arg2), [ arg3 ] "c"(arg3));
  return ret;
}

void syscall_handler(void);

void syscall_init(void) {
  void *handler;
  asm volatile("lea syscall_handler, %[handler]" : [ handler ] "=r"(handler));
  set_intr_desc(SYSCALL_INTR_NO, handler);
  enable_pic_intr(SYSCALL_INTR_NO);
}

unsigned long long read(unsigned long long fd, void *buf,
                        unsigned long long buf_size) {
  if (fd == 0) {
    return read_from_stdin((char *)buf, buf_size);
  }
  return 0;
}

unsigned long long read_from_stdin(char *buf, unsigned long long buf_size) {
  unsigned long long i;
  for (i = 0; i < buf_size - 1;) {
    buf[i] = getc();
    putc(buf[i]);
    if (buf[i] == '\n') {
      putc('\r');
      break;
    }
    i++;
  }
  buf[i] = '\0';

  return i;
}
