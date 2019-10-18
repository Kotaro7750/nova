#include "include/paging.h"
#include "include/fbcon.h"

void show_current_page(void) {
  unsigned long long cr3;
  asm volatile("mov %%cr3,%%rax\n"
               "mov %%rax,%0\n"
               : "=m"(cr3));
  puth(cr3, 16);
}
