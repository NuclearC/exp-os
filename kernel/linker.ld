OUTPUT_FORMAT("elf32-i386")

OUTPUT_ARCH(i386)
ENTRY(KeMain)

SECTIONS
{
    . = 0xc0100000; /* load kernel at 1mb */
    _kernel_start = .;
     .text ALIGN (4K) : AT(ADDR(.text) - 0xC0000000)
    {
        *(.text)
    }

    .rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000) 
    {
        *(.rodata)
    }

    .data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000)
    {
        *(.data)
    }
    
    .bss ALIGN(4K) : AT(ADDR(.bss) - 0xC0000000)
    {
        *(COMMON)
        *(.bss)
        *(.bootstrap_stack)
    }
    _kernel_end = .;
}

