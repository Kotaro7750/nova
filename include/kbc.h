#ifndef _KBC_H_
#define _KBC_H_

#define ASCII_ESC 0x1b
#define ASCII_BS 0x08
#define ASCII_HT 0x09

void kbc_init(void);
char getc(void);

#endif
