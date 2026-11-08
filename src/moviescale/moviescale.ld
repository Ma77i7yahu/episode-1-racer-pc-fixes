/* Flat code at the start of the patch's own EXE section. Buffers follow at 00ED1000. */
SECTIONS
{
  . = 0x00ED0000;
  .text : {
    *(.text) *(.text.*) *(.rdata) *(.rdata*) *(.data) *(.data.*) *(.bss) *(.bss.*) *(COMMON)
  }
  /DISCARD/ : { *(*) }
}
