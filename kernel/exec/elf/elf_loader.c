
#include "elf_loader.h"

#include "exec/exec.h"
#include "memory/memory.h"
#include "memory/paging.h"

#include "modules/filesystem/filesystem.h"

#include <elf.h>
#include <stdlib.h>

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

int KAPI LoadProgramHeaders(LoadedFile *loaded_file, uintptr_t base_address,
                            int supervisor_mode,
                            MappedExecSubImage **sub_images,
                            int *sub_image_count) {

    *sub_image_count = 0;
    /* VLA? fuck them */
    MappedExecSubImage tmp_sub_images[16];

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

            size_t prog_size = ALIGN(program_header.p_memsz, PG_ALIGN);
            void *memory = AllocatePhysicalMemory(prog_size, PG_ALIGN);
            if (memory != MM_ERR_ALLOC_FAILED) {
                return 1;
            }

            int page_flags = 0;
            if (program_header.p_flags & PF_X) {
                page_flags |= PAGE_EXECUTE;
            }
            if (program_header.p_flags & PF_W) {
                page_flags |= PAGE_READWRITE;
            }
            if (supervisor_mode == 0) {
                page_flags |= PAGE_ACCESSALL;
            }

            uintptr_t prog_base = program_header.p_vaddr + base_address;
            if (KSUCCESS != KeAllocateMemory((uintptr_t)memory, prog_size,
                                             prog_base, page_flags)) {
                return 1;
            }

            KeZeroMemory((void *)prog_base, prog_size);

            ReadFile(loaded_file->file_index, (void *)prog_base,
                     program_header.p_filesz, program_header.p_offset);

            MappedExecSubImage sub_image;
            sub_image.length = prog_size;
            sub_image.page_addr = prog_base;
            sub_image.page_flags = page_flags;
            sub_image.real_addr = (uintptr_t)memory;

            tmp_sub_images[(*sub_image_count)++] = sub_image;
        } else if (program_header.p_type == PT_DYNAMIC) {
        }
    }

    *sub_images = AllocatePhysicalMemory(
        sizeof(MappedExecSubImage) * (*sub_image_count), MEM_ALIGN);
    KeMemoryCopy(*sub_images, tmp_sub_images,
                 sizeof(MappedExecSubImage) * (*sub_image_count));

    return KSUCCESS;
}
