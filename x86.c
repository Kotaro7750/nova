#include "include/x86.h"

unsigned char io_read(unsigned short addr){
  unsigned char value;
  asm volatile ("inb %[addr],%[value]" : [value]"=a"(value) : [addr]"d"(addr));
  return value;
}
