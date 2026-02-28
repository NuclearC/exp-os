
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
    uintptr_t begin;
    uintptr_t end;
} PhysicalMemoryBlock;

typedef struct {
    uintptr_t begin;
    uintptr_t end;
    uintptr_t stack_begin;
    uintptr_t stack_end;
} KernelImage;

#define MEM_ALIGN sizeof(uintptr_t)

#define ALIGN(x, k) ((x % k) == 0 ? (x) : ((x / k + 1) * k))
#define MAX(x, y) ((x > y) ? x : y)

void KPRIV InitializeMemory(PhysicalMemoryMap const *memory_map,
                            KernelImage const *kernel_image);

int KAPI KeMemoryCopy(void *restrict dest, const void *restrict src,
                      size_t nbytes);
int KAPI KeMemoryCompare(const void *source1, const void *source2,
                         size_t nbytes);
void KAPI KeMemorySet(void *dest, int set, size_t nbytes);
void KAPI KeMemoryZero(void *dest, size_t nbytes);

void KAPI KePrintBlocks(size_t maxcnt);
void *KAPI KeAllocatePhysicalMemory(size_t length, size_t align);
int KAPI KeTryReallocatePhysicalMemory(const void *oldmemory,
                                       size_t new_length);
void *KAPI KeAllocateContiguousMemory(size_t *length, size_t align,
                                      size_t minlength);

void KAPI KeDeallocatePhysicalMemory(const void *addr);

#endif
