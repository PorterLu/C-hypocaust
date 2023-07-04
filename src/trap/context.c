#include "context.h"
#include "malloc.h"

TrapContext* init_context(uint64_t entry, reg_t sp, reg_t kernel_satp, reg_t kernel_sp, uint64_t trap_handler) {
  reg_t hstatus = read_csr("hstatus");
  hstatus = hstatus | HSTATUS_SPV;
  TrapContext* ctx = malloc();
  for(int i = 0; i < 32; i++) {
    ctx->x[i] = 0;
  }
  ctx->hstatus = hstatus;
  ctx->kernel_satp = kernel_satp;
  ctx->kernel_sp = kernel_sp;
  ctx->trap_handler = trap_handler;
  ctx->x[2] = sp;
  return ctx;
}
