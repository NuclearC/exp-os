
#include "segment.h"

#include "memory/memory.h"

extern void _mem_seg_load(void *);
extern void _mem_flush_tss(void);
static GlobalDescriptorTable gdt;
static SegmentDescriptor descriptors[6];
static TaskSegmentSelector tss;

static void InitializeDescriptor(SegmentDescriptor *desc, uintptr_t limit,
                                 uintptr_t base) {
    desc->present = 1;
    desc->accessed = 0;
    desc->limit_low = limit;
    desc->limit_high = (limit >> 16) & 0xf;

    desc->base_low = base;
    desc->base_high = (base >> 24) & 0xff;
}

static void InitializeTSS(SegmentDescriptor *desc) {
    KeMemoryZero(&tss, sizeof(tss));

    InitializeDescriptor(desc, sizeof(tss), (uint32_t)&tss);

    desc->code_data_segment = 0;
    desc->code = 1;
    desc->accessed = 1;
    desc->read_write = 0;
    desc->conforming_expand_down = 0;
    desc->privilege_level = 0;
    desc->available = 0;
    desc->long_mode = 0;
    desc->big = 0;
    desc->gran = 0;

    tss.ss0 = KERNEL_DATA_SEGMENT;
    tss.esp0 = KERNEL_STACK_BASE + 0x10000; /* todo: xd */
}

void KPRIV InitializeGDT(void) {

    InitializeDescriptor(&descriptors[0], 0, 0);
    for (int i = 1; i < 6; i++) {
        InitializeDescriptor(&descriptors[i], 0xfffff, 0);
    }
    /* ring 0 descriptors */
    descriptors[1].read_write = 1;
    descriptors[1].conforming_expand_down = 0;
    descriptors[1].code = 1;
    descriptors[1].code_data_segment = 1;
    descriptors[1].privilege_level = 0;
    descriptors[1].long_mode = 0;
    descriptors[1].available = 0;
    descriptors[1].big = 1;
    descriptors[1].gran = 1;

    descriptors[2] = descriptors[1];
    descriptors[2].code = 0;

    /* ring 3 descriptors */
    descriptors[3] = descriptors[1];
    descriptors[3].privilege_level = 3;
    descriptors[4] = descriptors[2];
    descriptors[4].privilege_level = 3;

    gdt.size = sizeof(descriptors) - 1;
    gdt.offset = descriptors;

    InitializeTSS(&descriptors[5]);

    _mem_seg_load(&gdt);

    _mem_flush_tss();
}
