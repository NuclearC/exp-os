
#ifndef NC_KE_MEMORY_H_
#define NC_KE_MEMORY_H_

#include "ke_main.h"
#include "params.h"
#include "typedefs.h"

#define MEM_ALIGN sizeof(uintptr_t)

#define ALIGN(x, k) ((x % k) == 0 ? (x) : ((x / k + 1) * k))
#define MAX(x, y) ((x > y) ? x : y)

void KPRIV MemoryCopy(void *restrict dest, const void *restrict src,
                      size_t nbytes);

int KPRIV MemoryCompare(const void *source1, const void *source2,
                        size_t nbytes);

void KPRIV SetMemory(void *dest, size_t nbytes, uint64_t set);
void KPRIV ZeroMemory(void *dest, size_t nbytes);
void KPRIV InitializeMemory(PhysicalMemoryMap const *memory_map);
void KPRIV PrintMemoryBlocks(void);

#define MM_ERR_ALLOC_FAILED (void *)(~0)
void *KPRIV AllocatePhysicalMemory(size_t length, size_t align);

int KPRIV TryReallocatePhysicalMemory(const void *old_memory,
                                      size_t new_length);
#define MM_ERR_DEALLOC_INVLD 1
int KPRIV DeallocatePhysicalMemory(const void *addr);

#endif
