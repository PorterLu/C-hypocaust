OUTPUT_ARCH( riscv )
ENTRY( _start )
BASE_ADDRESSS = 0x80000000;

SECTIONS
{
  . = BASE_ADDRESSS;
  skernel = .;

  stext = .;
  .text : {
    *(.text.entry)
    . = ALIGN(4K);
    strampoline = .;
    *(.text.trampoline);
    . = ALIGN(4K);
    *(.text .text.*);
  }

  . = ALIGN(4K);
  etext = .;
  srodata = .;
  .rodata : {
    *(.rodata .rodata.*)
    *(.srodata .srodata.*)
  }

  . = ALIGN(4K);
  erodata = .;
  sdata = .;
  .data : {
    *(.data .data.*)
    *(.sdata .sdata.*)
  }

  . = ALIGN(4K);
  edata = .;
  sbss = .;
  .bss : {
    *(.bss .bss.*)
    *(.sbss .sbss.*)
  }

  . = ALIGN(4K);
  ebss = .;

  ekernel = .;

  /DISCARD/ : {
    *(.eh_frame)
  }
}