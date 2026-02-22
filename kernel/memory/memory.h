
#ifndef NC_KE_MEMORY_H_
#define NC_KE_MEMORY_H_

#include "ke_main.h"
#include "typedefs.h"

typedef struct {
    uint32_t base_low;
    uint32_t base_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
    uint32_t acpi3_attr;
} PhysicalMemoryMapEntry;

typedef struct {
    uint32_t count;
    PhysicalMemoryMapEntry *entries;
} PhysicalMemoryMap;

typedef struct {
    size_t length;
    void *base;
} PhysicalMemoryBlock;

typedef struct {
    void *begin;
    void *end;
    void *stack_begin;
    void *stack_end;
} KernelImage;

void KPRIV InitializeMemory(PhysicalMemoryMap const *memory_map,
                            KernelImage const *kernel_image);

int KAPI KeMemoryCopy(void *restrict dest, const void *restrict src,
                      size_t nbytes);
int KAPI KeMemoryCompare(const void *source1, const void *source2,
                         size_t nbytes);
void KAPI KeMemorySet(const void *source, int set, size_t nbytes);
void KAPI KeMemoryZero(const void *source, size_t nbytes);

void *KAPI KeVirtualToPhysical(void *virt);

void KAPI KePrintBlocks(int maxcnt);
void *KAPI KeAllocatePhysicalMemory(size_t length, size_t align);

#endif
