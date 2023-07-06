  .section .rodata
  .global GUEST_START 
  .global GUEST_END
GUEST_START:
.incbin "guest/guest.elf"
GUEST_END: