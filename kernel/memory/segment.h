
#ifndef NC_MEMORY_SEGMENT_H_
#define NC_MEMORY_SEGMENT_H_

#include "ke_main.h"
#include "typedefs.h"

#define KERNEL_CODE_SEGMENT 0x8
#define KERNEL_DATA_SEGMENT 0x10
#define USER_CODE_SEGMENT 0x18
#define USER_DATA_SEGMENT 0x20

typedef struct KPACK {
    uint16_t size;
    void *offset;
} GlobalDescriptorTable;

typedef struct KPACK {
    uint16_t limit_low : 16;
    uint32_t base_low : 24;

    uint8_t accessed : 1;
    uint8_t read_write : 1;
    uint8_t conforming_expand_down : 1;
    uint8_t code : 1;
    uint8_t code_data_segment : 1;
    uint8_t privilege_level : 2;
    uint8_t present : 1;
    uint8_t limit_high : 4;

    uint8_t available : 1;
    uint8_t long_mode : 1;
    uint8_t big : 1;
    uint8_t gran : 1;

    uint8_t base_high : 8;
} SegmentDescriptor;

typedef struct KPACK {
    uint32_t prev_tss; // The previous TSS - with hardware task switching these
                       // form a kind of backward linked list.
    uint32_t esp0; // The stack pointer to load when changing to kernel mode.
    uint32_t ss0;  // The stack segment to load when changing to kernel mode.
    // Everything below here is unused.
    uint32_t esp1; // esp and ss 1 and 2 would be used when switching to rings 1
                   // or 2.
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;
    uint32_t es;
    uint32_t cs;
    uint32_t ss;
    uint32_t ds;
    uint32_t fs;
    uint32_t gs;
    uint32_t ldt;
    uint16_t trap;
    uint16_t iomap_base;
} TaskSegmentSelector;

void KPRIV InitializeGDT(void);

#endif
