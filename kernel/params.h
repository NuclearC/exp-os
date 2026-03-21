
#ifndef NC_KE_PARAMS_H_
#define NC_KE_PARAMS_H_

#include "ke_main.h"
#include "typedefs.h"

typedef struct KPACK {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t acpi3_attr;
} PhysicalMemoryMapEntry;

typedef struct KPACK {
    uint32_t count;
    const PhysicalMemoryMapEntry *entries;
} PhysicalMemoryMap;

typedef struct {
    const PhysicalMemoryMap *memory_map;
} KernelParameters;

#endif
