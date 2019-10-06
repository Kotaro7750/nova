#ifndef _INTR_H_K
#define _INTR_H_K
void set_intr_desc(unsigned char intr_no, void *handler);
void intr_init(void);
#endif
