
OUTPUT_FORMAT(binary)
OUTPUT_ARCH(i386:x86-64)
ENTRY(LdrMain)

LDR_BASE = 0x10000;

SECTIONS
{
    . = LDR_BASE; /* load at the specified address */

    .text : 
    {
        *(.text)
        *(.text.*)
    }

    .rodata : 
    {
        *(.rodata)
    }

    .data : 
    {
        *(.data)
    }
}

