#include "context.h"
#include "malloc.h"

TrapContext* init_context(uint64_t entry, reg_t sp, reg_t hgatp, reg_t kernel_sp, uint64_t trap_handler) {
  reg_t sstatus = read_csr("sstatus");
  sstatus = sstatus & (~SSTATUS_SPP);
  TrapContext* ctx = malloc();
  for(int i = 0; i < 32; i++) {
    ctx->x[i] = 0;
  }
  ctx->sepc = entry;
  ctx->sstatus = sstatus;
  ctx->hgatp = hgatp;
  ctx->kernel_sp = kernel_sp;
  ctx->trap_handler = trap_handler;
  ctx->x[2] = sp;
  return ctx;
}
