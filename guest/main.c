#include "types.h"
#include "print.h"

extern uint64_t sbss;
extern uint64_t ebss;

int main(void) {
  for(int i = 0; i < (ebss - sbss)/sizeof(uint64_t); i++) {
    *((uint64_t*)(sbss) + i) = 0; 
  }
  printf("Welcome to guest\n");
  return 0;
}