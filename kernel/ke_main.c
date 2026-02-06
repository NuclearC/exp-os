
#include "modules/fs/filesystem.h"
#include "modules/vga/vga_io.h"
#include "ke_main.h"
#include "idt.h"

int KAPI KeMain(void) {
    FsInitialize();
    SetupInterrupts();    
   
    VgaPrintString("Kernel initialized...\n", 7);
 
    while (1) ;
    return 0;
}



