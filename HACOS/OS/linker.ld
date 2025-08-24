ENTRY(efi_main)

SECTIONS
{
  . = 0;
  .text : { *(.text*) }
  .data : { *(.data*) }
  .rdata : { *(.rdata*) }
  .bss  : { *(.bss*)  }
}
