
#include "paging.h"

#include "diag/print.h"
#include "memory.h"

extern void _pg_load(void *);
extern void _pg_invld(void *);
extern void _pg_exec_check(void);

#define KMAP_SIZE 512 * 4096 /* 2MB */
#define KMAP_LOWER_BOUND KMAP_SIZE
#define KMAP_UPPER_BOUND KERNEL_VBASE

static PageTopEntry *page_dir;
void KAPI InvalidatePage(uintptr_t page_address) {
    _pg_invld((void *)page_address);
}

int KAPI SetupKernelPaging(PageTopEntry *top) {
    if (top != page_dir) {
        /* we can just copy the mapping */
        top[0] = page_dir[0];
        top[KERNEL_VBASE >> 39] = page_dir[KERNEL_VBASE >> 39];
        return KSUCCESS;
    }

    /* identity map of first region */
    int err =
        AllocatePages(top, 0x0, KMAP_SIZE, 0x0, PAGE_EXECUTE | PAGE_READWRITE);
    if (err != KSUCCESS) {
        return err;
    }
    /* higher half kernel */
    err = AllocatePages(top, KERNEL_PBASE, KMAP_SIZE, KERNEL_VBASE,
                        PAGE_READWRITE | PAGE_EXECUTE);
    return err;
}

int KAPI InitializePaging(void) {
    const size_t page_dir_sz = sizeof(PageMapLevel4Entry) * PG_PML4_ENTRIES;
    page_dir = AllocatePhysicalMemory(page_dir_sz, PG_ALIGN);
    if (page_dir == MM_ERR_ALLOC_FAILED) {
        return PG_ERR_ALLOC_FAILED;
    }
    KeZeroMemory(page_dir, page_dir_sz);

    SetupKernelPaging(page_dir);

    _pg_exec_check();
    _pg_load(page_dir);

    return KSUCCESS;
}

static PageDirectoryPointerTable *AllocatePDPTables(PageMapLevel4Entry *entry,
                                                    int page_flags) {
    entry->present = 1;
    entry->read_write = (page_flags & PAGE_READWRITE) ? 1 : 0;
    entry->us = (page_flags & PAGE_ACCESSALL) ? 1 : 0;
    entry->xd = (page_flags & PAGE_EXECUTE) ? 0 : 1;
    entry->pwt = (page_flags & PAGE_WRITETHROUGH) ? 1 : 0;
    entry->pcd = (page_flags & PAGE_NOCACHE) ? 1 : 0;

    const size_t sz = sizeof(PageDirectoryPointerTable) * PG_PDP_ENTRIES;
    PageDirectoryPointerTable *pd = AllocatePhysicalMemory(sz, PG_ALIGN);
    if (pd == MM_ERR_ALLOC_FAILED) {
        return 0;
    }
    KeZeroMemory(pd, sz);
    entry->raw |= (uintptr_t)pd;

    return pd;
}

static PageDirectoryEntry64 *AllocatePDTables(PageDirectoryPointerTable *entry,
                                              int page_flags) {
    entry->present = 1;
    entry->read_write = (page_flags & PAGE_READWRITE) ? 1 : 0;
    entry->us = (page_flags & PAGE_ACCESSALL) ? 1 : 0;
    entry->xd = (page_flags & PAGE_EXECUTE) ? 0 : 1;
    entry->pwt = (page_flags & PAGE_WRITETHROUGH) ? 1 : 0;
    entry->pcd = (page_flags & PAGE_NOCACHE) ? 1 : 0;
    const size_t sz = sizeof(PageDirectoryEntry64) * PG_PD_ENTRIES;
    PageDirectoryEntry64 *pd = AllocatePhysicalMemory(sz, PG_ALIGN);
    if (pd == MM_ERR_ALLOC_FAILED) {
        return 0;
    }
    KeZeroMemory(pd, sz);
    entry->raw |= (uintptr_t)pd;

    return pd;
}

static PageTableEntry64 *AllocatePageTables(PageDirectoryEntry64 *entry,
                                            int page_flags) {
    entry->present = 1;
    entry->read_write = (page_flags & PAGE_READWRITE) ? 1 : 0;
    entry->us = (page_flags & PAGE_ACCESSALL) ? 1 : 0;
    entry->xd = (page_flags & PAGE_EXECUTE) ? 0 : 1;
    entry->pwt = (page_flags & PAGE_WRITETHROUGH) ? 1 : 0;
    entry->pcd = (page_flags & PAGE_NOCACHE) ? 1 : 0;
    const size_t sz = sizeof(PageTableEntry64) * PG_ENTRIES;
    PageTableEntry64 *pt = AllocatePhysicalMemory(sz, PG_ALIGN);
    if (pt == MM_ERR_ALLOC_FAILED) {
        return 0;
    }

    KeZeroMemory(pt, sz);
    entry->raw |= (uintptr_t)pt;

    return pt;
}

