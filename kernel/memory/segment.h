

#ifndef NC_KE_MEMORY_SEGMENT_H_
#define NC_KE_MEMORY_SEGMENT_H_

#include "ke_main.h"
#include "typedefs.h"

#define SEG_NUM_DESCRIPTORS 7

#define SEG_DPL_0 0b00
#define SEG_DPL_1 0b01
#define SEG_DPL_2 0b10
#define SEG_DPL_3 0b11

#define SEG_KERNEL_CS 0x08
#define SEG_KERNEL_DS 0x10
#define SEG_USER_CS 0x18
#define SEG_USER_DS 0x20
#define SEG_TSS 0x28

#define SEG_SYS_LDT 0x2
#define SEG_SYS_TSS_AVL 0x9
#define SEG_SYS_TSS_BSY 0xb

typedef struct KPACK {
    uint32_t _reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t _reserved1;
    uint64_t ist[7];
    uint64_t _reserved2;
    uint16_t _reserved3;
    uint16_t iomap_base;
} TaskStateSegment64;

typedef union {
    struct KPACK {
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
    };
    struct KPACK {
        uint16_t limit_low : 16;
        uint32_t base_low : 24;

        uint8_t type : 4;
        uint8_t code_data_segment : 1;
        uint8_t privilege_level : 2;
        uint8_t present : 1;
        uint8_t limit_high : 4;

        uint8_t available : 1;
        uint8_t long_mode : 1;
        uint8_t big : 1;
        uint8_t gran : 1;

        uint8_t base_high : 8;
    } sys;
    uint64_t data;
} SegmentDescriptor64;

typedef struct KPACK {
    uint16_t size;
    SegmentDescriptor64 *descriptors;
} GlobalDescriptorTable;

void KAPI InitializeSegments(void);

#endif
