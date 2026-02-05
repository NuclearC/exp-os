
#ifndef NC_FILESYSTEM_H_
#define NC_FILESYSTEM_H_

#include "ke_types.h"

typedef uint32_t FileIndex;

#define MAX_OPEN_FILES 128
#define FILE_INVALID_INDEX (uint32_t)-1

typedef struct {
    FileIndex file_index;
} FileHandle;

int FsInitialize(void);
int FsFindFile(const char* name);
FileHandle* FsOpenFile(FileIndex fidx);

int FsReadBytes(FileHandle* handle, char* buffer, size_t nbytes);
int FsQuerySize(FileHandle* handle);

#endif // NC_FILESYSTEM_H_


