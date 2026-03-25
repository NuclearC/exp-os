
#include "memory.h"
#include "params.h"

#include "diag/print.h"

#define MAX_MEMORY_BLOCKS 1024
#define MAX_AVAILABLE_BLOCKS 32

typedef struct {
    uintptr_t begin, end;
} PhysicalMemoryBlock;

static PhysicalMemoryBlock avl_blocks[MAX_AVAILABLE_BLOCKS];
static PhysicalMemoryBlock blocks[MAX_MEMORY_BLOCKS];
static int avl_block_count;
static int block_count;

void KAPI PrintMemoryBlocks(void) {
    for (int i = 0; i < block_count; i++) {
        Print("memory block %x:%x \n", blocks[i].begin, blocks[i].end);
    }
}

void KEXP KeMemoryCopy(void *restrict dest, const void *restrict src,
                       size_t nbytes) {
    const size_t nqwords = nbytes / 8;
    for (size_t i = 0; i < nqwords; ++i) {
        *((uint64_t *)dest + i) = *((uint64_t *)src + i);
    }
    for (size_t i = 0; i < (nbytes % 8); ++i) {
        *((uint8_t *)dest + nqwords * 8 + i) =
            *((uint8_t *)src + nqwords * 8 + i);
    }
}

void KEXP KeSetMemory(void *dest, size_t nbytes, uint64_t set) {
    const size_t nqwords = nbytes / 8;
    for (size_t i = 0; i < nqwords; ++i) {
        *((uint64_t *)dest + i) = set;
    }
    for (size_t i = 0; i < (nbytes % 8); ++i) {
        *((uint8_t *)dest + nqwords * 8 + i) = (set >> (8 * i)) & 0xff;
    }
}

void KEXP KeZeroMemory(void *dest, size_t nbytes) {
    KeSetMemory(dest, nbytes, 0);
}

int KEXP KeMemoryCompare(const void *source1, const void *source2,
                         size_t nbytes) {
    const size_t nqwords = nbytes / 8;
    for (size_t i = 0; i < nqwords; ++i) {
        if (*((uint64_t *)source1 + i) != *((uint64_t *)source2 + i)) {
            return 1;
        }
    }
    for (size_t i = 0; i < (nbytes % 8); ++i) {
        if (*((uint8_t *)source1 + nqwords * 8 + i) !=
            *((uint8_t *)source2 + nqwords * 8 + i)) {
            return 1;
        }
    }
    return 0;
}

void KAPI InitializeMemory(PhysicalMemoryMap const *memory_map) {
    KeZeroMemory(blocks, sizeof(blocks));
    KeZeroMemory(avl_blocks, sizeof(avl_blocks));

    block_count = 0;
    avl_block_count = 0;
    for (uint32_t i = 0; i < memory_map->count; i++) {
        PhysicalMemoryMapEntry const *entry = &memory_map->entries[i];
        /* useable memory, as per BIOS */
        if (entry->type == 1 && avl_block_count < MAX_AVAILABLE_BLOCKS &&
            entry->base > 0) {
            PhysicalMemoryBlock *block = &avl_blocks[avl_block_count++];
            block->begin = (uintptr_t)entry->base;
            block->end = block->begin + (uintptr_t)entry->length;

            if (block->begin <= KERNEL_PEND && block->end >= KERNEL_PEND) {
                block->begin = MAX(block->begin, KERNEL_PEND);
            }
        }
    }
}

int KAPI TryReallocatePhysicalMemory(const void *old_memory,
                                     size_t new_length) {
    DeallocatePhysicalMemory(old_memory);
    AllocatePhysicalMemory(new_length, MEM_ALIGN);
    return 0;
}

int KAPI DeallocatePhysicalMemory(const void *addr) {
    for (int i = 0; i < block_count; i++) {
        if (blocks[i].begin == (uintptr_t)addr) {
            if (i == block_count - 1) {
                blocks[i].begin = (blocks[i].end = 0);
                block_count--;
                return 0;
            } else {
                for (int j = i + 1; j < block_count; j++) {
                    blocks[j - 1] = blocks[j];
                }
                blocks[block_count - 1].begin =
                    (blocks[block_count - 1].end = 0);
                block_count--;
                return 0;
            }
        }
    }

    return MM_ERR_DEALLOC_INVLD;
}

void *KAPI AllocatePhysicalMemory(size_t length, size_t align) {
    Print("allocating %x bytes with align %x \n", length, align);
    if (block_count >= MAX_MEMORY_BLOCKS)
        return MM_ERR_ALLOC_FAILED;
    /* walk through memory to find an available block */
    if (block_count == 0) {
        for (int i = 0; i < avl_block_count; i++) {
            if (avl_blocks[i].end == 0)
                continue;
            uintptr_t begin = ALIGN(avl_blocks[i].begin, align);
            if (begin + length <= avl_blocks[i].end) {
                blocks[block_count].begin = begin;
                blocks[block_count].end = begin + length;
                return (void *)blocks[block_count++].begin;
            }
        }
    } else {
        for (int i = 0, j = 0; i < avl_block_count && j < block_count;) {
            if (blocks[j].begin > avl_blocks[i].end) {
                i++;
                continue;
            }

            /* blocks[j].begin <= avl_blocks[i].end aka we are inside
             * avl_blocks[i] */

            if (j < block_count - 1 &&
                blocks[j + 1].begin <= avl_blocks[i].end) {
                /* blocks j and j+1 are from same region, consider inside of
                 * them */
                uintptr_t begin = ALIGN(blocks[j].end, align);
                uintptr_t end = begin + length;
                if (end <= blocks[j + 1].begin) {
                    /* shift the blocks so j+1 becomes j+2 */
                    for (int k = block_count; k >= j + 2; k--) {
                        blocks[k] = blocks[k - 1];
                    }
                    block_count++;
                    blocks[j + 1].begin = begin;
                    blocks[j + 1].end = end;
                    return (void *)begin;
                }
            } else {
                uintptr_t begin = ALIGN(blocks[j].end, align);
                uintptr_t end = begin + length;
                if (end <= avl_blocks[i].end) {
                    if (j == block_count - 1) {
                        blocks[block_count].begin = begin;
                        blocks[block_count].end = end;
                    } else {
                        for (int k = block_count; k >= j + 1; k--) {
                            blocks[k] = blocks[k - 1];
                        }
                        blocks[j].begin = begin;
                        blocks[j].end = end;
                    }
                    block_count++;
                    return (void *)begin;
                }
            }
            j++; /* goto next block */
        }
    }
    /* we failed to find a block */
    return MM_ERR_ALLOC_FAILED;
}
