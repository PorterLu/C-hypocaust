.altmacro
.macro SAVE_GP n
  sd x\n, \n*8(sp)
.endm

.macro LOAD_GP n
  ld x\n, \n*8(sp)
.endm

  .section .text.trampoline
  .global __alltraps
  .global __restore
  .global __alltraps_k
  .global __restore_k
  .align 12
__alltraps:
  csrrw sp, sscratch, sp
  # now sp points to *TrapContext in user space, sscratch points to user stack
  sd x1, 1*8(sp)
  # skip sp, save it later
  sd x3, 3*8(sp)
  .set n, 5
  .rept 27
    SAVE_GP %n
    .set n, n+1
  .endr
  # use t0/t1/t2 freely, because they have been saved in TrapContext
  csrr t0, sstatus
  csrr t1, sepc
  sd t0, 32*8(sp)
  sd t1, 33*8(sp)
  # read user stack from sscratch and save it in TrapContext
  csrr t2, sscratch
  sd t2, 2*8(sp)
  # load kernel_satp into t0
  ld t0, 34*8(sp)
  # load trap_handler into t1
  ld t1, 36*8(sp)
  # move kernel sp
  ld sp, 35*8(sp)
  # let VS guest switch to HS Hypervisor
  jr t1

__restore:
  # At this moment, we don't have to change page table, as we are still
  # in HS mode and use satp to translate address
  # a0: *TrapContext in user Space(constant); a1: userspace token
  # switch to user space
  csrw sscratch, a0
  mv sp, a0
  # new sp points to TrapContext in user space
  # restore sstatus/sepc
  ld t0, 32*8(sp)
  ld t1, 33*8(sp)
  csrw sstatus, t0
  csrw sepc, t1
  # restore gps
  ld x1, 1*8(sp)
  ld x3, 3*8(sp)
  .set n, 5
  .rept 27
    LOAD_GP %n
    .set n, n+1
  .endr
  # back to user stack
  ld sp, 2*8(sp)
  sret

  .align 2
__alltraps_k:
  addi sp, sp, -34*8
  sd x1, 1*8(sp)
  sd x3, 3*8(sp)
  .set n, 5
  .rept 27
    SAVE_GP %n
    .set n, n+1
  .endr
  csrr t0, sstatus
  csrr t1, sepc
  sd t0, 32*8(sp)
  sd t1, 33*8(sp)
  mv a0, sp
  csrr t2, sscratch
  jalr t2

__restore_k:
  ld t0, 32*8(sp)
  ld t1, 33*8(sp)
  csrw sstatus, t0
  csrw sepc, t1
  ld x1, 1*8(sp)
  ld x3, 3*8(sp)
  .set n, 5
  .rept 27
    LOAD_GP %n
    .set n, n+1
  .endr
  addi sp, sp, 34*8
  sret