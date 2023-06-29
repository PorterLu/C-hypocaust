#include "hypervisor.h"
#include "sbi.h"
#include "detect.h"

extern void page_init(void);
#include "vm.h"

int main(void) {
  sbiret r = SBI_CALL(BASE_EXTENSION, PROBE_EXTENSION, HSM_EXTENSION, 0, 0);
  if(r.value == 1) 
    printf("HSM supported\n");
  else 
    panic("HSM not supported");
  

  if(detect_h_extension())
    printf("H extension supported\n");
  else 
    panic("H extension unsupported");

  page_init();

  init_hypervisor();

  enable_paging();

  printf("Welcome to hypervisor!\n");

  panic("over");

  return 0;
}
