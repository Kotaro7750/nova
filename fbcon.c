#include "include/fbcon.h"
#include "include/fb.h"
#include "include/font.h"

/* 64bit unsignedの最大値0xffffffffffffffffは
 * 10進で18446744073709551615(20桁)なので'\0'含め21文字分のバッファで足りる */
#define MAX_STR_BUF 21

unsigned int cursor_x = 0;
unsigned int cursor_y = 0;

void putc(char c) {
  switch (c) {
  case '\r':
    cursor_x = 0;
    break;
  case '\n':
    cursor_x = 0;
    cursor_y += FONT_HEIGHT;
    if ((cursor_y + FONT_HEIGHT) >= fb.hr) {
      cursor_x = 0;
      cursor_y = 0;
      clear_screen();
    }
    break;
  default:
    // draw character using font
    for (int y = 0; y < FONT_HEIGHT; y++) {
      for (int x = 0; x < FONT_WIDTH; x++) {
        if (font_bitmap[(unsigned int)c][y][x]) {
          draw_px_fg(x + cursor_x, y + cursor_y);
        }
      }
    }
    // update cursor position
    cursor_x += FONT_WIDTH;
    if ((cursor_x + FONT_WIDTH) >= fb.vr) {
      cursor_x = 0;
      cursor_y += FONT_HEIGHT;
      if (cursor_y + FONT_HEIGHT >= fb.hr) {
        cursor_x = 0;
        cursor_y = 0;
        clear_screen();
      }
    }
    break;
  }
}

void puts(char *str) {
  while (*str != '\0') {
    putc(*str++);
  }
}

void putd(unsigned long long val, unsigned char num_digits) {
  char str[MAX_STR_BUF];

  int i;
  for (i = num_digits - 1; i >= 0; i--) {
    int digit = val % 10;
    str[i] = '0' + digit;
    val /= 10;
  }
  str[num_digits] = '\0';

  puts(str);
}
