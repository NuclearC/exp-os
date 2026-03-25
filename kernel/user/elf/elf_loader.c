
#include "elf_loader.h"

#include "memory/memory.h"
#include "memory/paging.h"

#include "modules/filesystem/filesystem.h"

#include <elf.h>

static int CheckElfHeader(Elf64_Ehdr *header) {

    if (header->e_ident[EI_MAG0] != ELFMAG0 ||
        header->e_ident[EI_MAG1] != ELFMAG1 ||
        header->e_ident[EI_MAG2] != ELFMAG2 ||
        header->e_ident[EI_MAG3] != ELFMAG3 ||
        header->e_ident[EI_DATA] != ELFDATA2LSB ||
        header->e_ident[EI_CLASS] != ELFCLASS64) {
        return 0;
    }

    return 1;
}

int KAPI LoadElf(int file_index, LoadedFile *loaded_file) {
    if (sizeof(loaded_file->hdr) !=
        ReadFile(file_index, &loaded_file->hdr, sizeof(Elf64_Ehdr), 0)) {
        return 1;
    }

    if (!CheckElfHeader(&loaded_file->hdr)) {
        return 1;
    }

    loaded_file->file_index = file_index;

    return KSUCCESS;
}

int KAPI LoadProgramHeaders(LoadedFile *loaded_file) {
    Elf64_Phdr program_header;
    for (int i = 0; i < loaded_file->hdr.e_phnum; i++) {

        if (loaded_file->hdr.e_phentsize !=
            ReadFile(loaded_file->file_index, &program_header,
                     sizeof(program_header),
                     loaded_file->hdr.e_phoff +
                         loaded_file->hdr.e_phentsize * i)) {
            return 1;
        }

        if (program_header.p_type == PT_LOAD) {
        } else if (program_header.p_type == PT_DYNAMIC) {
        }
    }

    return KSUCCESS;
}
