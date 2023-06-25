#include "hypervisor.h"
#include "sbi.h"

int main(void) {
  sbiret r = SBI_CALL(0x10, 0x3, 0x48534D, 0, 0);
  if(r.value == 1) {
    printf("HSM supported\n");
  } else {
    panic("HSM not supported");
  }
  
  panic("over");

  return 0;
}
