
#include "memory.h"
#include "print.h"

#define MEMORY_BLOCKS 1024
static PhysicalMemoryBlock blocks[MEMORY_BLOCKS];
static size_t block_count = 0;

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

void KAPI KeMemorySet(const void *source, int set, size_t nbytes) {
    const size_t ndwords = nbytes / 4;
    for (size_t i = 0; i < ndwords; ++i) {
        *((uint32_t *)source + i) = set;
    }
    for (size_t i = 0; i < (nbytes & 3); ++i) {
        *((uint8_t *)source + ndwords * 4 + i) = (set >> (8 * i)) & 0xff;
    }
}

void KAPI KeMemoryZero(const void *source, size_t nbytes) {
    KeMemorySet(source, 0, nbytes);
}

void *KAPI KeVirtualToPhysical(void *virt) {
    return (void *)((uint32_t)virt + KERNEL_PBASE - KERNEL_VBASE);
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
                blocks[0].base = (void *)(entry->base_low);
                if (blocks[0].base <= kernel_image->stack_end) {
                    blocks[0].base = kernel_image->stack_end;
                }
                blocks[0].length = entry->base_low + entry->length_low -
                                   (size_t)(blocks[0].base);
            }
        }
    }
    KePrint("available memory: %x %x \n", blocks[0].base, blocks[0].length);
}

void KAPI KePrintBlocks(int maxcnt) {
    for (int i = 0; i < maxcnt; i++) {
        KePrint("block %d %x:%x \n", i, blocks[i].base,
                blocks[i].base + blocks[i].length);
    }
}

void *KAPI KeAllocatePhysicalMemory(size_t length, size_t align) {
    /* walk through memory to find an available block */
    block_count += 1;
    blocks[block_count].base = blocks[block_count - 1].base;
    if (block_count > 1) {
        blocks[block_count].base += blocks[block_count - 1].length;
    }
    if ((size_t)blocks[block_count].base % align) {
        blocks[block_count].base =
            (void *)(((size_t)blocks[block_count].base / align + 1) * align);
    }
    blocks[block_count].length = length;

    KePrint("allocated %d (align %d) bytes at %x \n", length, align,
            blocks[block_count].base);
    return blocks[block_count].base;
}
