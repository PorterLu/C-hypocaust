#include "hypervisor.h"
#include "reg.h"
#include "vm.h"
#include "sbi.h"

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
  write_hedeleg(read_hedeleg() | 1);
}
// breakpoint
void set_hbreak() {
  write_hedeleg(read_hedeleg() | (1 << 3));
}
// Environment call from U-mode or VU-mode 
void set_hecall_from_u() {
  write_hedeleg(read_hedeleg() | (1 << 8));
}
// Instruction page fault
void set_hinst_page() {
  write_hedeleg(read_hedeleg() | (1 << 12));
} 
// Load page fault 
void set_hload_page() {
  write_hedeleg(read_hedeleg() | (1 << 13));
}
// Store/AMO page fault 
void set_hstore_page() {
  write_hedeleg(read_hedeleg() | (1 << 14));
}
// hideleg: delegate all interrupts
void set_hexternal_int() {
  write_hideleg(read_hideleg() | (1 << 10));
}

void set_hsoft_int() {
  write_hideleg(read_hideleg() | (1 << 2));
}

void clear_htime_int() {
  write_hideleg(read_hideleg() & ~(1 << 6));
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
  set_hecall_from_u();
  set_hinst_page();
  set_hload_page();
  set_hstore_page();
  set_hexternal_int();
  set_hsoft_int();
  clear_htime_int();
  clear_hvip_all();

  write_csr("stvec", (uint64_t) trap_test & 0xc0);
}