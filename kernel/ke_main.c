
#include "ke_main.h"
#include "diag/print.h"
#include "interrupts/interrupts.h"
#include "memory/memory.h"
#include "memory/paging.h"
#include "memory/segment.h"
#include "params.h"
#include "sys/syscall.h"

/* The main entry point of the kernel.
 *
 */
int KAPI KeMain(KernelParameters const *params) {
    /* Initialize everything */

    InitializeDiagnostics();
    Print("kernel init\n");

    InitializeInterrupts();

    InitializeSegments();
    InitializeMemory(params->memory_map);
    InitializePaging();
    InitializeSyscalls();

    void *a = AllocatePhysicalMemory(10, MEM_ALIGN);
    void *b = AllocatePhysicalMemory(10, MEM_ALIGN);
    void *c = AllocatePhysicalMemory(100, 0x100);

    PrintMemoryBlocks();

    DeallocatePhysicalMemory(b);

    PrintMemoryBlocks();
    DeallocatePhysicalMemory(c);

    PrintMemoryBlocks();
    DeallocatePhysicalMemory(a);
    PrintMemoryBlocks();

    const char *s = (const char *)KERNEL_VBASE;
    Print("vbase %s \n", s);

    while (1)
        ;

    return 0;
}
