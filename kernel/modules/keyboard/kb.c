
#include "kb.h"
#include "kernel/interrupts.h"
#include "modules/vga/vga_io.h"

extern int _isr_keyboard();


int KAPI _IsrKeyboardHandler(int scancode) {
    char st[10];
    for (int i = 0; i < 10; i++) st[i] = 0;
    int idx = 8;
    while (scancode > 0) {
        st[--idx] = (scancode % 10) + 0x30;
        scancode /= 10;
    }  

    VgaPrintString("keyboard press ", 6);
    VgaPrintString(st + idx, 5);
    VgaPrintString("\n", 0);

    return 0;
}

int KAPI InitializeKeyboard(void) {
    KeSetupIRQ(0x1, _isr_keyboard);
    return 0;
}

