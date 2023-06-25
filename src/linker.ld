OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY
{
  ram (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 512M
}

BASE_ADDRESS = 0x80200000;

SECTIONS
{
  . = BASE_ADDRESS;
  skernel = .;

  stext = .;
  .text : {
    . = ALIGN(4K);
    *(.text .text.*)
  } > ram

  . = ALIGN(4K);
  etext = .;
  srodata = .;
  .rodata : {
    *(.rodata .rodata.*)
    *(.srodata .srodata.*)
  } > ram

  . = ALIGN(4K);
  erodata = .;
  sdata = .;
  .data : {
    *(.data .data.*)
    *(.sdata .sdata.*)
  } > ram

  . = ALIGN(4K);
  sbss = .;
  .bss : {
    *(.bss .bss.*)
    *(.sbss .sbss.*)
  } > ram

  . = ALIGN(4K);
  ebss = .;
  ekernel = .;

  . = 0x90000000;
  .dtb : {
    *(.dtb)
  } > ram

  . = 0x90200000;
  .initrd : {
    *(.initrd)
  } > ram

  /DISCARD/ : {
    *(.eh_frame)
  } > ram

  memory_end = .;
  heap_start = .;
  heap_size = 2*ORIGIN(ram) + LENGTH(ram) - heap_start - BASE_ADDRESS;
}
