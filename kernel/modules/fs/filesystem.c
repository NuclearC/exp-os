

#include "filesystem.h"
#include "typedefs.h"
#include "memory.h"
#include "ke_main.h"

#include "print.h"

extern int _s_strcmp(const char* s1, const char* s2);
extern int _io_read_sector(char* buf, int sector, int numsectors);
extern int _io_read_bytes(char* buf, int sectors, int offset, int length); 

typedef struct KPACK {
    uint16_t sector;
    uint16_t length;
    char name[12];
} FileTableEntry;

#define FILE_TABLE_SZ 512/16

FileHandle open_handles[MAX_OPEN_FILES];
FileTableEntry file_table[FILE_TABLE_SZ];

int KAPI FsInitialize(void) {
    /* initialize the file system */
    for (int i = 0; i < MAX_OPEN_FILES; ++i) {
        open_handles[i].file_index = FILE_INVALID_INDEX;
    }
    /* load the file table */
    _io_read_bytes((char*)file_table, 1, 0, 512);

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
    if (fidx >= FILE_TABLE_SZ || file_table[fidx].length <= 0) return 0;
    for (int i = 0; i < MAX_OPEN_FILES; ++i) {
        if (open_handles[i].file_index == FILE_INVALID_INDEX) {
            open_handles[i].file_index = fidx;
            return &open_handles[i];
        }
    }

    return 0;
}

int KAPI FsReadBytes(FileHandle* handle, int offset, char* buffer, size_t nbytes) {
    FileTableEntry* entry = &file_table[handle->file_index];
    uint16_t len = entry->length;
    if (len > nbytes) {
        len = nbytes;
    }

    int secoffset = offset % 512;
    int sector = entry->sector + (offset / 512); 
    return _io_read_bytes(buffer, sector, secoffset, nbytes);
}

int KAPI FsQuerySize(FileHandle* handle) {
    FileTableEntry* entry = &file_table[handle->file_index];
    return entry->length; 
}    



