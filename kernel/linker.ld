OUTPUT_FORMAT("elf32-i386")

OUTPUT_ARCH(i386)
ENTRY(KeMain)

KERNEL_P_BASE = 0x100000;
KERNEL_V_BASE = 0xc0000000;

SECTIONS
{
    . = KERNEL_V_BASE + SIZEOF_HEADERS; /* load kernel at 1mb */
    _kernel_start = .;
     .text ALIGN(4K) : AT(ADDR(.text) - KERNEL_V_BASE + KERNEL_P_BASE) 
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
    
    _kernel_end = .;
}

