OUTPUT_FORMAT("elf64-x86-64")

OUTPUT_ARCH(i386:x86-64)
ENTRY(KeMain)

KERNEL_P_BASE = 0x100000;
MAX_PAGE_SZ = 4K;

SECTIONS
{
    . = KERNEL_P_BASE + SIZEOF_HEADERS; /* load kernel at 1mb */
    _kernel_start = .;
    .text ALIGN(MAX_PAGE_SZ) :
    {
        *(.text)
    }

    .rodata ALIGN(MAX_PAGE_SZ) : 
    {
        *(.rodata)
    }

    .data ALIGN(MAX_PAGE_SZ) : 
    {
        *(.data)
    }

    _kernel_end = .;
}

