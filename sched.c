#include "include/fbcon.h"
#include "include/hpet.h"
#include "include/pic.h"

#define SCHED_PERIOD (4 * MS_TO_US)
#define NUM_TASKS 2

unsigned long long task_sp[NUM_TASKS];
volatile unsigned int current_task = 0;

void schedule(unsigned long long current_rsp) {
  task_sp[current_task] = current_rsp;
  current_task = (current_task + 1) % NUM_TASKS;
  set_pic_eoi(HPET_INTR_NO);
  asm volatile("mov %[sp], %%rsp" ::[sp] "a"(task_sp[current_task]));
}

void sched_init(void) { ptimer_setup(SCHED_PERIOD, schedule); }

void sched_start(void) { ptimer_start(); }
