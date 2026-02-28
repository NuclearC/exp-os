
#include "memory.h"
#include "diag/print.h"

#define MEMORY_BLOCKS 1024
static PhysicalMemoryBlock blocks[MEMORY_BLOCKS];
static size_t block_count;

int KAPI KeMemoryCopy(void *restrict dest, const void *restrict src,
                      size_t nbytes) {
    const size_t ndwords = nbytes >> 2;
    for (size_t i = 0; i < ndwords; ++i) {
        *((uint32_t *)dest + i) = *((uint32_t *)src + i);
    }
    for (size_t i = 0; i < (nbytes & 3); ++i) {
        *((uint8_t *)dest + ndwords * 4 + i) =
            *((uint8_t *)src + ndwords * 4 + i);
    }
    return 0;
}

int KAPI KeMemoryCompare(const void *source1, const void *source2,
                         size_t nbytes) {
    const size_t ndwords = nbytes >> 2;
    for (size_t i = 0; i < ndwords; ++i) {
        if (*((uint32_t *)source1 + i) != *((uint32_t *)source2 + i)) {
            return 1;
        }
    }
    for (size_t i = 0; i < (nbytes & 3); ++i) {
        if (*((uint8_t *)source1 + ndwords * 4 + i) !=
            *((uint8_t *)source2 + ndwords * 4 + i)) {
            return 1;
        }
    }
    return 0;
}

void KAPI KeMemorySet(void *dest, int set, size_t nbytes) {
    const size_t ndwords = nbytes / 4;
    for (size_t i = 0; i < ndwords; ++i) {
        *((uint32_t *)dest + i) = set;
    }
    for (size_t i = 0; i < (nbytes & 3); ++i) {
        *((uint8_t *)dest + ndwords * 4 + i) = (set >> (8 * i)) & 0xff;
    }
}

void KAPI KeMemoryZero(void *dest, size_t nbytes) {
    KeMemorySet(dest, 0, nbytes);
}

void KPRIV InitializeMemory(PhysicalMemoryMap const *memory_map,
                            KernelImage const *kernel_image) {
    KeMemoryZero(blocks, sizeof(blocks));
    block_count = 0;
    for (uint32_t i = 0; i < memory_map->count; i++) {
        PhysicalMemoryMapEntry *entry = &memory_map->entries[i];
        if (entry->type == 1) {
            /* usable memory block */
            if (entry->base_low >= 0x100000 && entry->length_low >= 0x1000000) {
                blocks[0].begin = entry->base_low;
                if (blocks[0].begin <= kernel_image->stack_end) {
                    blocks[0].begin = kernel_image->stack_end;
                }
                blocks[0].end = entry->base_low + entry->length_low;
            }
        }
    }
    KePrint("available memory: %x %x \n", blocks[0].begin, blocks[0].end);
}

void KAPI KePrintBlocks(size_t maxcnt) {
    for (size_t i = 0; i < MAX(maxcnt, 1 + block_count); i++) {
        KePrint("block %d %x:%x \n", i, blocks[i].begin, blocks[i].end);
    }
}

void KAPI KeDeallocatePhysicalMemory(const void *addr) {
    for (size_t i = 1; i <= block_count; i++) {
        if (blocks[i].begin == (uintptr_t)addr) {

            for (size_t j = i; j <= block_count - 1; j++) {
                blocks[j] = blocks[j + 1];
            }

            blocks[block_count].begin = (blocks[block_count].end = 0);
            block_count--;

            break;
        }
    }
}

int KAPI KeTryReallocatePhysicalMemory(const void *oldmemory,
                                       size_t new_length) {
    for (size_t i = 1; i <= block_count; i++) {
        if (blocks[i].begin == (uintptr_t)oldmemory) {
            uintptr_t endlimit = blocks[0].end;
            if (i != block_count) {
                endlimit = blocks[i + 1].begin;
            }
            if (blocks[i].begin + new_length <= endlimit) {
                blocks[i].end = blocks[i].begin + new_length;
                return 0;
            } else {
                return 1;
            }
        }
    }
    return 1;
}

void *KAPI KeAllocateContiguousMemory(size_t *length, size_t align,
                                      size_t minlength) {
    int ret = 0;
    if (block_count >= MEMORY_BLOCKS - 1)
        return 0;
    /* walk through memory to find an available block */
    if (block_count == 0) {
        blocks[1].begin = ALIGN(blocks[0].begin, align);
        blocks[1].end = MAX(blocks[0].begin + *length, blocks[0].end);
        block_count = 1;
        ret = 1;
    } else {

        for (size_t i = 1; i <= block_count; i++) {
            if (i == block_count) {
                uintptr_t begin = ALIGN(blocks[i].end, align),
                          end = MAX(begin + *length, blocks[0].end);

                if (end - begin < minlength)
                    continue;
                block_count++;
                blocks[block_count].begin = begin;
                blocks[block_count].end = end;
                ret = block_count;

                break;
            } else {
                uintptr_t begin = ALIGN(blocks[i].end, align),
                          end = MAX(begin + *length, blocks[i + 1].begin);

                if (end - begin < minlength)
                    continue;
                for (size_t j = block_count + 1; j >= i; j--) {
                    blocks[j] = blocks[j - 1];
                }
                blocks[i].begin = begin;
                blocks[i].end = end;
                ret = i;
                block_count++;
            }
        }
    }
    if (ret == 0)
        return 0;
    KePrint("allocated %d (align %d) bytes at %x:%x \n", length, align,
            blocks[ret].begin, blocks[ret].end);
    *length = blocks[ret].end - blocks[ret].begin;
    return (void *)blocks[ret].begin;
}

void *KAPI KeAllocatePhysicalMemory(size_t length, size_t align) {
    int ret = 0;
    if (block_count >= MEMORY_BLOCKS - 1)
        return 0;
    /* walk through memory to find an available block */
    if (block_count == 0) {
        blocks[1].begin = ALIGN(blocks[0].begin, align);
        if (blocks[1].begin + length > blocks[0].end) {
            blocks[1].begin = 0;
            return 0;
        }
        blocks[1].end = blocks[0].begin + length;

        block_count = 1;
        ret = 1;
    } else {
        for (size_t i = 1; i <= block_count; i++) {
            if (i == block_count) {
                uintptr_t begin = ALIGN(blocks[i].end, align),
                          end = begin + length;
                if (end <= blocks[0].end) {
                    block_count++;
                    blocks[block_count].begin = begin;
                    blocks[block_count].end = end;
                    ret = block_count;

                    break;
                }
            } else {
                uintptr_t begin = ALIGN(blocks[i].end, align),
                          end = begin + length;
                if (end <= blocks[i + 1].begin && end <= blocks[0].end) {
                    for (size_t j = block_count + 1; j >= i; j--) {
                        blocks[j] = blocks[j - 1];
                    }
                    blocks[i].begin = begin;
                    blocks[i].end = end;
                    ret = i;
                    block_count++;

                    break;
                }
            }
        }
    }
    if (ret == 0)
        return 0;
    KePrint("allocated %d (align %d) bytes at %x:%x \n", length, align,
            blocks[ret].begin, blocks[ret].end);
    return (void *)blocks[ret].begin;
}
