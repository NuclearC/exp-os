

#include "segment.h"
#include "memory.h"

extern void _gdt_load(GlobalDescriptorTable const *);
extern void _tss_load(uint16_t entry);

static SegmentDescriptor64 descriptors[SEG_NUM_DESCRIPTORS];
static GlobalDescriptorTable gdt;
static TaskStateSegment64 tss;

static void InitializeTSS(void) {
    ZeroMemory(&tss, sizeof(tss));

    tss.rsp0 = KERNEL_STACK_TOP + 0x10000;
    tss.iomap_base = 0xffff;

    uintptr_t base = (uintptr_t)&tss;
    size_t limit = sizeof(tss);

    descriptors[5].limit_low = limit & 0xffff;
    descriptors[5].limit_high = (limit >> 16) & 0xf;

    descriptors[5].base_low = base & 0xffffff;
    descriptors[5].base_high = (base >> 24) & 0xff;

    descriptors[5].sys.type = SEG_SYS_TSS_AVL;
    descriptors[5].sys.code_data_segment = 0;
    descriptors[5].sys.present = 1;

    descriptors[6].data = (base >> 32) & 0xffffffff;
}

void KAPI InitializeSegments(void) {
    ZeroMemory(descriptors, sizeof(descriptors));

    /* Kernel CS */
    descriptors[1].limit_low = 0xffff;
    descriptors[1].read_write = 1;
    descriptors[1].limit_high = 0xf;
    descriptors[1].code_data_segment = 1; /* not TSS */
    descriptors[1].code = 1;
    descriptors[1].present = 1;
    descriptors[1].privilege_level = SEG_DPL_0;

    descriptors[1].long_mode = 1;
    descriptors[1].gran = 1;

    /* Kernel DS */
    descriptors[2] = descriptors[1];
    descriptors[2].long_mode = 0;
    descriptors[2].big = 1;
    descriptors[2].code = 0;

    /* User CS */
    descriptors[3] = descriptors[1];
    descriptors[3].privilege_level = SEG_DPL_3;

    /* User DS */
    descriptors[4] = descriptors[2];
    descriptors[4].privilege_level = SEG_DPL_3;

    InitializeTSS();

    /* Setup the GDT */
    gdt.size = sizeof(descriptors) - 1;
    gdt.descriptors = descriptors;

    _gdt_load(&gdt);
    _tss_load(SEG_TSS);
}
