#ifndef _FBCON_H_
#define _FBCON_H_

void putc(char c);
void puts(char *str);
void puth(unsigned long long val, unsigned char num_digits);
void putb(unsigned long long val);

#endif
