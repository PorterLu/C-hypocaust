.section .rodata
.global HEAP_START
HEAP_START: .dword heap_start

.global HEAP_SIZE
HEAP_SIZE: .dword heap_size

.global TEXT_START
TEXT_START: .dword stext

.global TEXT_END
TEXT_END: .dword etext

.global RODATA_START
RODATA_START: .dword srodata

.global RODATA_END
RODATA_END: .dword erodata

.global BSS_START
BSS_START: .dword sbss

.global BSS_END
BSS_END: .dword ebss
