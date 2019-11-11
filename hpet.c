#include "include/hpet.h"
#include "include/acpi.h"
#include "include/common.h"
#include "include/fbcon.h"
#include "include/intr.h"
#include "include/pic.h"

#define TIMER_N 0
#define US_TO_FS 1000000000

#define GCIDR_ADDR (reg_base)
#define GCIDR (*(volatile unsigned long long *)GCIDR_ADDR)
union gcidr {
  unsigned long long raw;
  struct __attribute__((packed)) {
    unsigned long long rev_id : 8;
    unsigned long long num_tim_cap : 5;
    unsigned long long count_size_cap : 1;
    unsigned long long _reserved : 1;
    unsigned long long leg_rt_cap : 1;
    unsigned long long vendor_id : 16;
    unsigned long long counter_clk_period : 32;
  };
};

#define GCR_ADDR (reg_base + 0x10)
#define GCR (*(volatile unsigned long long *)GCR_ADDR)
union gcr {
  unsigned long long raw;
  struct __attribute__((packed)) {
    unsigned long long enable_cnf : 1;
    unsigned long long leg_rt_cnf : 1;
    unsigned long long _reserved : 62;
  };
};

#define MCR_ADDR (reg_base + 0xf0)
#define MCR (*(volatile unsigned long long *)MCR_ADDR)

#define TNCCR_ADDR(n) (reg_base + (0x20 * (n)) + 0x100)
#define TNCCR(n) (*(volatile unsigned long long *)(TNCCR_ADDR(n)))
#define TNCCR_INT_TYPE_EDGE 0
#define TNCCR_INT_TYPE_LEVEL 1
#define TNCCR_TYPE_NON_PERIODIC 0
#define TNCCR_TYPE_PERIODIC 1
union tnccr {
  unsigned long long raw;
  struct __attribute__((packed)) {
    unsigned long long _reserved1 : 1;
    unsigned long long int_type_cnf : 1;
    unsigned long long int_enb_cnf : 1;
    unsigned long long type_cnf : 1;
    unsigned long long per_int_cap : 1;
    unsigned long long size_cap : 1;
    unsigned long long val_set_cnf : 1;
    unsigned long long _reserved2 : 1;
    unsigned long long mode32_cnf : 1;
    unsigned long long int_route_cnf : 5;
    unsigned long long fsb_en_cnf : 1;
    unsigned long long fsb_int_del_cap : 1;
    unsigned long long _reserved3 : 16;
    unsigned long long int_route_cap : 32;
  };
};

/* Timer N Comparator Register */
#define TNCR_ADDR(n) (reg_base + (0x20 * (n)) + 0x108)
#define TNCR(n) (*(volatile unsigned long long *)(TNCR_ADDR(n)))

void hpet_handler(void);
void (*user_handler)(unsigned long long current_rsp) = NULL;

struct __attribute__((packed)) HPET_TABLE {
  struct SDTH header;
  unsigned int event_timer_block_id;
  struct ACPI_ADDRESS base_address;
  unsigned char hpet_number;
  unsigned short minimum_tick;
  unsigned char flags;
};

unsigned long long reg_base;
unsigned int counter_clk_period;
unsigned long long cmpr_clk_counts;
unsigned char is_oneshot = 0;

void hpet_init(void) {
  struct HPET_TABLE *hpet_table = (struct HPET_TABLE *)get_sdt("HPET\0");

  reg_base = hpet_table->base_address.address;
  union gcr gcr;
  gcr.raw = GCR;
  gcr.enable_cnf = 0;
  gcr.leg_rt_cnf = 1;
  GCR = gcr.raw;

  union gcidr gcidr;
  gcidr.raw = GCIDR;
  counter_clk_period = gcidr.counter_clk_period;

  union tnccr tnccr;
  tnccr.raw = TNCCR(TIMER_N);
  tnccr.int_type_cnf = TNCCR_INT_TYPE_EDGE;
  tnccr.int_enb_cnf = 0;
  tnccr.type_cnf = TNCCR_TYPE_NON_PERIODIC;
  tnccr.val_set_cnf = 0;
  tnccr.mode32_cnf = 0;
  tnccr.fsb_en_cnf = 0;
  tnccr._reserved1 = 0;
  tnccr._reserved2 = 0;
  tnccr._reserved3 = 0;
  TNCCR(TIMER_N) = tnccr.raw;

  void *handler;
  asm volatile("lea hpet_handler, %[handler]" : [ handler ] "=r"(handler));
  set_intr_desc(HPET_INTR_NO, handler);

  enable_pic_intr(HPET_INTR_NO);
}

