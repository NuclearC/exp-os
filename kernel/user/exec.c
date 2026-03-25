
#include "exec.h"

#include "elf/elf_loader.h"
#include "modules/filesystem/filesystem.h"
#include "tasks/task.h"

int KAPI UserExecuteFile(const char *filename) {
    uint32_t len = 0;
    int file_index = FindFile(filename, 10, &len);
    /* File not found */
    if (file_index == -1) {
        return USX_ERR_NOFILE;
    }

    LoadedFile loaded_file;
    if (KSUCCESS != LoadElf(file_index, &loaded_file)) {
        return USX_ERR_INVLD;
    }

    if (KSUCCESS != LoadProgramHeaders(&loaded_file)) {
        return USX_ERR_INVLD;
    }

    return (int)len;
}
