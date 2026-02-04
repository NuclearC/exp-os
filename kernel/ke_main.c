
#include "modules/fs/filesystem.h"
#include "util/v_io.h"

int KeMain(void) {
    
    for (int i = 0; i < 16; i++)
       _v_print("Hello World", i, 16, ((15-i)<<4) | i);
    FsInitialize();
    int index = FsFindFile("boot");
    char buf[10] = "Hello x \0";
    buf[6] = index + 0x30;
    _v_print(buf, 1, 0, 0x0f);

    while (1) ;
    return 0;
}



