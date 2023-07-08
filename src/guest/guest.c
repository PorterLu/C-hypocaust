#include "types.h"
#include "elf.h"
#include "page.h"
#include "string.h"
#include "platform.h"
#include "reg.h"
#include "context.h"
#include "trap.h"
#include "guest.h" 
#include "malloc.h"
#include "print.h"

#define GUEST_START_HPA 0x88000000
#define GUEST_START_GVA 0x80000000
#define GUEST_SIZE 0x200000
#define STACK_SIZE 0x2000

static global_vmid;
VMB global_vmb_list[10];

uint64_t new_guest_kernel() {
  uint16_t ph_count = elf->e_phnum;
  Elf64_Phdr *ph = NULL;
  pagetable_t guest_pt = page_alloc(1);
  int perm = 0;
  uint64_t last_offset = 0;
  for(int i = 0; i < ph_count; i++) {
    ph = (uint8_t*)elf + elf->e_phoff + i * elf->e_phentsize;
    if(ph->p_type == PT_LOAD) {
      uint64_t p_vaddr = ph->p_vaddr;
      uint64_t p_offset = ph->p_offset;
      uint64_t p_filesz = ph->p_filesz;
      uint64_t p_memsz = ph->p_memsz;
      uint64_t inner_offset = p_vaddr - GUEST_START_GVA;
      memcpy((uint8_t*)inner_offset + GUEST_START_HPA, (uint8_t*)elf + p_offset, p_filesz);
      memset((uint8_t*)inner_offset + GUEST_START_HPA + p_filesz, 0, p_memsz - p_filesz);
      if(ph->p_flags & PF_R != 0) {
        perm |= PTE_R;
      }
      if(ph->p_flags & PF_W != 0) {
        perm |= PTE_W;
      }
      if(ph->p_flags & PF_X != 0) {
        perm |= PTE_X;
      }
      mappages(guest_pt, p_vaddr, PGROUNDUP(p_memsz),  PGROUNDDOWN(inner_offset + GUEST_START_HPA), perm);
      perm = 0;
      if(inner_offset + p_memsz > last_offset) {
        last_offset = inner_offset + p_memsz;
      }
    }
  }
  mappages(guest_pt, PGROUNDUP(GUEST_START_GVA + last_offset) , GUEST_SIZE - last_offset, GUEST_START_HPA + last_offset, 0);
  uint64_t vmid = global_vmid;
  global_vmb_list[vmid].vmid = vmid;
  global_vmb_list[vmid].pt = guest_pt;
  global_vmid ++;
  return vmid;
}

extern uint64_t STRAMPOLINE;
extern uint64_t ALLTRAPS;
extern uint64_t RESTORE;

void initialize_gpm(pagetable_t pt) {
  mappages(pt, TRAMPOLINE, PGSIZE, STRAMPOLINE, PTE_R | PTE_X);
  mappages(pt, TRAMPOLINE - PGSIZE, PGSIZE, page_alloc(1), PTE_R | PTE_X);
  mappages(pt, UART0, PGSIZE, UART0, PTE_R | PTE_W);
}

// set the new addr of __restore asm function in TRAMPOLINE page,
// set the reg a0 = trap_cx_ptr, reg a1 = phy addr of usr page table,
// finally, jump to new addr of __restore asm function
void switch_to_guest() {
  set_user_trap_entry();
  TrapContext* ctx = (TrapContext*)(TRAMPOLINE - PGSIZE);
  // guest physical address translation
  if(read_csr("hgatp") != ctx->hgatp) {
    write_csr("hgatp", ctx->hgatp);
    /*
    * rs1 = zero
    * rs2 = zero
    * HFENCE.GVMA
    * 0110001 00000 00000 000 00000 1110011
    */
    asm volatile (".word 0x62000073" ::: "memory");
  }

  write_csr("hstatus", read_csr("hstatus") | HSTATUS_SPV);
  write_csr("sstatus", read_csr("sstatus") | SSTATUS_SPP);

  printf("%lx\n%lx\n", ALLTRAPS, RESTORE);
  uint64_t restore_va = RESTORE - ALLTRAPS + TRAMPOLINE;

  register uintptr_t a0 asm("a0") = TRAMPOLINE - PGSIZE;
  register uintptr_t a1 asm("a1") = ctx->hgatp;

  asm volatile(
    "fence.i;" \
    "jr %0" ::"r"(restore_va):
  );
}

uint64_t hstack_position(uint64_t guest_id) {
  uint64_t bottom = TRAMPOLINE - PGSIZE - guest_id * (STACK_SIZE + PGSIZE);
  uint64_t top = bottom - STACK_SIZE;
  return top;
}

void hstack_alloc(uint64_t guest_id) {
  uint64_t top = hstack_position(guest_id);
  mappages(pagetable, top, STACK_SIZE, page_alloc(2), PTE_R | PTE_W);
  mappages(global_vmb_list[guest_id].pt, top, STACK_SIZE, page_alloc(2), PTE_R | PTE_W);
  TrapContext *ctx = (TrapContext*)(TRAMPOLINE - PGSIZE);
  TrapContext *tmp = init_context(
    GUEST_START_GVA,
    0,
    global_vmb_list[guest_id].pt,
    top,
    trap_handler
  );
  memcpy(ctx, tmp, MALLOC_CONTENT_SIZE);
  page_free(tmp);
}

void hentry() {  
  uint64_t vmid = new_guest_kernel();
  pagetable_t pt = global_vmb_list[vmid].pt;
  /*asm volatile("csrw satp, %0;" \
    "sfence.vma"::"r"(pt):
  );*/
  set_kernel_trap_entry();
  initialize_gpm(pt);
  printf("initialize gpm over\n");
  hstack_alloc(0);
  printf("hstack alloc over\n");
  switch_to_guest();
}