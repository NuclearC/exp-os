
#include "elf_loader.h"
#include "kernel/diag/print.h"

int KAPI KeCheckElfHeader(Elf32_Ehdr *header) {

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

int KAPI KeDynamicLink(uintptr_t image_base, const Elf32_Phdr *header) {
    Elf32_Dyn *dyn = (Elf32_Dyn *)(image_base + header->p_vaddr);
    Elf32_Rel *rel = 0;
    size_t rel_sz = 0;

    for (Elf32_Dyn *d = dyn; d->d_tag != DT_NULL; ++d) {
        switch (d->d_tag) {
        case DT_STRTAB: /* string table */ {

        } break;
        case DT_REL:
            rel = (Elf32_Rel *)(image_base + d->d_un.d_ptr);
            break;
        case DT_RELSZ:
            rel_sz = d->d_un.d_val;
        }
    }

    const size_t rel_c = rel_sz / sizeof(Elf32_Rel);
    for (size_t i = 0; i < rel_c; i++) {
        uint32_t type = ELF32_R_TYPE(rel[i].r_info);
        /* uint32_t sym_idx = ELF32_R_SYM(rela[i].r_info);
         */
        uintptr_t patch_addr = image_base + rel[i].r_offset;

        KePrint("patch %d %x %x \n", type, patch_addr,
                *(uintptr_t *)patch_addr);
        if (type == R_386_RELATIVE) {
            *(uintptr_t *)(patch_addr) += image_base;
        }
    }

    return 0;
}
