.section .rodata
.global HEAP_START
HEAP_START: .dword _heap_start

.global HEAP_SIZE
HEAP_SIZE: .dword _heap_size

.global TEXT_START
TEXT_START: .dword _stext

.global TEXT_END
TEXT_END: .dword _etext

.global RODATA_START
RODATA_START: .dword _srodata

.global RODATA_END
RODATA_END: .dword _erodata

.global DATA_START
DATA_START: .dword _sdata 

.global DATA_END
DATA_END: .dword _edata 

.global BSS_START
BSS_START: .dword _sbss

.global BSS_END
BSS_END: .dword _ebss
