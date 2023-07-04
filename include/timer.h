#ifndef __TIMER_H__
#define __TIMER_H__

#include "reg.h"

void enable_stimer_interrupt() {
  write_csr("sie", read_csr("sie") | SIE_STIE);
}

void disable_stimer_interrupt() {
  write_csr("sie", read_csr("sie") & ~SIE_STIE);
}

#endif