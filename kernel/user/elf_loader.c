
#include "elf_loader.h"

int KAPI KeCheckElfHeader(Elf32_Ehdr* header) {

    if (header->e_ident[EI_MAG0] != ELFMAG0 ||
        header->e_ident[EI_MAG1] != ELFMAG1 ||
        header->e_ident[EI_MAG2] != ELFMAG2 ||
        header->e_ident[EI_MAG3] != ELFMAG3 ||
        header->e_ident[EI_DATA] != ELFDATA2LSB ||
        header->e_ident[EI_CLASS] != ELFCLASS32) {
        return 1;
    }

    return 0;
}





