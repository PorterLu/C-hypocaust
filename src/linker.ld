OUTPUT_ARCH( "riscv" )
ENTRY( _start )

BASE_ADDRESS = 0x80200000;

SECTIONS
{
  . = BASE_ADDRESS;
  skernel = .;

  _stext = .;
  .text : {
    *(.text.entry)
    . = ALIGN(4K);
    _strampoline = .;
    *(.text.trampoline)
    . = ALIGN(4K);
    *(.text .text.*)
  }

  . = ALIGN(4K);
  _etext = .;
  _srodata = .;
  .rodata : {
    *(.rodata .rodata.*)
    *(.srodata .srodata.*)
  }

  . = ALIGN(4K);
  _erodata = .;
  _sdata = .;
  .data : {
    *(.data .data.*)
    *(.sdata .sdata.*)
  }

  . = ALIGN(4K);
  _edata = .;
  _sbss = .;
  .bss : {
    *(.bss .bss.*)
    *(.sbss .sbss.*)
  }

  . = ALIGN(4K);
  _ebss = .;
  _ekernel = .;

  . = 0x90000000;
  .dtb : {
    *(.dtb)
  } 

  /DISCARD/ : {
    *(.eh_frame)
  }

  _memory_end = .;
  _heap_start = .;
  _heap_size = 0x80000000 + 512*1024*1024 - _heap_start;
}
