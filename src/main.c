#include "hypervisor.h"
#include "sbi.h"
#include "malloc.h"
#include "print.h"
#include "elf.h"
#include "vm.h"
#include "page.h"
#include "guest.h"

extern uint64_t BSS_START;
extern uint64_t BSS_END;
extern uint8_t GUEST_START;
extern uint8_t GUEST_END;

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

  for(int i = 0; i < (BSS_END - BSS_START)/(sizeof(uint64_t)); i++) {
    *((uint64_t*)(BSS_START + i)) = 0;
  }

  init_hypervisor();

  enable_paging();

  heap_init();
  //heap_test();

  char* logo[8] = 
  {" _____ _",                          
"/  __ \\ |",                         
"| /  \\/ |__  _   _ _ __   ___ _ __" ,
"| |   | '_ \\| | | | '_ \\ / _ \\ '__|",
"| \\__/\\ | | | |_| | |_) |  __/ |",   
" \\____/_| |_|\\__, | .__/ \\___|_|",   
"              __/ | |",              
"             |___/|_|"              };

  for(int i = 0; i < 8; i++){
    printf("%s\n",logo[i]);
  }

  init_elf((void*)&GUEST_START);

  printf("Welcome to hypervisor!\n");

  hentry();
  
  panic("over");

  return 0;
}
