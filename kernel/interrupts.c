
#include "interrupts.h"

#include "idt_st.h"
#include "isr.h"
#include "pic_st.h"

#include "modules/vga/vga_io.h"
int KAPI _IsrZeroDivide(void) {
    VgaPrintString("Zero Divison error\n", 7);
    while (1);
    return 0;
}
int KAPI _IsrKeyboard(int scancode) {

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

    *(int*)(0xc0800000) = 1; 

    return 0;
}

int KAPI _IsrPFault(void) { 
    VgaPrintString("Page fault\n", 0x4);
    while (1);
    return 0;
}

int KAPI _IsrGPFault(void) {
    VgaPrintString("General Protection fault\n", 0x4);
    while (1);
    return 0;
}

int KAPI _IsrDFault(void) {
    VgaPrintString("Double fault\n", 0x4);
    while (1);
    return 0;
}

void KPRIV SetGate(InterruptDescriptor32* table, int index,
     void* ptr, int type, int selector) {

    index = index & 0xff;     

    table[index].offset_1 = (int)ptr & 0xffff;
    table[index].offset_2 = ((int)ptr >> 16) & 0xffff; 
    table[index].type_attributes = type;
    table[index].zero = 0;
    table[index].selector = selector;
}

int KPRIV InitializeInterrupts(void) {
    InterruptDescriptor32* idt = (InterruptDescriptor32*) _idt_addr();

    SetGate(idt, ISR_DE, &_isr_zero_divide, GATE_ISR, CODE_SEG);
    SetGate(idt, ISR_GP, &_isr_gp, GATE_ISR, CODE_SEG);
    SetGate(idt, ISR_DF, &_isr_df, GATE_ISR, CODE_SEG);
    SetGate(idt, ISR_PF, &_isr_pf, GATE_ISR, CODE_SEG);

    SetGate(idt, 0x21, &_isr_keyboard, GATE_ISR, CODE_SEG);
 
    _idt_setup();
    _pic_setup();
    /* enable keyboard interrupt */
    _pic_setmask(0xffff & (~2));

    /* enable interrupts finally */
    _idt_enable();
}

