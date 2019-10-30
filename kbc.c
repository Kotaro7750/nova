#include "include/kbc.h"
#include "include/common.h"
#include "include/fbcon.h"
#include "include/intr.h"
#include "include/lock.h"
#include "include/pic.h"
#include "include/shell.h"
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

void kbc_handler(void);

struct queue keycode_queue;

void do_kbc_interrupt(void) {
  if (!(io_read(KBC_STATUS_ADDR) & KBC_STATUS_BIT_OBF)) {
    goto kbc_exit;
  }

  unsigned char keycode = io_read(KBC_DATA_ADDR);
  enqueue(&keycode_queue, keycode);
  if (keycode & KBC_DATA_BIT_IS_BRAKE) {
    goto kbc_exit;
  }

  if (keycode_queue.status == ERROR) {
    puts("ERROR!!\n");
    puth(keycode_queue.size, 16);
  }

kbc_exit:
  set_pic_eoi(KBC_INTR_NO);
}

void kbc_init(void) {
  queue_init(&keycode_queue);
  void *handler;
  asm volatile("lea kbc_handler, %[handler]" : [ handler ] "=r"(handler));
  set_intr_desc(KBC_INTR_NO, handler);
  enable_pic_intr(KBC_INTR_NO);
}

char get_keydata(void) {
  char data;
  while (1) {
    char ifflag;
    intr_lock(&ifflag);
    data = dequeue(&keycode_queue);
    if (keycode_queue.status == ERROR) {
      intr_unlock(&ifflag);
      continue;
    } else {
      intr_unlock(&ifflag);
      break;
    }
    intr_unlock(&ifflag);
  }
  return data;
}

char get_keycode(char keydata) { return keydata & ~KBC_DATA_BIT_IS_BRAKE; }

char get_pressed_keycode(void) {
  char keydata;
  while ((keydata = get_keydata()) & KBC_DATA_BIT_IS_BRAKE) {
    cpu_halt();
  }
  return get_keycode(keydata);
}

char getc(void) { return keymap[get_pressed_keycode()]; }

char getline(void) {
  char c = getc();
  if (('a' <= c && c <= 'z')) {
    c = c - 'a' + 'A';
  }
  return c;
}
