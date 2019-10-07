#include "include/pic.h"
#include "include/x86.h"

#define MPIC_ICW1_ADDR 0x0020
#define MPIC_ICW2_ADDR 0x0021
#define MPIC_ICW3_ADDR 0x0021
#define MPIC_ICW4_ADDR 0x0021
#define MPIC_OCW1_ADDR 0x0021
#define MPIC_OCW2_ADDR 0x0020
#define MPIC_IMR_ADDR 0x0021

#define SPIC_ICW1_ADDR 0x00a0
#define SPIC_ICW2_ADDR 0x00a1
#define SPIC_ICW3_ADDR 0x00a1
#define SPIC_ICW4_ADDR 0x00a1
#define SPIC_OCW1_ADDR 0x00a1
#define SPIC_IMR_ADDR 0x00a1

#define INTR_NO_BASE_MASTER 32
#define INTR_NO_BASE_SLAVE 40

#define PIC_OCW2_BIT_MANUAL_EOI 0x60

void pic_init(void) {
  // refuse interrupt
  io_write(MPIC_IMR_ADDR, 0xff);
  io_write(SPIC_IMR_ADDR, 0xff);
  // master
  io_write(MPIC_ICW1_ADDR, 0x11);
  io_write(MPIC_ICW2_ADDR, INTR_NO_BASE_MASTER);
  io_write(MPIC_ICW3_ADDR, 0x04);
  io_write(MPIC_ICW4_ADDR, 0x01);
  io_write(MPIC_OCW1_ADDR, 0xff);

  // slave
  io_write(SPIC_ICW1_ADDR, 0x11);
  io_write(SPIC_ICW2_ADDR, INTR_NO_BASE_SLAVE);
  io_write(SPIC_ICW3_ADDR, 0x02);
  io_write(SPIC_ICW4_ADDR, 0x01);
  io_write(SPIC_OCW1_ADDR, 0xff);
}

// slave is not supported
void enable_pic_intr(unsigned char intr_no) {
  // make  bit field which only intr_no is 1
  unsigned char ir_no = intr_no - INTR_NO_BASE_MASTER;
  unsigned char ir_bit = 1U << ir_no;

  // get present mask width
  unsigned char mask = io_read(MPIC_IMR_ADDR);

  // do nothing if mask is already invalid
  if (!(ir_bit & mask))
    return;

  // make mask invalid
  io_write(MPIC_OCW1_ADDR, mask - ir_bit);
}

// slave is not supported
void set_pic_eoi(unsigned char intr_no) {
  // calculate IR number
  unsigned char ir_no = intr_no - INTR_NO_BASE_MASTER;
  // set EOI to ir_no
  io_write(MPIC_OCW2_ADDR, PIC_OCW2_BIT_MANUAL_EOI | ir_no);
}
