#include "detect.h"
#include "hypervisor.h"

static reg_t store_stvec, store_sstatus;

void unsupported_extension() {
  panic("Don't support H-Extension");
}

void init_detect_trap() {
  store_stvec = read_csr("stvec");
  store_sstatus = read_csr("sstatus");

  // disable interrupt
  write_csr("sstatus", read_csr("sstatus") & ~SSTATUS_SIE);
  write_csr("stvec", (uint64_t)unsupported_extension & 0xb1 | TRAP_MODE_DIRECT);
}

void restore_detect_trap() {
  write_csr("sstatus", store_sstatus);
  write_csr("stvec", store_stvec);
}

bool detect_h_extension() {
  reg_t ans;
  init_detect_trap();
  asm volatile("csrr %0, hgatp":"=r"(ans));
  restore_detect_trap();
  return true;
}

