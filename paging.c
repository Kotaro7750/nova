#include "include/paging.h"
#include "include/fbcon.h"
#include "include/intr.h"
#include "include/pic.h"

#define ENTRY_NUM 512
#define PG_INTR_NO 0x0e

unsigned long long PML4[ENTRY_NUM];

void page_fault_handler(void);

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

void paging_info(void) {
  unsigned long long cr0, cr3, cr4;

  asm volatile("mov %%cr0,%%rax\n"
               "mov %%rax,%0\n"
               : "=m"(cr0));
  unsigned long long pg = (cr0 & 0x80000000) >> 31;
  puts("PAGING ");
  putb(pg);
  puts("\n");

  asm volatile("mov %%cr3,%%rax\n"
               "mov %%rax,%0\n"
               : "=m"(cr3));

  unsigned long long PML4 = (cr3 & 0xfffff000) >> 12;

  puts("PML4 PHISICAL ADDRESS ");
  puth(PML4, 16);
  puts("\n");

  asm volatile("mov %%cr4,%%rax\n"
               "mov %%rax,%0\n"
               : "=m"(cr4));

  unsigned long long PAE = (cr4 & 0x20) >> 5;
  puts("PAE ");
  putb(PAE);
  puts("\n");
}

void switch_PML4(void) {
  unsigned long long cr3;
  asm volatile("mov %%cr3,%%rax\n"
               "mov %%rax,%0\n"
               : "=m"(cr3));
  puth(cr3, 16);
  puts("\n");
  unsigned long long FLAG = cr3 & 0xfff;
  unsigned long long phi_add = (unsigned long long)PML4;
  unsigned long long new_cr3 = (phi_add << 12) | FLAG;

  puth(new_cr3, 16);
  puts("\n");

  puth(PML4, 16);
  puts("\n");
  asm volatile("mov %0,%%rax\n"
               "mov %%rax,%%cr3\n" ::"m"(new_cr3));
}

void copy_PML4(void) {
  unsigned long long cr3;
  asm volatile("mov %%cr3,%%rax\n"
               "mov %%rax,%0\n"
               : "=m"(cr3));

  cr3 = cr3 >> 12;

  unsigned long long *cur_PML4 = (unsigned long long *)cr3;
  for (int i = 0; i < ENTRY_NUM; i++) {
    PML4[i] = cur_PML4[i];
  }
}

void do_page_fault_handler(void) {
  puts("abababababab\n");
  set_pic_eoi(PG_INTR_NO);
}

void pg_init(void) {
  void *handler;
  asm volatile("lea page_fault_handler, %[handler]"
               : [ handler ] "=r"(handler));
  set_intr_desc(PG_INTR_NO, handler);
  enable_pic_intr(PG_INTR_NO);
}
