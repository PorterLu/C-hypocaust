#ifndef __REG_H__
#define __REG_H__

#include "types.h"

#define SSTATUS   0x100
#define SIE       0x104
#define STVEC     0x105
#define SSCRATCH  0x140
#define SEPC      0x141
#define SCAUSE    0x142
#define STVAL     0x143
#define SIP       0x144
#define SATP      0x180

#define HGATP     0x680

#define SSTATUS_SPP (1 << 8)
#define SSTATUS_SPIE (1 << 5)
#define SSTATUS_SIE (1 << 1)
#define SIE_STIE (1 << 5)

#define TRAP_MODE_DIRECT 0
#define TRAP_MODE_VECTOR 1

#define HSTATUS_SPV (1 << 7)
#define HSTATUS_SPVP (1 << 8)

#define INST_ADDR_MISALIGN (1 << 0)
#define INST_ACCESSS_FAULT  (1 << 1)
#define ILLEGAL_INST  (1 << 2)
#define BREAKPOINT  (1 << 3)
#define LOAD_ADDR_MISALIGNED  (1 << 4)
#define LOAD_ACCESS_FAULT  (1 << 5)
#define STORE_ADDR_MISALIGNED  (1 << 6)
#define STORE_ACCESS_FAULT  (1 << 7)
#define ENV_CALL_FROM_U_OR_VU  (1 << 8)
#define ENV_CALL_FROM_HS  (1 << 9)
#define ENV_CALL_FROM_VS  (1 << 10)
#define ENV_CALL_FROM_M  (1 << 11)
#define INST_PAGE_FAULT  (1 << 12)
#define LOAD_PAGE_FAULT  (1 << 13)
#define STORE_PAGE_FAULT  (1 << 15)
#define INST_GUEST_PAGE_FAULT  (1 << 20)
#define LOAD_GUEST_PAGE_FAULT  (1 << 21)
#define VIRTUAL_INST  (1 << 22)
#define STORE_GUEST_PAGE_FAULT  (1 << 23)

#define VSSIP (1 << 2)
#define VSTIP (1 << 6)
#define VSEIP (1 << 10)

typedef struct context {
  reg_t zero;
  reg_t ra;
  reg_t sp;
  reg_t gp;
  reg_t tp;
  reg_t t0;
  reg_t t2;
  reg_t s0;
  reg_t s1;
  reg_t a0;
  reg_t a1;
  reg_t a2;
  reg_t a3;
  reg_t a4;
  reg_t a5;
  reg_t a6;
  reg_t a7;
  reg_t s2;
  reg_t s3;
  reg_t s4;
  reg_t s5;
  reg_t s6;
  reg_t s7;
  reg_t s8;
  reg_t s9;
  reg_t s10;
  reg_t s11;
  reg_t t3;
  reg_t t4;
  reg_t t5;
  reg_t t6;
  
  reg_t pc;
  
  reg_t sstatus;
  reg_t sepc;
  reg_t scause;
  reg_t stval;
} context;

#define write_csr(csr_name, write_data)   ({                \
  asm volatile("csrw " csr_name ", %0"::"r"(write_data));    \
})

#define read_csr(csr_name) ({                               \
  reg_t x;                                                  \
  asm volatile("csrr %0, " csr_name: "=r"(x):);             \
  x;                                                        \
})

#endif