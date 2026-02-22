

#include "exec.h"
#include "elf_loader.h"
#include "kernel/print.h"
#include "memory/memory.h"
#include "memory/paging.h"
#include "modules/fs/filesystem.h"

int KAPI KeUserExecuteFile(const char *filename) {

    int fileindex = FsFindFile(filename);

    if (fileindex == -1) {
        return USER_EXEC_FILE_INVALID;
    }

    FileHandle handle;
    FsOpenFile(fileindex, &handle);

    Elf32_Ehdr elf_header;
    FsReadBytes(handle, 0, (char *)&elf_header, sizeof(Elf32_Ehdr));

    if (KeCheckElfHeader(&elf_header)) {
        return USER_EXEC_HDR_INVALID;
    }

    void *base = (void *)0x50000000;

    Elf32_Phdr program_header;
    for (int i = 0; i < elf_header.e_phnum; i++) {
        FsReadBytes(handle, elf_header.e_phoff + i * elf_header.e_phentsize,
                    (char *)&program_header, sizeof(Elf32_Phdr));

        if (program_header.p_type == PT_LOAD) {
            /* load into memory */
            KePrint("loadable section %x %x %x \n", program_header.p_vaddr,
                    program_header.p_paddr, program_header.p_memsz);

            void *pbase = base + program_header.p_vaddr;
            void *ptr = KeAllocatePhysicalMemory(program_header.p_memsz, 4096);
            KeAllocatePageTables(ptr, program_header.p_memsz, pbase,
                                 PAGE_ACCESS_ALL | PAGE_READ_WRITE);
            FsReadBytes(handle, program_header.p_offset, pbase,
                        program_header.p_filesz);
        }
    }

    return 0;
}

int KAPI KeUserExecute() { return 0; }
