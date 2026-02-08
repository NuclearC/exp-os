OUTPUT_FORMAT("elf32-i386")

OUTPUT_ARCH(i386)
ENTRY(KeMain)

SECTIONS
{
    . = 0xc0100000; /* load kernel at 1mb */

     .text : AT(ADDR(.text) - 0xC0000000)
    {
        *(.text)
    }
    
    .rodata : AT(ADDR(.rodata) - 0xC0000000)
    {
        *(.rodata)
    }
    
    .data : AT(ADDR(.data) - 0xC0000000)
    {
        *(.data)
    }
    
    .bss : AT(ADDR(.bss) - 0xC0000000)
    {
        *(.bss)
    }
}

