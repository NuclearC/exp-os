

#include "filesystem.h"
#include "ke_main.h"
#include "memory/memory.h"
#include "typedefs.h"

#include "diag/print.h"

extern int _s_strcmp(const char *s1, const char *s2);
extern int _io_read_sector(char *buf, int sector, int numsectors);
extern int _io_read_bytes(char *buf, int sectors, int offset, int length);

#define FILE_TABLE_SZ 512 / 16

static FileTableEntry file_table[FILE_TABLE_SZ];

int KAPI FsInitialize(void) {
    /* load the file table */
    _io_read_bytes((char *)file_table, 1, 0, 512);

    return 0;
}

int KAPI FsFindFile(const char *name) {
    for (int i = 0; i < FILE_TABLE_SZ; ++i) {
        if (file_table[i].length > 0 &&
            _s_strcmp(file_table[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int KAPI FsOpenFile(int index, FileHandle *handle) {
    if (index >= FILE_TABLE_SZ || file_table[index].length <= 0)
        return 0;

    *handle = &file_table[index];
    return 0;
}

int KAPI FsReadBytes(FileHandle handle, int offset, char *buffer,
                     size_t nbytes) {
    FileTableEntry *entry = handle;
    uint16_t len = entry->length;
    if (len > nbytes) {
        len = nbytes;
    }

    int secoffset = offset % 512;
    int sector = entry->sector + (offset / 512);
    return _io_read_bytes(buffer, sector, secoffset, nbytes);
}

int KAPI FsQuerySize(FileHandle handle) {
    FileTableEntry *entry = handle;
    return entry->length;
}
