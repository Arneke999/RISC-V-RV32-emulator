ENTRY(_start)
SECTIONS {
  . = 0x80000000;
  .text   : { *(.text.start) *(.text*) }
  .rodata : { *(.rodata*) *(.srodata*) }
  .data   : { *(.data*) *(.sdata*) }
  .bss    : { *(.bss*) *(.sbss*) }
  _stack_top = 0x80010000;
}