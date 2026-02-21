
#include "paging.h"
#include "print.h"
#include "memory/memory.h"

extern void _mem_pg_load(PageDirectoryEntry32* dir_table);

#define PAGE_DIR_COUNT 1024
#define PAGE_TABLE_COUNT 1024
#define PAGE_

PageDirectoryEntry32 KALIGN(4096) base_directory[PAGE_DIR_COUNT];
PageTableEntry32 KALIGN(4096) base_table[PAGE_TABLE_COUNT];
PageTableEntry32 KALIGN(4096) higher_table[PAGE_TABLE_COUNT];

void KPRIV KeInitializePageDirectories(PageDirectoryEntry32* entry, size_t nentries, int flags) {
    for (size_t i = 0; i < nentries; i++) {
        entry[i].bits.present = 1;
        entry[i].bits.read_write = (flags & PAGE_READ_WRITE) ? 1 : 0;
        entry[i].bits.user_access = (flags & PAGE_ACCESS_ALL) ? 1 : 0;
        entry[i].bits.pwt = 0;
        entry[i].bits.disable_cache = 0;
        entry[i].bits.accessed = 0;
        entry[i].bits.available1 = 0;
        entry[i].bits.available4 = 0;
        entry[i].bits.page_size = (flags & PAGE_SIZE_4M) ? 1 : 0;
        entry[i].bits.addr = 0;
    }
}

void KPRIV KeInitializePageTables(PageTableEntry32* entry, size_t nentries, void* base_address, int flags) {
    for (size_t i = 0; i < nentries; i++) {
        entry[i].bits.present = 1;
        entry[i].bits.read_write = (flags & PAGE_READ_WRITE) ? 1 : 0;
        entry[i].bits.user_access = (flags & PAGE_ACCESS_ALL) ? 1 : 0;
        entry[i].bits.pwt = 0;
        entry[i].bits.disable_cache = 0;
        entry[i].bits.accessed = 0;
        entry[i].bits.dirty = 0;
        entry[i].bits.global = 0;
        entry[i].bits.available = 0;
        entry[i].bits.pat = 0;
        entry[i].bits.addr = 0;

        entry[i].wrd |= (uint32_t)base_address;
        base_address = (void*)((uint32_t)base_address + 4096);
    }
}

void KPRIV KeLoadPageDirectories(PageDirectoryEntry32* entry) {
    _mem_pg_load(entry);
}

/*
int KPRIV KeAllocatePageTables(size_t nentries, int flags, void** paddr) {
    size_t cnt = 0;
    for (size_t i = 1; i < 768; i++) {
        for (size_t j = 0; j < PAGE_TABLE_COUNT; j++) {

        }
    }
}
*/

void KPRIV InitializePages(const PhysicalMemoryMapEntry* memory_map, size_t nentries) {

    KeMemoryZero(base_directory, PAGE_DIR_COUNT * sizeof(PageDirectoryEntry32));
    /* first we do the identity mapping */
    KeInitializePageDirectories(base_directory, 1, PAGE_READ_WRITE);
    base_directory[0].wrd |= (uint32_t)KeVirtualToPhysical(base_table);

    KeInitializePageDirectories(base_directory + 768, 1, PAGE_READ_WRITE);
    base_directory[768].wrd |= (uint32_t)KeVirtualToPhysical(higher_table);

    KeInitializePageTables(base_table, PAGE_TABLE_COUNT, 0, PAGE_READ_WRITE);
    KeInitializePageTables(higher_table, PAGE_TABLE_COUNT, (void*)0x100000, PAGE_READ_WRITE);

    KeLoadPageDirectories(KeVirtualToPhysical(base_directory));
}

