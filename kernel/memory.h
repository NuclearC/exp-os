
#ifndef NC_KE_MEMORY_H_
#define NC_KE_MEMORY_H_

#include "typedefs.h"
#include "ke_main.h"

typedef struct KPACK {
    uint8_t present : 1;
    uint8_t read_write : 1;
    uint8_t user_access : 1;
    uint8_t pwt : 1;
    uint8_t disable_cache : 1;
    uint8_t accessed : 1;
    uint8_t available1 : 1;
    uint8_t page_size : 1;
    uint8_t available4 : 4;
    uint32_t addr : 20; 
} PageDirectoryEntry32;

typedef struct KPACK {
    uint8_t present : 1;
    uint8_t read_write : 1;
    uint8_t user_access : 1;
    uint8_t pwt : 1;
    uint8_t disable_cache : 1;
    uint8_t accessed : 1;
    uint8_t dirty : 1;
    uint8_t pat : 1;
    uint8_t global : 1;
    uint8_t available : 3;
    uint32_t addr : 20; 
} PageTableEntry32;

int KAPI KeMemoryCopy(void* restrict dest, const void* restrict src, size_t nbytes); 
int KAPI KeMemoryCompare(const void* source1, const void* source2, size_t nbytes);

int KPRIV KeAllocatePageDirectores(PageDirectoryEntry32* entry, size_t nentries, int flags);
int KPRIV KeAllocatePageTables(PageTableEntry32* entry, size_t nentries, int flags);

int KAPI KeAllocateMemory(size_t nbytes, void** addr);
int KAPI KeFreeMemory(void* addr);


#endif 

