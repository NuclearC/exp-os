
#include "interrupts.h"

#include "idt_st.h"
#include "isr.h"
#include "pic_st.h"

#include "memory/memory.h"
#include "memory/segment.h"
#include "modules/keyboard/kb.h"
#include "modules/vga/vga_text.h"
#include "user/syscall.h"

#include "diag/error.h"
#include "diag/print.h"

static InterruptDescriptor32 table[256];
static InterruptDescriptorTable idt;

int KAPI _IsrZeroDivide(void) {
    VgaTextWrite("Zero Divison error\n", 7);
    while (1)
        ;
    return 0;
}
int KAPI _IsrPFault(PageFaultError *pf) {
    KeShowHardError(0, "Page fault", "kernel", &pf->regs);
    KePrint("error code: %x \n", pf->error_code);
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

void KPRIV SetGate(int index, void *ptr, int type, int selector) {

    index = index & 0xff;

    table[index].offset_1 = (int)ptr & 0xffff;
    table[index].offset_2 = ((int)ptr >> 16) & 0xffff;
    table[index].type_attributes = type;
    table[index].zero = 0;
    table[index].selector = selector;
}

void KAPI KeSetupIRQ(int index, void *ptr) {
    if (index <= 16)
        SetGate(ISR_PIC_BASE + index, ptr, GATE_ISR, KERNEL_CODE_SEGMENT);
}
void KAPI KeSetupUserISR(int index, void *ptr) {
    SetGate(index, ptr, GATE_USERMODE_ISR, KERNEL_CODE_SEGMENT);
}

void KPRIV InitializeInterrupts(void) {
    KeMemoryZero(table, sizeof(table));

    SetGate(ISR_DE, &_isr_zero_divide, GATE_ISR, KERNEL_CODE_SEGMENT);
    SetGate(ISR_GP, &_isr_gp, GATE_ISR, KERNEL_CODE_SEGMENT);
    SetGate(ISR_DF, &_isr_df, GATE_ISR, KERNEL_CODE_SEGMENT);
    SetGate(ISR_PF, &_isr_pf, GATE_ISR, KERNEL_CODE_SEGMENT);

    InitializeKeyboard();
    InitializeSyscalls();

    idt.descriptors = table;
    idt.sz = sizeof(table);

    _idt_setup(&idt);
    _pic_setup();
    /* enable keyboard interrupt */
    _pic_setmask(0xffff & (~2));
}

void KPRIV EnableInterrupts() {
    /* enable interrupts finally */
    _idt_enable();
}
