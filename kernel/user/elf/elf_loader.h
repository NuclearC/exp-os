
#ifndef NC_TASKS_ELF_LOADER_H_
#define NC_TASKS_ELF_LOADER_H_

#include "ke_main.h"
#include "typedefs.h"

#include <elf.h>

typedef struct {
    int file_index;
    Elf64_Ehdr hdr;
} LoadedFile;

int KAPI LoadElf(int file_index, LoadedFile *loaded_file);
int KAPI LoadProgramHeaders(LoadedFile *loaded_file);

#endif
