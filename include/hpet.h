#ifndef _HPET_H_
#define _HPET_H_

#define HPET_INTR_NO 32
#define SEC_TO_US 1000000
#define MS_TO_US 1000

void hpet_init(void);
void dump_gcidr(void);
void dump_gcr(void);
void dump_mcr(void);
void alert(unsigned long long us, void *handler);
void ptimer_setup(unsigned long long us, void *handler);
void ptimer_start(void);
void ptimer_stop(void);
#endif
