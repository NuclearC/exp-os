
#include "modules/fs/filesystem.h"
#include "modules/vga/vga_io.h"
#include "ke_main.h"
#include "interrupts.h"

int KAPI KeMain(void) {
    InitializeInterrupts();    
    FsInitialize();
    VgaPrintString("Kernel initialized...\n", 7);

    while (1) {
        for (int i = 0; i < 1000000000; i++) ;
        
        VgaPrintString("next line\n", 7); 

    }
    return 0;
}



