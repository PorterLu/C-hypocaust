#include "hypervisor.h"
#include "reg.h"
#include "vm.h"
#include "sbi.h"

static reg_t store_stvec, store_sstatus;

#define HEDELEG 0x602
#define HIDELEG 0x601

reg_t read_hedeleg() {
  return read_csr("hedeleg");
}

void write_hedeleg(reg_t value) {
  write_csr("hedeleg", value);
}

reg_t read_hideleg() {
  return read_csr("hideleg");
}

void write_hideleg(reg_t value) {
  write_csr("hideleg", value);
}

// Instruction address misaligned
void set_hinst_misaligned() {
  write_hedeleg(read_hedeleg() | INST_ADDR_MISALIGN);
}
// breakpoint
void set_hbreak() {
  write_hedeleg(read_hedeleg() | BREAKPOINT);
}
// Environment call from U-mode or VU-mode 
void set_hecall_from_u() {
  write_hedeleg(read_hedeleg() | ENV_CALL_FROM_U_OR_VU);
}
// Instruction page fault
void set_hinst_page() {
  write_hedeleg(read_hedeleg() | INST_PAGE_FAULT);
} 
// Load page fault 
void set_hload_page() {
  write_hedeleg(read_hedeleg() | LOAD_PAGE_FAULT);
}
// Store/AMO page fault 
void set_hstore_page() {
  write_hedeleg(read_hedeleg() | STORE_PAGE_FAULT);
}
// hideleg: delegate all interrupts
void set_hexternal_int() {
  write_hideleg(read_hideleg() | VSEIP);
}

void set_hsoft_int() {
  write_hideleg(read_hideleg() | VSSIP);
}

void set_htime_int() {
  write_hideleg(read_hideleg() | VSTIP);
}

// hvip: clear all interrupts
void clear_hvip_all() {
  write_csr("hvip", read_csr("hvip") & (~(1 << 2)));
  write_csr("hvip", read_csr("hvip") & (~(1 << 6)));
  write_csr("hvip", read_csr("hvip") & (~(1 << 10)));
}

void trap_test() {
  SBI_CALL(SHUT_DOWN, 0, 0, 0, 0);
}

void init_hypervisor() {
  set_hinst_misaligned();
  set_hbreak();
  //set_hecall_from_u();
  set_hinst_page();
  set_hload_page();
  set_hstore_page();
  set_hexternal_int();
  set_hsoft_int();
  set_htime_int();
  clear_hvip_all();

  write_csr("stvec", (uint64_t) trap_test & 0xc0);
}

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
