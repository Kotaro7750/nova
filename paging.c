#include "include/paging.h"
#include "include/fbcon.h"

void show_current_page(void) {
  unsigned long long cr3;
  asm volatile("mov %%cr3,%%rax\n"
               "mov %%rax,%0\n"
               : "=m"(cr3));
  puth(cr3, 16);
}

void show_cr0(void) {
  unsigned long long cr0;
  asm volatile("mov %%cr0,%%rax\n"
               "mov %%rax,%0\n"
               : "=m"(cr0));
  puth(cr0, 16);
}

void disable_paging(void) {
  show_cr0();
  unsigned long long cr0;
  asm volatile("mov %%cr0,%%rax\n"
               "mov %%rax,%0\n"
               : "=m"(cr0));
  cr0 = cr0 & 0xffffffff7fffffff;
  puth(cr0, 16);
  asm volatile("mov %0,%%rax\n"
               "mov %%rax,%%cr0\n" ::"d"(cr0));
  show_cr0();
}
