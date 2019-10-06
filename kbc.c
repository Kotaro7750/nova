#include "include/kbc.h"
#include "include/fbcon.h"
#include "include/intr.h"
#include "include/pic.h"
#include "include/x86.h"

#define KBC_DATA_ADDR 0x0060
#define KBC_DATA_BIT_IS_BRAKE 0x80
#define KBC_STATUS_ADDR 0x0064
#define KBC_STATUS_BIT_OBF 0x01

#define KBC_INTR_NO 33

const char keymap[] = {
    0x00, ASCII_ESC, '1',  '2',  '3',      '4',      '5',  '6',  '7',  '8',
    '9',  '0',       '-',  '^',  ASCII_BS, ASCII_HT, 'q',  'w',  'e',  'r',
    't',  'y',       'u',  'i',  'o',      'p',      '@',  '[',  '\n', 0x00,
    'a',  's',       'd',  'f',  'g',      'h',      'j',  'k',  'l',  ';',
    ':',  0x00,      0x00, ']',  'z',      'x',      'c',  'v',  'b',  'n',
    'm',  ',',       '.',  '/',  0x00,     '*',      0x00, ' ',  0x00, 0x00,
    0x00, 0x00,      0x00, 0x00, 0x00,     0x00,     0x00, 0x00, 0x00, 0x00,
    0x00, '7',       '8',  '9',  '-',      '4',      '5',  '6',  '+',  '1',
    '2',  '3',       '0',  '.',  0x00,     0x00,     0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,      0x00, 0x00, 0x00,     0x00,     0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,      0x00, 0x00, 0x00,     0x00,     0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,      0x00, 0x00, 0x00,     '_',      0x00, 0x00, 0x00, 0x00,
    0x00, 0x00,      0x00, 0x00, 0x00,     '\\',     0x00, 0x00};
// when key is pressed, first bit of 0x0064 is 1.
unsigned char get_kbc_data(void) {
  while (!(io_read(KBC_STATUS_ADDR) & KBC_STATUS_BIT_OBF)) {
  }
  return io_read(KBC_DATA_ADDR);
}

// when key is pressed, 8th bit of 0x0060 is 1
unsigned char get_keycode(void) {
  unsigned char keycode;

  while ((keycode = get_kbc_data()) & KBC_DATA_BIT_IS_BRAKE) {
  }
  return keycode;
}

char getc(void) { return keymap[get_keycode()]; }

void kbc_handler(void);

void do_kbc_interrupt(void) {
  puts("HOGE");
  if (!(io_read(KBC_STATUS_ADDR) & KBC_STATUS_BIT_OBF)) {
    goto kbc_exit;
  }

  unsigned char keycode = io_read(KBC_DATA_ADDR);
  if (keycode & KBC_DATA_BIT_IS_BRAKE) {
    goto kbc_exit;
  }
  char c = keymap[keycode];
  if (('a' <= c && c <= 'z')) {
    c = c - 'a' + 'A';
  } else if (c == '\n') {
    putc('\r');
  }
  putc(c);

kbc_exit:
  set_pic_eoi(KBC_INTR_NO);
}

void kbc_init(void) {
  set_intr_desc(KBC_INTR_NO, kbc_handler);
  enable_pic_intr(KBC_INTR_NO);
}
