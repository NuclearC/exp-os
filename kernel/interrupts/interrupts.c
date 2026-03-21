
#include "interrupts.h"
#include "memory/memory.h"
#include "memory/segment.h"

#include "diag/print.h"

extern void _idt_setup(void *idt);
extern void _idt_enable(void);
extern void _pic_setup(void);
extern int _pic_getmask(void);
extern void _pic_setmask(int msk);

extern void _isr_handle_de(void);
extern void _isr_handle_pf(void);
extern void _isr_handle_df(void);
extern void _isr_handle_gpf(void);

static InterruptDescriptor64 isr[MAX_INTERRUPTS];
static InterruptDescriptorTable idt;

void KAPI KeHandleGeneralProtectionFault(void *e) {
    Print("general protection fault %x \n", e);
    while (1)
        ;
}

void KAPI KeHandleDoubleFault(void *e) {
    Print("double fault %x \n", e);
    while (1)
        ;
}
void KAPI KeHandlePageFault(PageFaultException *e) {
    Print("Page fault %x %d \n", e->address, e->frame.error_code);

    if (e->frame.error_code & 1)
        Print("page protection error \n");
    else
        Print("page not present \n");

    if (e->frame.error_code & 2)
        Print("write access \n");
    else
        Print("read access \n");

    if (e->frame.error_code & 8)
        Print("reserved write \n");
    else
        Print("no reserved write \n");

    if (e->frame.error_code & 16)
        Print("instruction fetch \n");
    else
        Print("no instruction \n");

    while (1)
        ;
}

static void SetupInterruptTable(void) {
    ZeroMemory(isr, sizeof(isr));

    SetupInterruptDescriptor(&idt, ISR_DE, (uintptr_t)_isr_handle_de,
                             SEG_KERNEL_CS, 0, GATE_TYPE_INT, INT_DPL_0);

    SetupInterruptDescriptor(&idt, ISR_PF, (uintptr_t)_isr_handle_pf,
                             SEG_KERNEL_CS, 0, GATE_TYPE_INT, INT_DPL_0);
    SetupInterruptDescriptor(&idt, ISR_DF, (uintptr_t)_isr_handle_df,
                             SEG_KERNEL_CS, 0, GATE_TYPE_INT, INT_DPL_0);
    SetupInterruptDescriptor(&idt, ISR_GP, (uintptr_t)_isr_handle_gpf,
                             SEG_KERNEL_CS, 0, GATE_TYPE_INT, INT_DPL_0);
}

void KPRIV InitializeInterrupts(void) {

    idt.descriptors = isr;
    idt.sz = sizeof(isr) - 1;
    /* we need to setup the IDT */
    SetupInterruptTable();

    _idt_setup(&idt);
    _pic_setup();

    /* enable keyboard interrupt */
    _pic_setmask(0xffff);

    _idt_enable();
}

void KPRIV SetupInterruptDescriptor(InterruptDescriptorTable *table,
                                    uint16_t index, uintptr_t routine,
                                    uint16_t selector, uint8_t ist,
                                    uint8_t gate_type, uint8_t dpl) {
    InterruptDescriptor64 *id = &table->descriptors[index & 0xff];
    id->offset_1 = routine & 0xffff;
    id->offset_2 = (routine >> 16) & 0xffff;
    id->offset_3 = (routine >> 32) & 0xffffffff;
    id->ist = ist;
    id->selector = selector;
    id->type_attr = 0x80 | (dpl & INT_DPL_MASK) | gate_type;
    id->zero = 0;
}