void dump_gcidr(void) {
  puts("GCIDR\r\n");

  union gcidr r;
  r.raw = GCIDR;

  puts("REV ID             ");
  putd(r.rev_id, 3);
  puts("\r\n");

  puts("NUM TIM CAP        ");
  putd(r.num_tim_cap, 2);
  puts("\r\n");

  puts("COUNT SIZE CAP     ");
  putd(r.count_size_cap, 1);
  puts("\r\n");

  puts("LEG RT CAP         ");
  putd(r.leg_rt_cap, 1);
  puts("\r\n");

  puts("COUNTER CLK PERIOD ");
  putd(r.counter_clk_period, 10);
  puts("\r\n");
}

void dump_gcr(void) {
  puts("GCR\r\n");

  union gcr r;
  r.raw = GCR;

  puts("ENABLE CNF ");
  putd(r.enable_cnf, 1);
  puts("\r\n");

  puts("LEG RT CNF ");
  putd(r.leg_rt_cnf, 1);
  puts("\r\n");
}

void dump_mcr(void) {
  puts("MCR ");
  puth(MCR, 16);
  puts("\r\n");
}

void do_hpet_interrupt(unsigned long long current_rsp) {
  if (is_oneshot == 1) {
    union gcr gcr;
    gcr.raw = GCR;
    gcr.enable_cnf = 0;
    GCR = gcr.raw;

    union tnccr tnccr;
    tnccr.raw = TNCCR(TIMER_N);
    tnccr.int_enb_cnf = 0;
    tnccr._reserved1 = 0;
    tnccr._reserved2 = 0;
    tnccr._reserved3 = 0;
    TNCCR(TIMER_N) = tnccr.raw;

    is_oneshot = 0;
  }

  if (user_handler)
    user_handler(current_rsp);

  set_pic_eoi(HPET_INTR_NO);
}

void alert(unsigned long long us, void *handler) {
  user_handler = handler;

  union tnccr tnccr;
  tnccr.raw = TNCCR(TIMER_N);
  tnccr.int_enb_cnf = 1;
  tnccr.type_cnf = TNCCR_TYPE_NON_PERIODIC;
  tnccr._reserved1 = 0;
  tnccr._reserved2 = 0;
  tnccr._reserved3 = 0;
  TNCCR(TIMER_N) = tnccr.raw;

  MCR = (unsigned long long)0;

  unsigned long long femt_sec = us * US_TO_FS;
  unsigned long long clk_counts = femt_sec / counter_clk_period;
  TNCR(TIMER_N) = clk_counts;

  is_oneshot = 1;

  union gcr gcr;
  gcr.raw = GCR;
  gcr.enable_cnf = 1;
  GCR = gcr.raw;
}

void ptimer_setup(unsigned long long us, void *handler) {
  union gcr gcr;
  gcr.raw = GCR;
  gcr.enable_cnf = 0;
  GCR = gcr.raw;

  user_handler = handler;

  union tnccr tnccr;
  tnccr.raw = TNCCR(TIMER_N);
  tnccr.int_enb_cnf = 1;
  tnccr.type_cnf = TNCCR_TYPE_PERIODIC;
  tnccr._reserved1 = 0;
  tnccr._reserved2 = 0;
  tnccr._reserved3 = 0;
  TNCCR(TIMER_N) = tnccr.raw;

  unsigned long long femt_sec = us * US_TO_FS;
  cmpr_clk_counts = femt_sec / counter_clk_period;
}

void ptimer_start(void) {
  union tnccr tnccr;
  tnccr.raw = TNCCR(TIMER_N);
  tnccr.val_set_cnf = 1;
  TNCCR(TIMER_N) = tnccr.raw;
  TNCR(TIMER_N) = cmpr_clk_counts;

  MCR = (unsigned long long)0;

  union gcr gcr;
  gcr.raw = GCR;
  gcr.enable_cnf = 1;
  GCR = gcr.raw;
}

void ptimer_stop(void) {
  union gcr gcr;
  gcr.raw = GCR;
  gcr.enable_cnf = 0;
  GCR = gcr.raw;

  union tnccr tnccr;
  tnccr.raw = TNCCR(TIMER_N);
  tnccr.int_enb_cnf = 0;
  tnccr._reserved1 = 0;
  tnccr._reserved2 = 0;
  tnccr._reserved3 = 0;
  TNCCR(TIMER_N) = tnccr.raw;
}
