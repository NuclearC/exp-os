
#include "memory.h"
    
int KAPI KeMemoryCopy(void* restrict dest, const void* restrict src, size_t nbytes) {
    const size_t ndwords = nbytes >> 2;
    for (size_t i = 0; i < ndwords; ++i) {
        *((uint32_t*)dest + i) = *((uint32_t*)src + i);
    }
    for (size_t i = 0; i < (nbytes & 3); ++i) {
        *((uint8_t*)dest + ndwords * 4 + i) = *((uint8_t*)src + ndwords * 4 + i);
    }
    return 0;
}
 
int KAPI KeMemoryCompare(const void* source1, const void* source2, size_t nbytes) {
    const size_t ndwords = nbytes >> 2;
    for (size_t i = 0; i < ndwords; ++i) {
        if (*((uint32_t*)source1 + i) != *((uint32_t*)source2 + i)) {
            return 1;
        }
    }
    for (size_t i = 0; i < (nbytes & 3); ++i) {
        if (*((uint8_t*)source1 + ndwords * 4 + i) != *((uint8_t*)source2 + ndwords * 4 + i)) {
            return 1;
        }
    }
    return 0;
}


