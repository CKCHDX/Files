/* Not used when building with gnu-efi's provided linker script.
   Kept here for completeness if you later switch to a fully custom link.
*/
ENTRY(EfiMain)

SECTIONS
{
  . = 0;
  .text : { *(.text*) }
  .data : { *(.data*) }
  .rodata : { *(.rodata*) }
  .bss : { *(.bss*) *(COMMON) }

  /* PE/COFF requires .reloc even if empty */
  .reloc : { *(.reloc) } 
}
