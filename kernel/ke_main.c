
#include "memory.h"
#include "modules/fs/filesystem.h"
#include "modules/serial/com.h"
#include "modules/vga/vga_text.h"

#include "memory/memory.h"
#include "memory/paging.h"

#include "user/elf_loader.h"
#include "user/exec.h"

#include "interrupts.h"
#include "ke_main.h"
#include "print.h"

typedef struct {
    PhysicalMemoryMap *memory_map;
    KernelImage *image;
} KernelParams;

/* The main entry point of the kernel.
 *
 */
int KAPI KeMain(KernelParams const *params) {
    InitializeInterrupts();
    InitializeCom(0);
    VgaTextClear();
    VgaTextWrite("Hello world\n", 0x02);

    KePrint("Initializing memory & paging...\n");

    InitializeMemory(params->memory_map, params->image);
    InitializePages();

    KePrint("Initializing filesystem...\n");
    FsInitialize();
    KePrint("Kernel initialized\n");

    void *test = KeAllocatePhysicalMemory(3000, 4096);

    KeAllocatePageTables(test, 3000, (void *)0x50000000, PAGE_READ_WRITE);

    KePrintBlocks(10);

    *(uint32_t *)(0x50000000) = 1;

    KePrint("wtf %d \n", *(uint32_t *)test);

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
