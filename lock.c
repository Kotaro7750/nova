#include "include/lock.h"

#define cli() asm("cli\n")
#define sti() asm("sti\n")

void intr_lock(char *ifflag) {
  unsigned long long rflag;

  asm volatile("pushf\n"
               "pop %[rflag]\n"
               : [ rflag ] "=r"(rflag));

  *ifflag = (rflag & 0x200) ? 1 : 0;

  if (*ifflag) {
    cli();
  }
}

void intr_unlock(char *ifflag) {
  if (ifflag) {
    sti();
  }
}
