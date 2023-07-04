#include "trap.h"
#include "types.h"
#include "vm.h"
#include "reg.h"

extern uint64_t __alltraps_k;
extern uint64_t __alltraps;

void set_kernel_trap_entry() {
  uint64_t __alltraps_k_va = __alltraps_k  - __alltraps + TRAMPOLINE;
  write_csr("stvec", __alltraps_k_va & (~3));
  write_csr("sscratch", (uint64_t)trap_from_kernel);
}

void set_user_trap_entry() {
  write_csr("stvec", TRAMPOLINE & (~3));
}

void trap_return() {
  set_user_trap_entry();
  unreachable();
}

void trap_from_kernel(TrapContext *ctx) {
  reg_t scause = read_csr("scause");
  reg_t sepc = read_csr("sepc");
  //switch(scause) {
    
  //}
  panic("trap from kernel");
}