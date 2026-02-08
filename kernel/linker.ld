OUTPUT_FORMAT("elf32-i386")

OUTPUT_ARCH(i386)
ENTRY(KeMain)

SECTIONS
{
    . = 0x100000; /* load kernel at 1mb */

      .text : ALIGN(CONSTANT(MAXPAGESIZE)) {
_TEXT_START_ = .;
    *(.text)
_TEXT_END_ = .;
  }

  .data : ALIGN(CONSTANT(MAXPAGESIZE)) {
_DATA_START_ = .;
    *(.data)
_DATA_END_ = .;
  }

  .bss : ALIGN(CONSTANT(MAXPAGESIZE)) {
_BSS_START_ = .;
    *(.bss)
_BSS_END_ = .;
  }
}

