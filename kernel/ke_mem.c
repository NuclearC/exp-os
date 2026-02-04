
#include "ke_mem.h"
    
int KeMemCpy(void* restrict dest, const void* restrict src, size_t nbytes) {
    for (size_t i = 0; i < nbytes; ++i) {
        *((char*)dest + i) = *((char*)src + i);
    }
    return 0;
}

