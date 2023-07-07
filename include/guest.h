#ifndef __GUEST_H__
#define __GUEST_H__

#include "vm.h"
#include "types.h"

typedef struct VMB {
  uint64_t vmid;
  pagetable_t pt;
} VMB;

pagetable_t new_guest_kernel();
void hentry();
void initialize_gpm(pagetable_t pt);

#endif