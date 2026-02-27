

#include "exec.h"
#include "elf/elf_loader.h"
#include "kernel/diag/print.h"
#include "memory/memory.h"
#include "memory/paging.h"
#include "modules/fs/filesystem.h"

extern void KNORETURN _user_jmp(uintptr_t addr, uintptr_t stack_top);

int KAPI KeUserExecuteFile(const char *filename) {
    /* find and load the ELF file from the filename */
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
    /* no specific reason */
    uintptr_t image_base = 0x5f000000;

    Elf32_Phdr program_header;
    for (int i = 0; i < elf_header.e_phnum; i++) {
        FsReadBytes(handle, elf_header.e_phoff + i * elf_header.e_phentsize,
                    (char *)&program_header, sizeof(Elf32_Phdr));

        if (program_header.p_type == PT_LOAD) {
            /* load into memory */
            KePrint("loadable section %x %x %x \n", program_header.p_vaddr,
                    program_header.p_paddr, program_header.p_memsz);

            uintptr_t ph_vbase = image_base + program_header.p_vaddr;
            uintptr_t ph_memory = (uintptr_t)KeAllocatePhysicalMemory(
                program_header.p_memsz, PAGE_ALIGN);

            int page_flags = PAGE_ACCESS_ALL | PAGE_READ_WRITE;

            KeMapPageTables(ph_memory, program_header.p_memsz, ph_vbase,
                            page_flags);
            FsReadBytes(handle, program_header.p_offset, (void *)ph_vbase,
                        program_header.p_filesz);
        } else if (program_header.p_type == PT_DYNAMIC) {
            KePrint("dynamic section %x %x %x \n", program_header.p_vaddr,
                    program_header.p_paddr, program_header.p_memsz);

            KeDynamicLink(image_base, &program_header);
        }
    }

    const size_t user_stack_size = 0x10000;
    uintptr_t stack_top = image_base - 0x1000;
    uintptr_t image_stack =
        (uintptr_t)KeAllocatePhysicalMemory(user_stack_size, PAGE_ALIGN);
    KeMapPageTables(image_stack, user_stack_size, stack_top - user_stack_size,
                    PAGE_ACCESS_ALL | PAGE_READ_WRITE);

    uintptr_t entry = image_base + elf_header.e_entry;
    KePrint("usermode jump to %x \n", entry);
    /* perform the usermode jump */
    KeUserJump(entry, stack_top);
}

void KNORETURN KAPI KeUserJump(uintptr_t addr, uintptr_t stack_top) {
    _user_jmp(addr, stack_top);
}
