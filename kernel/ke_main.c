
#include "modules/fs/filesystem.h"
#include "util/v_io.h"

int KeMain(void) {

    _v_print("kernel loaded", 0, 0, 0x08);
    for (int i = 0; i < 16; i++)
       _v_print("Hello World", i, 16, ((15-i)<<4) | i);
    FsInitialize();
    

    while (1) ;
    return 0;
}



