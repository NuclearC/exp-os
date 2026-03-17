

#include "main.h"
#include <elf.h>

typedef struct PACK {
    uint16_t sector;
    uint32_t length;
    char name[10];
} FileEntry;

typedef struct PACK {
    uint32_t base_low;
    uint32_t base_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
    uint32_t acpi3_attr;
} PhysicalMemoryMapEntry;

typedef struct PACK {
    uint32_t count;
    PhysicalMemoryMapEntry *entries;
} PhysicalMemoryMap;

static FileEntry file_entries[32];
static void Print(const char *str, uint8_t color);
static void PrintHex(uint64_t hex, int len, uint8_t color);
static void PrintChar(const char ch, uint8_t color);
static void LoadFile(int index);

static int cur_row = 0, cur_col = 0;

NORET int LdrMain(PhysicalMemoryMap const *memory_map) {
    PrintHex((uint64_t)memory_map, 16, VCOL);
    Print(" hello from OS loader\n", VCOL);
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

    for (uint32_t i = 0; i < memory_map->count; i++) {
        PhysicalMemoryMapEntry *entry = &memory_map->entries[i];
        Print("Memory region ", VCOL);
        PrintHex(entry->base_high, 8, VCOL2);
        Print(":", VCOL2);
        PrintHex(entry->base_low, 8, VCOL2);
        Print(" ", VCOL);

        PrintHex(entry->length_high, 8, VCOL2);
        Print(":", VCOL2);
        PrintHex(entry->length_low, 8, VCOL2);
        Print(" ", VCOL);
        PrintHex(entry->type, 8, VCOL);
        Print("\n", VCOL);
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

    Print("Loading the program tables... ", VCOL);
    PrintHex(hdr.e_phnum, 4, VCOL);
    Print("\n", VCOL);
    Elf64_Phdr phdr;

    for (Elf64_Half i = 0; i < hdr.e_phnum; i++) {
        for (int i = 0; i < 100000000; i++)
            ;
        uint64_t sector = file_entries[index].sector +
                          (hdr.e_phoff + i * hdr.e_phentsize) / 512;
        uint64_t skip = (hdr.e_phoff + i * hdr.e_phentsize) % 512;

        Print("reading from ", VCOL);
        PrintHex(i, 4, 0x14);
        Print(" ", VCOL);
        PrintHex(sector, 4, VCOL2);
        Print(" ", VCOL);
        PrintHex(skip, 4, VCOL2);
        Print(" ", VCOL);
        uint64_t red = _io_read_bytes(&phdr, sector, hdr.e_phentsize, skip);
        Print("read this many: ", VCOL);
        PrintHex(red, 6, VCOL);
        Print("\n", VCOL);
    }

    Print("kernel loaded!", VCOL);
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
