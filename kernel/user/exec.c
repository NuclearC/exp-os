

#include "exec.h"
#include "elf_loader.h"
#include "memory/memory.h"
#include "modules/fs/filesystem.h"


int KAPI KeUserExecuteFile(const char* filename) {

    int fileindex = FsFindFile(filename);

    if (fileindex == -1) {
        return USER_EXEC_FILE_INVALID;
    }

    FileHandle * handle = FsOpenFile(fileindex);

    Elf32_Ehdr elf_header;
    FsReadBytes(handle, 0, (char*)&elf_header, sizeof(Elf32_Ehdr)); 

    if (KeCheckElfHeader(&elf_header)) {
        return USER_EXEC_HDR_INVALID;
    }

    Elf32_Phdr program_header;
    for (int i = 0; i < elf_header.e_phnum; i++) {
        FsReadBytes(handle, elf_header.e_phoff + i * elf_header.e_phentsize, (char*)&program_header, sizeof(Elf32_Phdr));

        if (program_header.p_type == PT_LOAD) {
            /* load into memory */
            /* PageDirectoryEntry32 entry;
            KeAllocatePageDirectores(&entry, 1, 0);
            */
        }
    }

    return 0;
}

int KAPI KeUserExecute() {

    return 0;
}