static void InitializePageTableEntry(PageTableEntry64 *entry, uintptr_t address,
                                     int page_flags) {
    entry->present = 1;
    entry->read_write = (page_flags & PAGE_READWRITE) ? 1 : 0;
    entry->us = (page_flags & PAGE_ACCESSALL) ? 1 : 0;
    entry->xd = (page_flags & PAGE_EXECUTE) ? 0 : 1;
    entry->pwt = (page_flags & PAGE_WRITETHROUGH) ? 1 : 0;
    entry->pcd = (page_flags & PAGE_NOCACHE) ? 1 : 0;
    entry->raw |= (address & PG_ADDR);
}
static int AllAreClean(PageAnyEntry *entries, size_t count) {
    size_t cnt2 = 0;
    for (size_t i = 0; i < count; i++) {
        if (entries[i] != 0)
            cnt2++;
    }

    return cnt2 == count;
}

void KAPI FreePages(PageTopEntry *top, uintptr_t page_address, size_t length) {
    if (top == 0) {
        top = page_dir;
    }
    for (size_t i = 0; i < length; i += PG_SIZE) {
        uintptr_t addr = (page_address + i);

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
            continue;
        } else {
            pdp = (PageDirectoryPointerTable *)(top[pml4_idx].raw & PG_ADDR);
        }

        if (!pdp[pdp_idx].present) {
            continue;
        } else {
            pd = (PageDirectoryEntry64 *)(pdp[pdp_idx].raw & PG_ADDR);
        }

        if (!pd[pd_idx].present) {
            continue;
        } else {
            pt = (PageTableEntry64 *)(pd[pd_idx].raw & PG_ADDR);
        }

        pt[pt_idx].raw = 0;
    }
}

void KAPI DeallocatePages(PageTopEntry *top, uintptr_t page_address,
                          size_t length) {
    if (top == 0) {
        top = page_dir;
    }
    for (size_t i = 0; i < length; i += PG_SIZE) {
        uintptr_t addr = (page_address + i);

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
            continue;
        } else {
            pdp = (PageDirectoryPointerTable *)(top[pml4_idx].raw & PG_ADDR);
        }

        if (!pdp[pdp_idx].present) {
            continue;
        } else {
            pd = (PageDirectoryEntry64 *)(pdp[pdp_idx].raw & PG_ADDR);
        }

        if (!pd[pd_idx].present) {
            continue;
        } else {
            pt = (PageTableEntry64 *)(pd[pd_idx].raw & PG_ADDR);
        }

        pt[pt_idx].raw = 0;

        if (AllAreClean((PageAnyEntry *)pt, PG_ENTRIES)) {
            DeallocatePhysicalMemory(pt);
            pd[pd_idx].raw = 0;
        }

        if (AllAreClean((PageAnyEntry *)pd, PG_PD_ENTRIES)) {
            DeallocatePhysicalMemory(pd);
            pdp[pdp_idx].raw = 0;
        }

        if (AllAreClean((PageAnyEntry *)pdp, PG_PDP_ENTRIES)) {
            DeallocatePhysicalMemory(pdp);
            top[pml4_idx].raw = 0;
        }

        /*
        if (AllAreClean((PageAnyEntry *)top, PG_PML4_ENTRIES)) {
            DeallocatePhysicalMemory(top);
        }*/
    }
}

int KAPI AllocatePages(PageTopEntry *top, uintptr_t physical_address,
                       size_t length, uintptr_t page_address, int page_flags) {

    if ((physical_address % PG_ALIGN) || (page_address % PG_ALIGN))
        return PG_ERR_INVALID_ALIGN;

    if (top == 0) {
        top = page_dir;
    }

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
    }

    return KSUCCESS;
}

int KEXP KeAllocateMemory(uintptr_t physical_address, size_t length,
                          uintptr_t page_address, int page_flags) {
    if ((page_address % PG_ALIGN) || page_address < KMAP_LOWER_BOUND ||
        page_address + ALIGN(length, PG_ALIGN) > KMAP_UPPER_BOUND) {
        return ALLOC_ERR_RESTRICTED_PAGE_ADDRESS;
    }

    int res = AllocatePages(page_dir, physical_address, length, page_address,
                            page_flags);
    if (res != KSUCCESS) {
        return ALLOC_ERR_MEMORY_OVERFLOW;
    }
    return KSUCCESS;
}

int KEXP KeDeallocateMemory(uintptr_t page_address, size_t length) {

    if ((page_address % PG_ALIGN) || page_address < KMAP_LOWER_BOUND ||
        page_address + ALIGN(length, PG_ALIGN) > KMAP_UPPER_BOUND) {
        return ALLOC_ERR_RESTRICTED_PAGE_ADDRESS;
    }

    DeallocatePages(page_dir, page_address, length);
    return KSUCCESS;
}
