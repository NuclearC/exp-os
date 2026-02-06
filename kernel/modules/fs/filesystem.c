

#include "filesystem.h"
#include "typedefs.h"
#include "memory.h"
#include "ke_main.h"

extern int _s_strcmp(const char* s1, const char* s2);
extern int _io_read_sector(char* buf, int sector);

FileHandle open_handles[MAX_OPEN_FILES];

typedef struct KPACK {
    uint16_t sector;
    uint16_t length;
    char name[12];
} FileTableEntry;

#define FILE_TABLE_SZ 512/16

FileTableEntry file_table[FILE_TABLE_SZ];

int KAPI FsInitialize(void) {
    /* initialize the file system */
    for (int i = 0; i < MAX_OPEN_FILES; ++i) {
        open_handles[i].file_index = FILE_INVALID_INDEX;
    }
    /* load the file table */
    _io_read_sector((char*)file_table, 2); /* we read the second sector */

    return 0; 
}

int KAPI FsFindFile(const char* name) {
    for (int i = 0; i < FILE_TABLE_SZ; ++i) {
        if (file_table[i].length > 0 && _s_strcmp(file_table[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

FileHandle* KAPI FsOpenFile(FileIndex fidx) {
    if (fidx >= FILE_TABLE_SZ && file_table[fidx].length <= 0) return 0;
    for (int i = 0; i < MAX_OPEN_FILES; ++i) {
        if (open_handles[i].file_index == FILE_INVALID_INDEX) {
            open_handles[i].file_index = fidx;
            return &open_handles[i];
        }
    }

    return 0;
}

int KAPI FsReadBytes(FileHandle* handle, char* buffer, size_t nbytes) {
    FileTableEntry* entry = &file_table[handle->file_index];
    uint16_t len = entry->length;
    if (len > nbytes) {
        len = nbytes;
    }
    char buf[512];
    int ret = 0;
    for (uint16_t sec = entry->sector; sec < entry->sector + (len+511)/512; sec++) {
        _io_read_sector(buf, sec);
        int cur = (len > 512) ? 512 : len;
        KeMemoryCopy(buffer + ret, buf, cur);
        ret += cur;
    } 

    return ret;
}

int KAPI FsQuerySize(FileHandle* handle) {
    FileTableEntry* entry = &file_table[handle->file_index];
    return entry->length; 
}    



