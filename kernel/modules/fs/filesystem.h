
#ifndef NC_FILESYSTEM_H_
#define NC_FILESYSTEM_H_

#include "typedefs.h"
#include "ke_main.h"

typedef uint32_t FileIndex;

#define MAX_OPEN_FILES 128
#define FILE_INVALID_INDEX (uint32_t)-1

typedef struct {
    FileIndex file_index;
} FileHandle;

int KAPI FsInitialize(void);
int KAPI FsFindFile(const char* name);
FileHandle* KAPI FsOpenFile(FileIndex fidx);

int KAPI FsReadBytes(FileHandle* handle, char* buffer, size_t nbytes);
int KAPI FsQuerySize(FileHandle* handle);

#endif // NC_FILESYSTEM_H_


