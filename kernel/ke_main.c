
#include "ke_main.h"

#include "diag/print.h"

#include "ints/interrupts.h"

#include "memory/memory.h"
#include "memory/paging.h"
#include "memory/segment.h"
#include "modules/fs/filesystem.h"
#include "modules/serial/com.h"
#include "modules/vga/vga_text.h"
#include "objects/io_object.h"
#include "objects/object.h"

#include "user/exec.h"

typedef struct {
    PhysicalMemoryMap *memory_map;
    KernelImage *image;
} KernelParams;

/* The main entry point of the kernel.
 *
 */
int KAPI KeMain(KernelParams const *params) {
    VgaTextClear();

    InitializeInterrupts();
    InitializeCom(0);

    KePrint("Initializing memory & paging...\n");

    InitializeMemory(params->memory_map, params->image);
    InitializePages();
    InitializeGDT();

    /* enable the interrupts */
    EnableInterrupts();

    KePrint("Initializing filesystem...\n");
    FsInitialize();

    KePrint("Initializing objects...\n");
    InitializeObjects();
    InitializeIoObjects();

    KIoObjectHandle stdio;
    KeCreateIoObject(&stdio);

    KePrint("Kernel initialized\n");

    int ret = 0;
    if ((ret = KeUserExecuteFile("shell"))) {
        KePrint("Failed to execute shell %d \n", ret);
    }

    while (1) {
        for (int i = 0; i < 1000000000; i++)
            ;

        KePrint("next line...\n");
    }
    return 0;
}
