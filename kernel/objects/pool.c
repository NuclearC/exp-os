

#include "pool.h"

#include "memory/memory.h"

int KAPI KeCreatePool(KPool *pool, size_t size) {
    pool->length = size;
    pool->data = KeAllocatePhysicalMemory(size, MEM_ALIGN);
    KeMemoryZero(pool->data, pool->length);

    return 0;
}

void *KAPI KeTryAcquire(KPool *pool, size_t element_size) {
    for (size_t i = 0; i < pool->length; i += element_size) {
        uintptr_t b = *(uintptr_t *)(pool->data + i);

        if (b == 0) {
            return pool->data + i;
        }
    }

    return 0;
}

void KAPI KeDestroyPool(KPool const *pool) {
    if (pool != 0) {
        KeDeallocatePhysicalMemory(pool->data);
    }
}
int KAPI KeHasPool(KPool *pool, const void *object) {
    return (pool != 0) &&
           (object >= pool->data && object <= pool->data + pool->length);
}
