
#include "ke_main.h"
#include "diag/print.h"
#include "interrupts/interrupts.h"
#include "memory/memory.h"
#include "memory/paging.h"
#include "memory/segment.h"

#include "modules/filesystem/filesystem.h"
#include "modules/qvbe/bochs_vbe.h"
#include "pci/pci_setup.h"

#include "params.h"
#include "sys/syscall.h"
#include "tasks/task.h"
#include "user/exec.h"

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

    if (KSUCCESS != InitializeVbe()) {
        Print("failed to initialize video\n");
    }

    UserExecuteFile("mod_video");

    while (1)
        ;

    return 0;
}
