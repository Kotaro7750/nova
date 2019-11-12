#include "include/x86.h"

unsigned long long gdt[3];
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

// create_descriptor is a function to compose GDT entry
unsigned long long create_descriptor(unsigned long base, unsigned long limit,
                                     unsigned short flag) {
  unsigned long long descriptor = 0;

  // higher(32~64 bit) of descriptor
  descriptor = limit & 0x000F0000; // set limit bits 19:16
  descriptor |=
      (flag << 8) & 0x00F0FF00; // set type, p, dpl, s, g, d/b, l and avl fields
  descriptor |= (base >> 16) & 0x000000FF; // set base bits 23:16
  descriptor |= base & 0xFF000000;         // set base bits 31:24

  // Shift by 32 to allow for low part of segment
  descriptor <<= 32;

  // lower(0~31 bit) of descriptor
  descriptor |= base << 16;         // set base bits 15:0
  descriptor |= limit & 0x0000FFFF; // set limit bits 15:0

  return descriptor;
}

void gdt_init(void) {
  gdt[0] = create_descriptor(0, 0, 0);
  gdt[1] = create_descriptor(0, 0xffffffff, GDT_CODE_PL0);
  gdt[2] = create_descriptor(0, 0xffffffff, GDT_DATA_PL0);

  gdtr[0] = ((unsigned long long)gdt << 16) | (sizeof(gdt) - 1);
  gdtr[1] = ((unsigned long long)gdt >> 48);
  asm volatile("lgdt gdtr");

  // set data segment and stack segment
  asm volatile("movw $2*8, %ax\n"
               "movw %ax, %ds\n"
               "movw %ax, %ss\n");

  // set code segment
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
