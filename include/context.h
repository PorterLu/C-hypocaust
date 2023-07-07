#ifndef __CONTEXT_H__
#define __CONTEXT_H__
#include "reg.h"

typedef struct TrapContext {
  reg_t x[32];
  reg_t sstatus;
  reg_t sepc;
  reg_t hgatp;
  reg_t kernel_sp;
  reg_t trap_handler;
} TrapContext;

TrapContext* init_context(uint64_t entry, reg_t sp, reg_t hgatp, reg_t kernel_sp, uint64_t trap_handler);

#endif 