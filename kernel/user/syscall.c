

#include "syscall.h"

#include "kernel/ints/interrupts.h"

#include "kernel/modules/vga/vga_text.h"

extern int _isr_syscall(void);

int KAPI KeSyscall(SyscallRegisters *regs) {
    int ret = 0;
    switch (regs->eax) {
    case 0x01:
        VgaTextWrite((const char *)regs->ebx, regs->edx);
        break;
    }
    return ret;
}

void KAPI InitializeSyscalls(void) { KeSetupUserISR(0x80, &_isr_syscall); }
