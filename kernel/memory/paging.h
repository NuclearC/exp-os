

#ifndef NC_KE_MEMORY_PAGING_H_
#define NC_KE_MEMORY_PAGING_H_

#include "ke_main.h"
#include "typedefs.h"

#define PG_TOP_ENTRIES 512

#define PG_PML4_ENTRIES 512
#define PG_PDP_ENTRIES 512
#define PG_PD_ENTRIES 512
#define PG_ENTRIES 512

#define PG_ALIGN 4096
#define PG_SIZE 4096
#define PG_PD_SIZE (PG_SIZE * PG_ENTRIES)
#define PG_PDP_SIZE (PG_PD_SIZE * PG_PD_ENTRIES)
#define PG_PML4_SIZE (PG_PDP_SIZE * PG_PTP_ENTRIES)

#define PG_ADDR 0x7FFFFFFFFF000

#define PG_ERR_INVALID_ALIGN 1
#define PG_ERR_ALLOC_FAILED 2

#define PAGE_READWRITE 0x1
#define PAGE_ACCESSALL 0x2
#define PAGE_EXECUTE 0x4
#define PAGE_WRITETHROUGH 0x8
#define PAGE_NOCACHE 0x10

typedef union {
    struct KPACK {
        uint8_t present : 1;
        uint8_t read_write : 1;
        uint8_t us : 1;  /* user/super access */
        uint8_t pwt : 1; /* page write through */
        uint8_t pcd : 1; /* page cache disable */
        uint8_t accessed : 1;
        uint8_t avl0 : 1;
        uint8_t rsv0 : 1;
        uint8_t avl1 : 4;
        uint64_t address : 51;
        uint8_t xd : 1; /* execute disable */
    };
    uint64_t raw;
} PageMapLevel4Entry;

typedef union {
    struct KPACK {
        uint8_t present : 1;
        uint8_t read_write : 1;
        uint8_t us : 1;  /* user/super access */
        uint8_t pwt : 1; /* page write through */
        uint8_t pcd : 1; /* page cache disable */
        uint8_t accessed : 1;
        uint8_t avl0 : 1;
        uint8_t ps : 1; /* page size bit */
        uint8_t avl1 : 4;
        uint64_t address : 51;
        uint8_t xd : 1; /* execute disable */
    };
    uint64_t raw;
} PageDirectoryPointerTable;

typedef union {
    struct KPACK {
        uint8_t present : 1;
        uint8_t read_write : 1;
        uint8_t us : 1;  /* user/super access */
        uint8_t pwt : 1; /* page write through */
        uint8_t pcd : 1; /* page cache disable */
        uint8_t accessed : 1;
        uint8_t avl0 : 1;
        uint8_t ps : 1; /* page size bit */
        uint8_t avl1 : 4;
        uint64_t address : 51;
        uint8_t xd : 1; /* execute disable */
    };
    uint64_t raw;
} PageDirectoryEntry64;

typedef union {
    struct KPACK {
        uint8_t present : 1;
        uint8_t read_write : 1;
        uint8_t us : 1;  /* user/super access */
        uint8_t pwt : 1; /* page write through */
        uint8_t pcd : 1; /* page cache disable */
        uint8_t accessed : 1;
        uint8_t dirty : 1;
        uint8_t pat : 1; /* page attribute table */
        uint8_t g : 1;   /* global */
        uint8_t avl0 : 3;

        uint64_t address : 40;

        uint8_t avl1 : 7;
        uint8_t pk : 4;
        uint8_t xd : 1; /* execute disable */
    };
    uint64_t raw;
} PageTableEntry64;

typedef PageMapLevel4Entry PageTopEntry;
typedef uint64_t PageAnyEntry;

/* exported functions */
#define ALLOC_ERR_RESTRICTED_PAGE_ADDRESS 1
#define ALLOC_ERR_MEMORY_OVERFLOW 2
int KEXP KeAllocateMemory(uintptr_t physical_address, size_t length,
                          uintptr_t page_address, int page_flags);
int KEXP KeDeallocateMemory(uintptr_t page_address, size_t length);

/* private functions */
int KAPI InitializePaging(void);

int KAPI SetupKernelPaging(PageTopEntry *top);

void KAPI InvalidatePage(uintptr_t page_address);

int KAPI AllocatePages(PageTopEntry *top, uintptr_t physical_address,
                       size_t length, uintptr_t page_address, int page_flags);
void KAPI FreePages(PageTopEntry *top, uintptr_t page_address, size_t length);
void KAPI DeallocatePages(PageTopEntry *top, uintptr_t page_address,
                          size_t length);
#endif
