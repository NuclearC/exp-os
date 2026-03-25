
#include "filesystem.h"
#include "memory/memory.h"

#define MAX_FAT_ENTRIES 128

extern uint64_t _io_read_bytes(void *, uint64_t initsector, uint64_t num_bytes,
                               uint64_t skip_offset);
extern uint64_t _io_read_sector(void *, uint64_t initsector,
                                uint64_t numsectors);

static FileTableEntry files[MAX_FAT_ENTRIES];

int KAPI LoadFiles(void) {
    KeZeroMemory(files, sizeof(files));
    _io_read_sector(files, 1, 1);

    int cnt = 0;
    for (int i = 0; i < MAX_FAT_ENTRIES; i++) {
        if (files[i].length > 0) {
            cnt++;
        }
    }

    return cnt;
}

uint64_t KAPI ReadFile(int file_index, void *buffer, size_t num_bytes,
                       size_t offset) {
    if (files[file_index].length >= num_bytes + offset) {
        return _io_read_bytes(buffer, files[file_index].sector + offset / 512,
                              num_bytes, offset % 512);
    }
    return 0;
}

int KAPI FindFile(const char *name, int name_len, uint32_t *length) {
    if (name_len > 10)
        name_len = 10;
    for (int i = 0; i < MAX_FAT_ENTRIES; i++) {
        if (files[i].length > 0 &&
            0 == KeMemoryCompare(name, files[i].name, name_len)) {
            if (length != 0)
                *length = files[i].length;
            return i;
        }
    }

    return -1;
}
