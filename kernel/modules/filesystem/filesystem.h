
#ifndef NC_MODULES_FILESYSTEM_H_
#define NC_MODULES_FILESYSTEM_H_

#include "ke_main.h"
#include "typedefs.h"

typedef struct KPACK {
    uint16_t sector;
    uint32_t length;
    char name[10];
} FileTableEntry;

int KAPI LoadFiles(void);

int KAPI FindFile(const char *name, int name_len, uint32_t *length);

uint64_t KAPI ReadFile(int file_index, void *buffer, size_t num_bytes,
                       size_t offset);

#endif
