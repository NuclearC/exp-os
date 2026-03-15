

#include "main.h"
#include <elf.h>

typedef struct PACK {
    uint16_t sector;
    uint32_t length;
    char name[10];
} FileEntry;

static FileEntry file_entries[32];
static void Print(const char *str, uint8_t color);
static void LoadFile(int index);

static int cur_row = 0, cur_col = 0;

NORET int LdrMain(void) {
    Print("hello from OS loader\n", VCOL);
    /* TODO: load some necessary modules (VGA, Disk I/O) */
    /* Load the kernel */

    _io_read_sector(file_entries, 1, 1);

    int krnl_index = -1;
    for (int i = 0; i < 32; i++) {
        if (file_entries[i].length != 0) {
            Print("file: ", VCOL);
            Print(file_entries[i].name, VCOL);
            Print("\n", 0);

            if (_s_strcmp(file_entries[i].name, "kernel", 6) == 0) {
                krnl_index = i;
            }
        }
    }

    if (krnl_index != -1) {
        Print("loading the kernel\n", VCOL);
        LoadFile(krnl_index);
    } else {
        Print("no kernel present on the 'filesystem'\n", VCOL);
    }
    while (1)
        ;
}

static void LoadFile(int index) {
    Elf64_Ehdr hdr;

    _io_read_bytes(&hdr, file_entries[index].sector, sizeof(hdr), 0);

    if (hdr.e_ident[EI_MAG0] != ELFMAG0 || hdr.e_ident[EI_MAG1] != ELFMAG1 ||
        hdr.e_ident[EI_MAG2] != ELFMAG2 || hdr.e_ident[EI_MAG3] != ELFMAG3) {
        Print("invalid e_ident for kernel\n", VERR);
        return;
    }

    if (hdr.e_ident[EI_CLASS] != ELFCLASS64) {
        Print("invalid ELF class for kernel\n", VERR);
        return;
    }

    if (hdr.e_ident[EI_DATA] != ELFDATA2LSB) {
        Print("wtf? big endian even? whats this\n", VERR);
        return;
    }
    if (hdr.e_type != ET_EXEC) {
        Print("invalid ELF type for kernel\n", VERR);
        return;
    }
    if (hdr.e_machine != EM_X86_64) {
        Print("invalid architecture for kernel\n", VERR);
        return;
    }

    Print("Loading the program tables...\n", VCOL);

    for (Elf64_Half i = 0; i < hdr.e_phnum; i++) {
        Elf64_Phdr phdr;

        uint64_t sector = file_entries[index].sector +
                          (hdr.e_phoff + i * hdr.e_phentsize) / 512;
        uint64_t skip = (hdr.e_phoff + i * hdr.e_phentsize) % 512;
        _io_read_bytes(&phdr, sector, sizeof(phdr), skip);
    }
}

static void Print(const char *str, uint8_t color) {
    char *base = (char *)VBASE;

    while (*str != (char)0) {
        if (*str == '\n') {
            cur_row += 1;
            cur_col = 0;
        } else {
            base[2 * (cur_row * VWIDTH + cur_col)] = *str;
            base[1 + 2 * (cur_row * VWIDTH + cur_col)] = color;

            cur_col += 1;
            if (cur_col >= VWIDTH) {
                cur_row += 1;
                cur_col %= VWIDTH;
            }
        }

        str++;

        if (cur_row >= VHEIGHT) {
            for (int i = cur_row - 1; i >= 1; i--) {
                for (int j = 0; j < VWIDTH; j++) {
                    *((uint16_t *)(base) + (i - 1) * VWIDTH + j) =
                        *((uint16_t *)(base) + i * VWIDTH + j);
                }
            }

            cur_row -= 1;
        }
    }

    _v_cursor(cur_row * VWIDTH + cur_col);
}
