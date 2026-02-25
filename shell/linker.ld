OUTPUT_FORMAT("elf32-i386")

OUTPUT_ARCH(i386)
ENTRY(main)

SECTIONS
{
    . = SIZEOF_HEADERS;
    _prog_start = .;
    
     .text ALIGN(4K) :
    {
        *(.text)
    }

    .rodata ALIGN(4K) : 
    {
        *(.rodata)
    }

    .data ALIGN(4K) : 
    {
        *(.data)
    }

    
    .bss ALIGN(4K) : {
        *(.bss)
        *(COMMON)
    }
    
    _prog_end = .;
}





