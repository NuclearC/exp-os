
#include "interrupts.h"

#include "idt_st.h"
#include "isr.h"
#include "pic_st.h"

#include "modules/keyboard/kb.h"
#include "modules/vga/vga_text.h"

#include "error.h"

InterruptDescriptor32 *idt;

int KAPI _IsrZeroDivide(void) {
    VgaTextWrite("Zero Divison error\n", 7);
    while (1)
        ;
    return 0;
}
int KAPI _IsrPFault(void *regs) {
    KeShowHardError(0, "Page fault", "kernel", regs);
    while (1)
        ;
    return 0;
}

int KAPI _IsrGPFault(void *regs) {
    KeShowHardError(0, "General protection fault", "kernel", regs);
    while (1)
        ;
    return 0;
}

int KAPI _IsrDFault(void *regs) {
    KeShowHardError(0, "Double fault", "kernel", regs);
    while (1)
        ;
    return 0;
}

void KPRIV SetGate(InterruptDescriptor32 *table, int index, void *ptr, int type,
                   int selector) {

    index = index & 0xff;

    table[index].offset_1 = (int)ptr & 0xffff;
    table[index].offset_2 = ((int)ptr >> 16) & 0xffff;
    table[index].type_attributes = type;
    table[index].zero = 0;
    table[index].selector = selector;
}

int KAPI KeSetupIRQ(int index, void *ptr) {
    if (index > 16)
        return 1;
    SetGate(idt, ISR_PIC_BASE + index, ptr, GATE_ISR, CODE_SEG);

    return 0;
}

int KPRIV InitializeInterrupts(void) {
    idt = (InterruptDescriptor32 *)_idt_addr();

    SetGate(idt, ISR_DE, &_isr_zero_divide, GATE_ISR, CODE_SEG);
    SetGate(idt, ISR_GP, &_isr_gp, GATE_ISR, CODE_SEG);
    SetGate(idt, ISR_DF, &_isr_df, GATE_ISR, CODE_SEG);
    SetGate(idt, ISR_PF, &_isr_pf, GATE_ISR, CODE_SEG);

    InitializeKeyboard();

    _idt_setup();
    _pic_setup();
    /* enable keyboard interrupt */
    _pic_setmask(0xffff & (~2));

    /* enable interrupts finally */
    _idt_enable();
    return 0;
}
