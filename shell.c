#include "include/shell.h"
#include "include/common.h"
#include "include/fbcon.h"
#include "include/syscall.h"

void shsh(void) {
  char buf[BUF_SIZE];
  while (1) {
    puts("$ ");
    int n = syscall(0, 0, (unsigned long long)buf, BUF_SIZE);
    puth(n, 16);
    puts("\n");
  }
}
