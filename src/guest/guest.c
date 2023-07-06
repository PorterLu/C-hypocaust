#include "types.h"
#include "elf.h"
#include "vm.h"
#include "page.h"

#define GUEST_START_HPA 0x88000000
#define GUEST_START_GVA 0x80000000

pagetable_t new_guest_kernel() {
  uint16_t ph_count = elf->e_phnum;
  Elf64_Phdr *ph = NULL;
  pagetable_t guest_pt = page_alloc(1);
  printf("%lx", guest_pt);
  for(int i = 0; i < 4096; i++) {
    printf("%c ", *((uint8_t*)guest_pt + i));
  }
  int perm = 0;
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
      printf("%lx %lx\n", inner_offset + GUEST_START_HPA, p_filesz);
      if(ph->p_flags & PF_R != 0) {
        perm |= PTE_R;
      }
      if(ph->p_flags & PF_W != 0) {
        perm |= PTE_W;
      }
      if(ph->p_flags & PF_X != 0) {
        perm |= PTE_X;
      }
      mappages(guest_pt, p_vaddr, PGROUNDUP(p_filesz),  PGROUNDDOWN(inner_offset + GUEST_START_HPA), perm | PTE_V);
      perm = 0;
    }
  }
  return guest_pt;
}