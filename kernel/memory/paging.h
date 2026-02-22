
#ifndef NC_KE_MEMORY_PAGING_H_
#define NC_KE_MEMORY_PAGING_H_

#include "ke_main.h"
#include "typedefs.h"

#define PAGE_READ_WRITE 0x0001
#define PAGE_ACCESS_ALL 0x0002
#define PAGE_SIZE_4M 0x0004

#define PAGE_SIZE 4096

#define PAGE_DIR_COUNT 1024
#define PAGE_TABLE_COUNT 1024

typedef union {
    struct {
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
    } bits;
    uint32_t wrd;
} PageDirectoryEntry32;

typedef union {
    struct {
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
    } bits;
    uint32_t wrd;
} PageTableEntry32;

void KPRIV InitializePages(void);

void KPRIV KeInitializePageDirectories(PageDirectoryEntry32 *entry,
                                       size_t nentries, int flags);

void KPRIV KeInitializePageTables(PageTableEntry32 *entry, size_t nentries,
                                  void *base_address, int flags);
void KPRIV KeLoadPageDirectories(PageDirectoryEntry32 *entry);

void KPRIV KeAllocatePageTables(void *memory, size_t length, void *page,
                                int flags);
void KPRIV KeDeallocatePageTables(size_t length, void *page);
/*
int KPRIV KeAllocatePageTables(size_t nentries, int flags, void** paddr);
*/
#endif
