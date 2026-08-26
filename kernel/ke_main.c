
#include "ke_main.h"
#include "diag/print.h"
#include "exec/elf/elf_loader.h"
#include "interrupts/interrupts.h"
#include "memory/memory.h"
#include "memory/paging.h"
#include "memory/segment.h"

#include "modules/filesystem/filesystem.h"
#include "modules/vga/vga.h"
#include "pci/pci_setup.h"

#include "exec/exec.h"
#include "params.h"
#include "sys/syscall.h"
#include "tasks/task.h"

/* The main entry point of the kernel.
 *
 */
int KEXP KeMain(KernelParameters const *params) {
    /* Initialize everything */

    InitializeDiagnostics();
    Print("kernel init\n");

    /* initialize interrupts first */
    InitializeInterrupts();

    /* initialize memory */
    InitializeSegments();
    InitializeMemory(params->memory_map);
    InitializePaging();
    InitializeSyscalls();
    InitializeTasks();

    EnumeratePciDevices();

    int num_files = LoadFiles();
    Print("loaded %d files from disk\n", num_files);

    for (int i = 0; i < 1e9; i++)
        ;

    VgaInitialize();
    VgaTextWriteString("Kernel initialized...\n", 22, 0x0f);

    while (1) {
    }

    return 0;
}
