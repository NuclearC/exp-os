
#include "paging.h"

#include "diag/print.h"
#include "memory.h"

extern void _pg_load(void *);
extern void _pg_invld(void *);

static PageTopEntry *page_dir;

int KAPI InitializePaging(void) {
    const size_t page_dir_sz = sizeof(PageMapLevel4Entry) * PG_PML4_ENTRIES;
    page_dir = AllocatePhysicalMemory(page_dir_sz, PG_ALIGN);
    if (page_dir == MM_ERR_ALLOC_FAILED) {
        return PG_ERR_ALLOC_FAILED;
    }
    ZeroMemory(page_dir, page_dir_sz);

    int err = AllocatePages(page_dir, 0x0, 512 * 4096, 0x0,
                            PAGE_EXECUTE | PAGE_READWRITE);
    if (err != KSUCCESS) {
        return err;
    }

    err = AllocatePages(page_dir, KERNEL_PBASE, 512 * 4096, KERNEL_VBASE,
                        PAGE_READWRITE | PAGE_EXECUTE);
    if (err != KSUCCESS) {
        return err;
    }

    _pg_load(page_dir);

    return KSUCCESS;
}

static PageDirectoryPointerTable *AllocatePDPTables(PageMapLevel4Entry *entry,
                                                    int page_flags) {
    entry->present = 1;
    entry->read_write = (page_flags & PAGE_READWRITE) ? 1 : 0;
    entry->us = (page_flags & PAGE_ACCESSALL) ? 1 : 0;
    entry->xd = (page_flags & PAGE_EXECUTE) ? 0 : 1;

    const size_t sz = sizeof(PageDirectoryPointerTable) * PG_PDP_ENTRIES;
    PageDirectoryPointerTable *pd = AllocatePhysicalMemory(sz, PG_ALIGN);
    if (pd == MM_ERR_ALLOC_FAILED) {
        return 0;
    }
    ZeroMemory(pd, sz);
    entry->raw |= (uintptr_t)pd;

    return pd;
}

static PageDirectoryEntry64 *AllocatePDTables(PageDirectoryPointerTable *entry,
                                              int page_flags) {
    entry->present = 1;
    entry->read_write = (page_flags & PAGE_READWRITE) ? 1 : 0;
    entry->us = (page_flags & PAGE_ACCESSALL) ? 1 : 0;
    entry->xd = (page_flags & PAGE_EXECUTE) ? 0 : 1;

    const size_t sz = sizeof(PageDirectoryEntry64) * PG_PD_ENTRIES;
    PageDirectoryEntry64 *pd = AllocatePhysicalMemory(sz, PG_ALIGN);
    if (pd == MM_ERR_ALLOC_FAILED) {
        return 0;
    }
    ZeroMemory(pd, sz);
    entry->raw |= (uintptr_t)pd;

    return pd;
}

static PageTableEntry64 *AllocatePageTables(PageDirectoryEntry64 *entry,
                                            int page_flags) {
    entry->present = 1;
    entry->read_write = (page_flags & PAGE_READWRITE) ? 1 : 0;
    entry->us = (page_flags & PAGE_ACCESSALL) ? 1 : 0;
    entry->xd = (page_flags & PAGE_EXECUTE) ? 0 : 1;

    const size_t sz = sizeof(PageTableEntry64) * PG_ENTRIES;
    PageTableEntry64 *pt = AllocatePhysicalMemory(sz, PG_ALIGN);
    if (pt == MM_ERR_ALLOC_FAILED) {
        return 0;
    }

    ZeroMemory(pt, sz);
    entry->raw |= (uintptr_t)pt;

    return pt;
}

static void InitializePageTableEntry(PageTableEntry64 *entry, uintptr_t address,
                                     int page_flags) {
    entry->present = 1;
    entry->read_write = (page_flags & PAGE_READWRITE) ? 1 : 0;
    entry->us = (page_flags & PAGE_ACCESSALL) ? 1 : 0;
    entry->xd = (page_flags & PAGE_EXECUTE) ? 0 : 1;

    entry->raw |= (address & PG_ADDR);
}

int KAPI AllocatePages(PageTopEntry *top, uintptr_t physical_address,
                       size_t length, uintptr_t page_address, int page_flags) {

    if ((physical_address % PG_ALIGN) || (page_address % PG_ALIGN))
        return PG_ERR_INVALID_ALIGN;

    for (size_t i = 0; i < length; i += PG_SIZE) {
        uintptr_t addr = (page_address + i);
        uintptr_t paddr = (physical_address + i);

        addr >>= 12;
        size_t pt_idx = (addr & 0x1ff);
        addr >>= 9;
        size_t pd_idx = (addr & 0x1ff);
        addr >>= 9;
        size_t pdp_idx = (addr & 0x1ff);
        addr >>= 9;
        size_t pml4_idx = (addr & 0x1ff);

        PageDirectoryPointerTable *pdp = 0;
        PageDirectoryEntry64 *pd = 0;
        PageTableEntry64 *pt = 0;
        if (!top[pml4_idx].present) {
            pdp = AllocatePDPTables(&top[pml4_idx], page_flags);
        } else {
            pdp = (PageDirectoryPointerTable *)(top[pml4_idx].raw & PG_ADDR);
        }

        if (pdp == 0) {
            return PG_ERR_ALLOC_FAILED;
        }

        if (!pdp[pdp_idx].present) {
            pd = AllocatePDTables(&pdp[pdp_idx], page_flags);
        } else {
            pd = (PageDirectoryEntry64 *)(pdp[pdp_idx].raw & PG_ADDR);
        }
        if (pd == 0) {
            return PG_ERR_ALLOC_FAILED;
        }

        if (!pd[pd_idx].present) {
            pt = AllocatePageTables(&pd[pd_idx], page_flags);
        } else {
            pt = (PageTableEntry64 *)(pd[pd_idx].raw & PG_ADDR);
        }
        if (pt == 0) {
            return PG_ERR_ALLOC_FAILED;
        }

        InitializePageTableEntry(&pt[pt_idx], paddr, page_flags);

        /* Print("page %d %x %x %x %x \n", pt_idx, pt[pt_idx].raw,
           pd[pd_idx].raw, pdp[pdp_idx].raw, top[pml4_idx].raw); */
    }

    return KSUCCESS;
}
