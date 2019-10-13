#include "include/fbcon.h"
#include "include/hpet.h"
#include "include/pic.h"

#define SCHED_PERIOD (4 * MS_TO_US)

void schedule(void) {
  set_pic_eoi(HPET_INTR_NO);
  putc('.');
}

void sched_init(void) { ptimer_setup(SCHED_PERIOD, schedule); }

void sched_start(void) { ptimer_start(); }
