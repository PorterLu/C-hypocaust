#include "types.h"
#include "elf.h"
#include "vm.h"
#include "page.h"
#include "string.h"
#include "platform.h"

#define GUEST_START_HPA 0x88000000
#define GUEST_START_GVA 0x80000000
#define GUEST_SIZE 0x200000
pagetable_t new_guest_kernel() {
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
      mappages(guest_pt, p_vaddr, PGROUNDUP(p_memsz),  PGROUNDDOWN(inner_offset + GUEST_START_HPA), perm | PTE_V);
      perm = 0;
      if(inner_offset + p_memsz > last_offset) {
        last_offset = inner_offset + p_memsz;
      }
    }
  }
  mappages(guest_pt, PGROUNDUP(GUEST_START_GVA + last_offset) , GUEST_SIZE - last_offset, GUEST_START_HPA + last_offset, PTE_V);
  return guest_pt;
}

extern uint64_t _strampoline;

void initialize_gpm(pagetable_t pt) {
  mappages(pt, TRAMPOLINE, PGSIZE, _strampoline, PTE_R | PTE_X);
  mappages(pt, TRAMPOLINE - PGSIZE, PGSIZE, page_alloc(1), PTE_R | PTE_X);
  mappages(pt, UART0, PGSIZE, UART0, PTE_R | PTE_W);
}