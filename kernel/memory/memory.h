
#ifndef NC_KE_MEMORY_H_
#define NC_KE_MEMORY_H_

#include "ke_main.h"
#include "params.h"
#include "typedefs.h"

#define MEM_ALIGN sizeof(uintptr_t)

#define ALIGN(x, k) ((x % k) == 0 ? (x) : ((x / k + 1) * k))
#define MAX(x, y) ((x > y) ? x : y)

/* exported functions */
void KEXP KeMemoryCopy(void *restrict dest, const void *restrict src,
                       size_t nbytes);

int KEXP KeMemoryCompare(const void *source1, const void *source2,
                         size_t nbytes);

void KEXP KeSetMemory(void *dest, size_t nbytes, uint64_t set);
void KEXP KeZeroMemory(void *dest, size_t nbytes);

/* private functions */
void KAPI InitializeMemory(PhysicalMemoryMap const *memory_map);
void KAPI PrintMemoryBlocks(void);

#define MM_ERR_ALLOC_FAILED (void *)(~0)
void *KAPI AllocatePhysicalMemory(size_t length, size_t align);

int KAPI TryReallocatePhysicalMemory(const void *old_memory, size_t new_length);
#define MM_ERR_DEALLOC_INVLD 1
int KAPI DeallocatePhysicalMemory(const void *addr);

#endif
