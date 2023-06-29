#include "platform.h"

  .equ STACK_SIZE, 4096
  .global _start

  .text
_start:
  la    sp, stacks  + STACK_SIZE
  j main

.align 3
stacks:
  .skip STACK_SIZE 
  .end
