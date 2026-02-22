
#ifndef NC_FILESYSTEM_H_
#define NC_FILESYSTEM_H_

#include "kernel/ke_main.h"
#include "kernel/typedefs.h"

typedef struct KPACK {
    uint16_t sector;
    uint16_t length;
    char name[12];
} FileTableEntry;

typedef FileTableEntry *FileHandle;
typedef uint32_t FileIndex;

#define MAX_OPEN_FILES 128
#define FILE_INVALID_INDEX (uint32_t)-1

int KAPI FsInitialize(void);
int KAPI FsFindFile(const char *name);
int KAPI FsOpenFile(int index, FileHandle *handle);

int KAPI FsReadBytes(FileHandle handle, int offset, char *buffer,
                     size_t nbytes);
int KAPI FsQuerySize(FileHandle handle);
#endif // NC_FILESYSTEM_H_
