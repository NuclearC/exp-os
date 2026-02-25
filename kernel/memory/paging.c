
#include "paging.h"
#include "diag/print.h"
#include "memory/memory.h"

extern void _mem_pg_load(PageDirectoryEntry32 *dir_table);
extern void _mem_pg_invld(int wtf);

static PageDirectoryEntry32 *base_directory;

void KPRIV KeInitializePageDirectories(PageDirectoryEntry32 *entry,
                                       size_t nentries, uint32_t flags) {
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

void KPRIV KeInitializePageTables(PageTableEntry32 *entry, size_t nentries,
                                  uintptr_t base_address, uint32_t flags) {
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

        entry[i].wrd |= (size_t)base_address;
        base_address += PAGE_SIZE;
    }
}

void KPRIV KeLoadPageDirectories(PageDirectoryEntry32 *entry) {
    _mem_pg_load(entry);
}

void KPRIV InitializePages(void) {
    PageTableEntry32 *base_table, *higher_table;

    /* create the tables */
    base_directory = KeAllocatePhysicalMemory(
        sizeof(PageDirectoryEntry32) * PAGE_DIR_COUNT, 4096);

    base_table = KeAllocatePhysicalMemory(
        sizeof(PageTableEntry32) * PAGE_TABLE_COUNT, 4096);
    higher_table = KeAllocatePhysicalMemory(
        sizeof(PageTableEntry32) * PAGE_TABLE_COUNT, 4096);

    KeMemoryZero(base_directory, PAGE_DIR_COUNT * sizeof(PageDirectoryEntry32));
    KeMemoryZero(base_table, PAGE_TABLE_COUNT * sizeof(PageTableEntry32));
    KeMemoryZero(higher_table, PAGE_TABLE_COUNT * sizeof(PageTableEntry32));

    /* first we do the identity mapping */
    KeInitializePageDirectories(base_directory, 1, PAGE_READ_WRITE);
    base_directory[0].wrd |= (size_t)base_table;

    /* higher kernel mapping */
    KeInitializePageDirectories(base_directory + 768, 1, PAGE_READ_WRITE);
    base_directory[768].wrd |= (size_t)higher_table;

    KeInitializePageTables(base_table, PAGE_TABLE_COUNT, 0, PAGE_READ_WRITE);
    KeInitializePageTables(higher_table, PAGE_TABLE_COUNT, 0x100000,
                           PAGE_READ_WRITE);
    /* load the page directories */
    KeLoadPageDirectories(base_directory);
}

void KPRIV KeMapPageTables(uintptr_t physical_address, size_t length,
                           uintptr_t page_address, uint32_t flags) {
    PageTableEntry32 *table;
    for (size_t i = 0; i < length; i += PAGE_SIZE) {
        size_t addr = (size_t)(page_address + i);
        size_t dirindex = (addr >> 22) & 1023;
        size_t tableindex = (addr >> 12) & 1023;
        if (base_directory[dirindex].wrd == 0) {
            KeInitializePageDirectories(&base_directory[dirindex], 1, flags);

            table = KeAllocatePhysicalMemory(
                sizeof(PageTableEntry32) * PAGE_TABLE_COUNT, PAGE_ALIGN);

            KeMemoryZero(table, sizeof(PageTableEntry32) * PAGE_TABLE_COUNT);
            base_directory[dirindex].wrd |= (uintptr_t)table;
        } else {
            table =
                (PageTableEntry32 *)(base_directory[dirindex].wrd & 0xfffff000);
        }
        if (1 || table[tableindex].wrd == 0) {
            KeInitializePageTables(&table[tableindex], 1, physical_address + i,
                                   flags);
        }
        _mem_pg_invld(addr);
    }
}

void KPRIV KeUnmapPageTables(uintptr_t page_address, size_t length) {
    PageTableEntry32 *table;
    for (size_t i = 0; i < length; i += PAGE_SIZE) {
        size_t addr = (size_t)(page_address + i);
        size_t dirindex = (addr >> 22) & 1023;
        size_t tableindex = (addr >> 12) & 1023;

        if (base_directory[dirindex].wrd != 0) {
            table =
                (PageTableEntry32 *)(base_directory[dirindex].wrd & 0xfffff000);
            if (table[tableindex].wrd != 0) {
                table[tableindex].wrd = 0;
            }
        }

        _mem_pg_invld(addr);
    }
}
