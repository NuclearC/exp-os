
#include "string.h"

#include "memory/memory.h"
#include <signal.h>

#define GetStringBuffer(string)                                                \
    (string->length <= KSTR_INIT_CAP) ? (string->sm.buf) : (string->big.buf)

int KAPI KeCreateString(KString *string, const char *init) {
    size_t len = 0;
    if (init != 0) {
        for (const char *ch = init; *ch != 0; ch++) {
            len += 1;
        }
    }

    KeCreateStringLength(string, len);
    KeMemoryCopy(GetStringBuffer(string), init, len);

    return 0;
}

int KAPI KeCreateStringLength(KString *string, size_t len) {
    if (len <= KSTR_INIT_CAP) {
        string->sm.length = len;
    } else {
        string->big.cap = len;
        string->big.length = len;
        string->big.buf = KeAllocatePhysicalMemory(string->big.cap, MEM_ALIGN);
    }
    return 0;
}

void KAPI KeEnsureCapacity(KString *string, size_t capacity) {
    if (string->length <= KSTR_INIT_CAP) {
        /* we are using the small buffer */
        if (capacity > KSTR_INIT_CAP) {
            char buf[16];
            size_t old_len = string->length;
            KeMemoryCopy(buf, string->sm.buf, old_len);

            KeCreateStringLength(string, capacity);
            KeMemoryCopy(GetStringBuffer(string), buf, old_len);
        }

    } else {
        /* we need the big buffer */
        if (capacity > string->big.cap) {
            /* if we are succesful then we are lucky */
            if (KeTryReallocatePhysicalMemory(GetStringBuffer(string),
                                              capacity) == 0)
                return;

            char *buf = KeAllocatePhysicalMemory(capacity, MEM_ALIGN);
            KeMemoryZero(buf, capacity);
            KeMemoryCopy(buf, string->big.buf, string->length);

            KeDeallocatePhysicalMemory(string->big.buf);

            string->length = capacity;
            string->big.buf = buf;
            string->big.cap = capacity;
        }
    }
}

void KAPI KeCopyString(KString *dest, const KString *source) {
    KeEnsureCapacity(dest, source->length);

    KeMemoryCopy(GetStringBuffer(dest), GetStringBuffer(source),
                 source->length);
}

void KAPI KeMoveString(KString *dest, KString *source) {
    KeMemoryCopy(dest, source, sizeof(KString));
    KeMemoryZero(source, sizeof(KString));
}

void KAPI KeDestroyString(const KString *string) {
    if (string->length > KSTR_INIT_CAP) {
        KeDeallocatePhysicalMemory(string->big.buf);
    }
}
