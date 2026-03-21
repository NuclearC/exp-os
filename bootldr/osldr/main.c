

#include "main.h"
#include "kernel/ke_main.h"
#include "kernel/params.h"

#include <elf.h>
#include <stdint.h>

typedef struct PACK {
    uint16_t sector;
    uint32_t length;
    char name[10];
} FileEntry;

static FileEntry file_entries[32];
static void Print(const char *str, uint8_t color);
static void PrintHex(uint64_t hex, int len, uint8_t color);
static void PrintChar(const char ch, uint8_t color);
static uintptr_t LoadFile(int index);

static int cur_row = 0, cur_col = 0;

NORET int LdrMain(PhysicalMemoryMap const *memory_map) {
    /* TODO: load some necessary modules (VGA, Disk I/O) */
    /* Load the kernel */

    _io_read_bytes(file_entries, 1, sizeof(file_entries), 0);

    int krnl_index = -1;
    for (int i = 0; i < 32; i++) {
        if (file_entries[i].length != 0) {
            if (_s_strcmp(file_entries[i].name, "kernel", 6) == 0) {
                krnl_index = i;
            }
        }
    }

    for (uint32_t i = 0; i < memory_map->count; i++) {
        const PhysicalMemoryMapEntry *entry = &memory_map->entries[i];
        Print("Memory region ", VCOL);
        PrintHex(entry->base, 16, VCOL2);

        Print(" ", VCOL);

        PrintHex(entry->length, 16, VCOL2);
        Print(" ", VCOL);
        PrintHex(entry->type, 8, VCOL);
        Print("\n", VCOL);
    }

    KernelParameters params;
    params.memory_map = memory_map;

    if (krnl_index != -1) {
        uintptr_t kernel_entry = LoadFile(krnl_index);

        Print("Starting the kernel... ", VCOL2);
        PrintHex(kernel_entry, 8, VCOL2);
        /* jump to kernel finally */
        _kernel_start(kernel_entry, KERNEL_STACK_TOP, &params);
    } else {
        Print("no kernel present on the 'filesystem'\n", VCOL);
    }

    while (1)
        ;
}

static uintptr_t LoadFile(int index) {
    Elf64_Ehdr hdr;

    _io_read_bytes(&hdr, file_entries[index].sector, sizeof(hdr), 0);

    if (hdr.e_ident[EI_MAG0] != ELFMAG0 || hdr.e_ident[EI_MAG1] != ELFMAG1 ||
        hdr.e_ident[EI_MAG2] != ELFMAG2 || hdr.e_ident[EI_MAG3] != ELFMAG3) {
        Print("invalid e_ident for executable\n", VERR);
        return 0;
    }

    if (hdr.e_ident[EI_CLASS] != ELFCLASS64) {
        Print("invalid ELF class for executable\n", VERR);
        return 0;
    }

    if (hdr.e_ident[EI_DATA] != ELFDATA2LSB) {
        Print("wtf? big endian even? whats this\n", VERR);
        return 0;
    }
    if (hdr.e_type != ET_EXEC) {
        Print("invalid ELF type for executable\n", VERR);
        return 0;
    }
    if (hdr.e_machine != EM_X86_64) {
        Print("invalid architecture for executable\n", VERR);
        return 0;
    }

    Elf64_Phdr phdr;

    for (Elf64_Half i = 0; i < hdr.e_phnum; i++) {

        uint64_t sector = file_entries[index].sector +
                          (hdr.e_phoff + i * hdr.e_phentsize) / 512;
        uint64_t skip = (hdr.e_phoff + i * hdr.e_phentsize) % 512;

        _io_read_bytes(&phdr, sector, hdr.e_phentsize, skip);

        if (phdr.p_type == PT_LOAD) {
            sector = file_entries[index].sector + phdr.p_offset / 512;
            skip = phdr.p_offset % 512;

            /* zero out memory */
            for (Elf64_Xword j = 0; j < phdr.p_memsz; ++j) {
                *((uint8_t *)phdr.p_paddr + j) = 0;
            }

            _io_read_bytes((void *)phdr.p_paddr, sector,
                           phdr.p_filesz + ((phdr.p_filesz & 1) ? 1 : 0), skip);
        }
    }

    return hdr.e_entry;
}

static void PrintChar(const char ch, uint8_t color) {
    char *base = (char *)VBASE;

    if (ch == '\n') {
        cur_row += 1;
        cur_col = 0;
    } else {
        base[2 * (cur_row * VWIDTH + cur_col)] = ch;
        base[1 + 2 * (cur_row * VWIDTH + cur_col)] = color;

        cur_col += 1;
        if (cur_col >= VWIDTH) {
            cur_row += 1;
            cur_col %= VWIDTH;
        }
    }

    if (cur_row >= VHEIGHT) {
        for (int i = 1; i < VHEIGHT; i++) {
            for (int j = 0; j < VWIDTH; j++) {
                *((uint16_t *)(base) + (i - 1) * VWIDTH + j) =
                    *((uint16_t *)(base) + i * VWIDTH + j);
            }
        }

        cur_row -= 1;
    }
}

static void PrintHex(uint64_t hex, int len, uint8_t color) {
    char buf[17];
    buf[16] = 0;
    int idx = 16;
    while (hex > 0) {
        idx -= 1;
        uint8_t nib = hex & 0xf;
        if (nib < 10) {
            buf[idx] = '0' + nib;
        } else {
            buf[idx] = 'A' + (nib - 10);
        }
        hex >>= 4;
    }

    while (idx > 0 && (16 - idx) < len) {
        buf[--idx] = '0';
    }
    Print(buf + idx, color);
}

static void Print(const char *str, uint8_t color) {
    while (*str != (char)0) {
        PrintChar(*(str++), color);
    }

    _v_cursor(cur_row * VWIDTH + cur_col);
}
