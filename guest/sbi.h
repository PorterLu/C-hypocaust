#ifndef __SBI_H__
#define __SBI_H__

#include "types.h"

#define SHUT_DOWN 0x08
#define HSM_EXTENSION 0x48534D
#define BASE_EXTENSION 0x10

// function id
#define PROBE_EXTENSION 0x3

typedef struct sbiret {
  long error;
  long value;
} sbiret;

#define SBI_CALL(which, fid, arg0, arg1, arg2) ({       \
  register uintptr_t a0 asm("a0") = (uintptr_t)(arg0);  \
  register uintptr_t a1 asm("a1") = (uintptr_t)(arg1);  \
  register uintptr_t a2 asm("a2") = (uintptr_t)(arg2);  \
  register uintptr_t a7 asm("a7") = (uintptr_t)(which); \
  register uintptr_t a6 asm("a6") = (uintptr_t)(fid);   \
  asm volatile("ecall"                                  \
              : "+r" (a0), "+r"(a1)                     \
              : "r"(a2), "r"(a7), "r"(a6)               \
              : "memory");                              \
  sbiret ret;                                           \
  ret.error = a0;                                       \
  ret.value = a1;                                       \
  ret;                                                  \
})

#endif


