
#include "idt.h"

#include "util/idt_proc.h"

int KPRIV SetupInterrupts(void) {
    InterruptDescriptor32* idt_table = (InterruptDescriptor32*) _idt_addr();


    _idt_setup();
}

