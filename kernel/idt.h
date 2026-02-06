
#ifndef NC_KE_IDT_H_
#define NC_KE_IDT_H_

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

int KPRIV SetupInterrupts(void);



#endif

