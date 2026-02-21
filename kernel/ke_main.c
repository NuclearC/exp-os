
#include "modules/fs/filesystem.h"
#include "modules/vga/vga_text.h"

#include "memory/paging.h"

#include "user/elf_loader.h"
#include "user/exec.h"

#include "ke_main.h"
#include "interrupts.h"
#include "print.h"



typedef struct {
    struct {
        uint32_t count;
        PhysicalMemoryMapEntry entries[32];
    } memory_map;
} KernelParams;

/* The main entry point of the kernel.
 *
 */
int KAPI KeMain(KernelParams const* params) {
    InitializeInterrupts();
    VgaTextClear();
    
    KePrint("Initializing paging...\n");
    InitializePages(params->memory_map.entries, params->memory_map.count);

    KePrint("Initializing filesystem...\n");
    FsInitialize();
  
    for (uint32_t i = 0; i < params->memory_map.count; i++) {
        KePrint("memory region %x:%x %x %x %d \n", params->memory_map.entries[i].base_high, params->memory_map.entries[i].base_low, 
                params->memory_map.entries[i].length_high, params->memory_map.entries[i].length_low, params->memory_map.entries[i].type);
    }

    KePrint("Kernel initialized\n");   

    int ret = 0;
    if ((ret = KeUserExecuteFile("shell"))) {
        KePrint("Failed to execute shell %d \n", ret);
    }

    while (1) {
        for (int i = 0; i < 1000000000; i++) ;
         
        KePrint("next line...\n");
    }
    return 0;
}


