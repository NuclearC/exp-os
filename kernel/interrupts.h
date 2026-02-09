
#ifndef NC_KE_INTERRUPTS_H_
#define NC_KE_INTERRUPTS_H_

#include "ke_main.h"
#include "typedefs.h"

typedef struct KPACK {
    uint16_t offset_1;
    uint16_t selector;
    uint8_t zero;
    uint8_t type_attributes;
    uint16_t offset_2;
} InterruptDescriptor32;

typedef struct KPACK {
    uint16_t sz;
    InterruptDescriptor32 * descriptors;
} InterruptDescriptorTable;

#define GATE_ISR 0x8e

#define ISR_FAULT 0x100
#define ISR_TRAP  0x200
#define ISR_INT   0x400
#define ISR_ABORT 0x800

/* list of interrupts */
#define ISR_DE (ISR_FAULT   |0x00) /* Division error */
#define ISR_DB (ISR_TRAP    |0x01) /* Debug exception */
#define ISR_NMI (ISR_INT    |0x02) 
#define ISR_BP (ISR_TRAP    |0x03) /* Breakpoint */
#define ISR_OF (ISR_TRAP    |0x04) /* Overflow */
#define ISR_BR (ISR_FAULT   |0x05) /* Bound range exceed */
#define ISR_UD (ISR_FAULT   |0x06) /* UD instruction or reserved opcode */
#define ISR_NM (ISR_FAULT   |0x07) /* Device not available (no math xd) */
#define ISR_DF (ISR_ABORT   |0x08) /* Double fault */
#define ISR_FPI (ISR_FAULT  |0x09) /* Coproc seg overrun */
#define ISR_TS (ISR_FAULT   |0x0A) /* Invalid TSS */
#define ISR_NP (ISR_FAULT   |0x0B) /* Segment not present */
#define ISR_SS (ISR_FAULT   |0x0C) /* Stack segment fault */
#define ISR_GP (ISR_FAULT   |0x0D) /* General protection */
#define ISR_PF (ISR_FAULT   |0x0E) /* Page fault */
#define ISR_MF (ISR_FAULT   |0x10) /* x87 FPU Floating-Point error (math fault) */
#define ISR_AC (ISR_FAULT   |0x11) /* Alignment check */
#define ISR_MC (ISR_ABORT   |0x12) /* Machine check */
#define ISR_XM (ISR_FAULT   |0x13) /* SIMD FP exception */
#define ISR_VE (ISR_FAULT   |0x14) /* Virt. exception */
#define ISR_CP (ISR_FAULT   |0x15) /* Control protection exception */

#define ISR_PIC_BASE 0x20

int KPRIV InitializeInterrupts(void);

int KAPI KeSetupIRQ(int index, void *ptr);

#endif

