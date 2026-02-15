
#include "kb.h"
#include "kernel/interrupts.h"
#include "kernel/print.h"

extern int _isr_keyboard();

int KAPI _IsrKeyboardHandler(int scancode) {
    KePrint("keyboard interrupt %d \n", scancode);      


    return 0;
}

int KAPI InitializeKeyboard(void) {
    KeSetupIRQ(0x1, _isr_keyboard);
    return 0;
}
