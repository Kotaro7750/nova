#ifndef _PIC_H_
#define _PIC_H_

#define cli() asm("cli\n")
#define sti() asm("sti\n")

void pic_init(void);
void enable_pic_intr(unsigned char intr_no);
void set_pic_eoi(unsigned char intr_no);

#endif
