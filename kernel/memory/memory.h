
#ifndef NC_KE_MEMORY_H_
#define NC_KE_MEMORY_H_

#include "typedefs.h"
#include "ke_main.h"
typedef struct {
    void* addr;
    size_t length;
} MemoryBlock;

int KAPI KeMemoryCopy(void* restrict dest, const void* restrict src, size_t nbytes); 
int KAPI KeMemoryCompare(const void* source1, const void* source2, size_t nbytes);
void KAPI KeMemorySet(const void* source, int set, size_t nbytes);
void KAPI KeMemoryZero(const void* source, size_t nbytes);

void* KAPI KeVirtualToPhysical(void* virt);

#endif 

