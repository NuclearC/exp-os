
#include "modules/fs/filesystem.h"
#include "modules/vga/vga_text.h"
#include "ke_main.h"
#include "interrupts.h"
#include "print.h"

/* The main entry point of the kernel.
 *
 */
int KAPI KeMain(void) {
    InitializeInterrupts();    
    FsInitialize();
    VgaTextClear();
    KePrint("kernel initialized\n");	
    
    while (1) {
        for (int i = 0; i < 1000000000; i++) ;
         
        KePrint("next line...\n");
    }
    return 0;
}


