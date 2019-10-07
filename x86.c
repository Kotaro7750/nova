#include "include/x86.h"

const unsigned long long gdt[] = {
    0x0000000000000000, /* NULL descriptor */
    0x00af9a000000ffff, /* base=0, limit=4GB, mode=code(r-x),kernel */
    0x00cf93000000ffff  /* base=0, limit=4GB, mode=data(rw-),kernel */
};

unsigned long long gdtr[2];

unsigned char io_read(unsigned short addr) {
  unsigned char value;
  asm volatile("inb %[addr],%[value]"
               : [ value ] "=a"(value)
               : [ addr ] "d"(addr));
  return value;
}

void io_write(unsigned short addr, unsigned char value) {
  asm volatile("outb %[value], %[addr]" ::[value] "a"(value),
               [ addr ] "d"(addr));
}

void gdt_init(void) {
  gdtr[0] = ((unsigned long long)gdt << 16) | (sizeof(gdt) - 1);
  gdtr[1] = ((unsigned long long)gdt >> 48);
  asm volatile("lgdt gdtr");

  // data segment and stack segment
  asm volatile("movw $2*8, %ax\n"
               "movw %ax, %ds\n"
               "movw %ax, %ss\n");

  // code segment
  unsigned short selector = SS_KERNEL_CODE;
  unsigned long long dummy;
  asm volatile("pushq %[selector];"
               "leaq ret_label(%%rip), %[dummy];"
               "pushq %[dummy];"
               "lretq;"
               "ret_label:"
               : [ dummy ] "=r"(dummy)
               : [ selector ] "m"(selector));
}

void enable_cpu_intr(void) { asm volatile("sti"); }
void cpu_halt(void) { asm volatile("hlt"); }
