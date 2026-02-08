
#include "modules/fs/filesystem.h"
#include "modules/vga/vga_io.h"
#include "ke_main.h"
#include "interrupts.h"
extern int _io_read_bytes(void * ptr, int sector, int offset, int len);

char nib_to_hex(uint8_t);
void to_hex(uint8_t, char*);
void print_buf(uint8_t*, int);
int KAPI KeMain(void) {
    InitializeInterrupts();    
    FsInitialize();
    VgaClearString();
    VgaPrintString("Kernel initialized...\n", 7);

    uint8_t buf[512];
    _io_read_bytes(buf, 1, 4, 4); 
    print_buf(buf, 10);
    _io_read_bytes(buf, 1, 20, 10);
    print_buf(buf, 10);

    while (1) {
        for (int i = 0; i < 1000000000; i++) ;
        
        VgaPrintString("next line\n", 7); 

    }
    return 0;
}

void print_buf(uint8_t* buf, int len) {
    char hx[4];
    hx[2] = ' ';
    hx[3] = 0;
    for (int i = 0; i < len; i++) {
        to_hex(buf[i], hx);
        VgaPrintString(hx, 3);
    }
    VgaPrintString("\n", 0);
}

char nib_to_hex(uint8_t nib) {
    if (nib < 10) return '0' + nib;
    return 'A' + (nib - 10);
}

void to_hex(uint8_t b, char* hx) {
    hx[0] = nib_to_hex((b & 0xf0) >> 4);  
    hx[1] = nib_to_hex(b & 0x0f); 
}




