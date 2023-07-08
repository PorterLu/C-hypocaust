#include "vm.h"
#include "page.h"
#include "string.h"
#include "reg.h"
#include "platform.h"

pagetable_t pagetable;
extern uint64_t TEXT_START;
extern uint64_t TEXT_END;
extern uint64_t RODATA_START;
extern uint64_t RODATA_END;
extern uint64_t BSS_START;
extern uint64_t BSS_END;
extern uint64_t HEAP_START;
extern uint64_t HEAP_SIZE;

// Return the address of the PTE in page table pagetable
// that corresponds to virtual address va. If alloc!=0,
// create any required page-table pages.
//
// The risc-v Sv39 scheme has three levels of page-table
// pages. A page-table page contains 512 64-bit PTEs.
// A 64-bit virtual address is split into five fields:
// 39..63 -- must be all zero or all one.
// 30..38 -- 9 bits of level-2 index.
// 21..29 -- 9 bits of level-1 index.
// 12..20 -- 9 bits of level-0 index.
//  0..11 -- 12 bits of byte offset within the page.
pte_t* walk(pagetable_t pagetable, uint64_t va, int alloc) {
  if(va >= LOW_MAXVA && va < HIGH_MINVA) 
    panic("walk");

  for(int level = 2; level > 0; level--) {
    pte_t *pte = &pagetable[PX(level, va)];
    if(*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } else {
      if(!alloc || (pagetable = (pde_t*)page_alloc(1)) == 0)
        return 0;
      memset(pagetable, 0, PGSIZE);
      *pte = PA2PTE(pagetable) | PTE_V;
    }
  }
  return &pagetable[PX(0, va)];
}

// Recursively free page-table pages.
// All leaf mappings must already have been removed
void freewalk(pagetable_t pagetable) {
  // there are 2^9 = 512 PTEs in a page table.
  for(int i = 0; i < 512; i++) {
    pte_t pte = pagetable[i];
    if((pte & PTE_V) && (pte & (PTE_R|PTE_W|PTE_X)) == 0) {
      // This pte points to a lower-level page table
      uint64_t child = PTE2PA(pte);
      freewalk((pagetable_t)child);
      pagetable[i] = 0;
    } else if(pte & PTE_V) {
      panic("freewalk: leaf");
    }
    page_free((void*)pagetable);
  }
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa. va and size might not 
// be page-aligned. Returns 0 on success, -1 if walk() couldn't
// allocate a needed page-table page.
int mappages(pagetable_t pagetable, uint64_t va, uint64_t size, uint64_t pa, int perm) {
  uint64_t a, last;
  pte_t *pte;

  if(size == 0)
    panic("mappages: size");

  a = PGROUNDDOWN(va);
  last = PGROUNDDOWN(va + size - 1);
  for(;;){
    if((pte = walk(pagetable, a, 1)) == 0)
      return -1;
    if(*pte & PTE_V)
      panic("mappages: remap");
    *pte = PA2PTE(pa) | perm | PTE_V;       // leaf PTE
   // printf("map %lx ---> %lx\n", a, pa);
    if(a == last)
      break;
    a += PGSIZE;
    pa += PGSIZE;                           // continuous physic memory
  }
  return 0;
}

int map_single_page(pagetable_t pagetable, uint64_t va, uint64_t pa, int perm) {
  return mappages(pagetable, va, PGSIZE, pa, perm);
}

// Remove npages of mappings starting form va. va must be
// page-aligned. The mappings must exist.
// Optionally free the physical memory.
void uvmunmap(pagetable_t pagetable, uint64_t va, uint64_t npages, int do_free) {
  uint64_t a;
  pte_t *pte;

  if((va % PGSIZE) != 0) 
    panic("uvmunmap: not aligned");
  
  for(a = va; a < va + npages*PGSIZE; a += PGSIZE) {
    if((pte = walk(pagetable, a, 0)) == 0) 
      panic("uvmunmap: walk");
    if((*pte & PTE_V) == 0)
      panic("uvmunmap: not mapped");
    if(PTE_FLAGS(*pte) == PTE_V)
      panic("uvmunmap: not a leaf");
    if(do_free){
      uint64_t pa = PTE2PA(*pte);
      page_free((void*)pa);
    }
    *pte = 0;
  }
}

// Free user memory pages
// then free page-table pages
void uvmfreee(pagetable_t pagetable, uint64_t sz) {
  // free leaf page in a continuous virtual space
  if(sz > 0)
    uvmunmap(pagetable, 0, PGROUNDUP(sz)/PGSIZE, 1);
  // After you release all leaf pages, then you can free all level2 and level1 pages
  freewalk(pagetable);
}

int unmap_single_page(pagetable_t pagetable, uint64_t va) {
  pte_t *pte = walk(pagetable, va, 0);
  if (pte == NULL)
    return 0;
  else {
    uint64_t pa = PTE2PA(*pte);
    page_free(pa);
    *pte = 0;
  }
}

uint64_t translate(pagetable_t pagetable, uint64_t va) {
  for(int level = 2; level > 0; level--) {
    pte_t *pte = &pagetable[PX(level, va)];
    if(*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } else {
      return 0;
    }
  }
  return PTE2PA(pagetable[PX(0, va)]);
}

extern uint64_t STRAMPOLINE;

void enable_paging() {
  pagetable = page_alloc(1);
  mappages(pagetable, TEXT_START, TEXT_END - TEXT_START, TEXT_START, PTE_R | PTE_W | PTE_X);
  mappages(pagetable, RODATA_START, RODATA_END - RODATA_START, RODATA_START, PTE_R);
  mappages(pagetable, BSS_START, BSS_END - BSS_START, BSS_START, PTE_R | PTE_W);  
  mappages(pagetable, UART0, PGSIZE, UART0, PTE_R | PTE_W);
  mappages(pagetable, BSS_END, HEAP_START + HEAP_SIZE - BSS_END, BSS_END, PTE_R | PTE_W);
  mappages(pagetable, TRAMPOLINE - PGSIZE, PGSIZE, page_alloc(1), PTE_R | PTE_W | PTE_X);
  mappages(pagetable, TRAMPOLINE, PGSIZE, STRAMPOLINE, PTE_R | PTE_W | PTE_X);
  write_csr("satp", ((uint64_t)8 << 60) | (((ppn_t)pagetable >> 12) & 0xfffffffffff));
  asm volatile("sfence.vma");
}