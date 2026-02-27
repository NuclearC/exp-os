

#ifndef NC_USER_ELF_LOADER_H_
#define NC_USER_ELF_LOADER_H_

#include "ke_main.h"
#include "typedefs.h"

#define EI_NIDENT 16

typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef int32_t Elf32_Sword;

typedef struct KPACK {
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;

#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_TLS 7

#define ET_NONE 0
#define ET_REL 1
#define ET_EXEC 2
#define ET_DYN 3
#define ET_CORE 4
#define ET_LOPROC 0xff00
#define ET_HIPROC 0xffff

#define PF_R 0x4
#define PF_W 0x2
#define PF_X 0x1

typedef struct {
    Elf32_Word p_type;
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;

#define EI_MAG0 0 /* e_ident[] indexes */
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_OSABI 7
#define EI_PAD 8

#define ELFMAG0 0x7f /* EI_MAG */
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'
#define ELFMAG "\177ELF"
#define SELFMAG 4

#define ELFCLASSNONE 0 /* EI_CLASS */
#define ELFCLASS32 1
#define ELFCLASS64 2
#define ELFCLASSNUM 3

#define ELFDATANONE 0 /* e_ident[EI_DATA] */
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

#define EV_NONE 0 /* e_version, EI_VERSION */
#define EV_CURRENT 1
#define EV_NUM 2

typedef struct {
    Elf32_Sword d_tag;
    union {
        Elf32_Word d_val;
        Elf32_Addr d_ptr;
    } d_un;
} Elf32_Dyn;

typedef struct {
    Elf32_Addr r_offset;
    Elf32_Word r_info;
} Elf32_Rel;

typedef struct {
    Elf32_Addr r_offset;
    Elf32_Word r_info;
    Elf32_Sword r_addend;
} Elf32_Rela;

#define ELF32_R_SYM(i) ((i) >> 8)
#define ELF32_R_TYPE(i) ((unsigned char)(i))
#define ELF32_R_INFO(s, t) (((s) << 8) + (unsigned char)(t))

/* Termination */
#define DT_NULL 0 /* End of dynamic array */

/* Dependencies */
#define DT_NEEDED 1 /* Name of needed library (strtab offset) */

/* PLT / GOT */
#define DT_PLTRELSZ 2 /* Size of PLT relocation entries */
#define DT_PLTGOT 3   /* Address of PLT/GOT */
#define DT_JMPREL 23  /* Address of PLT relocations */
#define DT_PLTREL 20  /* Type of PLT relocs (DT_REL or DT_RELA) */

/* Symbol & String Tables */
#define DT_STRTAB 5  /* Address of string table */
#define DT_SYMTAB 6  /* Address of symbol table */
#define DT_STRSZ 10  /* Size of string table */
#define DT_SYMENT 11 /* Size of one symbol table entry */

/* Relocations - RELA (with explicit addend) */
#define DT_RELA 7               /* Address of RELA relocation table */
#define DT_RELASZ 8             /* Size of RELA table in bytes */
#define DT_RELAENT 9            /* Size of one RELA entry */
#define DT_RELACOUNT 0x6ffffff9 /* Count of RELATIVE RELA relocations */

/* Relocations - REL (addend implicit, in-place) */
#define DT_REL 17              /* Address of REL relocation table */
#define DT_RELSZ 18            /* Size of REL table in bytes */
#define DT_RELENT 19           /* Size of one REL entry */
#define DT_RELCOUNT 0x6ffffffa /* Count of RELATIVE REL relocations */

/* Hash Tables (for symbol lookup) */
#define DT_HASH 4              /* Address of ELF symbol hash table */
#define DT_GNU_HASH 0x6ffffef5 /* Address of GNU hash table (faster) */

/* Init / Fini */
#define DT_INIT 12            /* Address of init function */
#define DT_FINI 13            /* Address of fini function */
#define DT_INIT_ARRAY 25      /* Address of array of init functions */
#define DT_FINI_ARRAY 26      /* Address of array of fini functions */
#define DT_INIT_ARRAYSZ 27    /* Size of DT_INIT_ARRAY in bytes */
#define DT_FINI_ARRAYSZ 28    /* Size of DT_FINI_ARRAY in bytes */
#define DT_PREINIT_ARRAY 32   /* Array of pre-init functions */
#define DT_PREINIT_ARRAYSZ 33 /* Size of DT_PREINIT_ARRAY */

/* Shared Object Info */
#define DT_SONAME 14  /* Shared object name (strtab offset) */
#define DT_RPATH 15   /* Library search path (deprecated) */
#define DT_RUNPATH 29 /* Library search path (preferred over RPATH) */

/* Flags */
#define DT_SYMBOLIC 16        /* Search SO before exe for symbols (legacy) */
#define DT_FLAGS 30           /* Flags (DF_* values) */
#define DT_FLAGS_1 0x6ffffffb /* Extended flags (DF_1_* values) */
#define DT_BIND_NOW 24        /* Process all relocations at load time */

/* Versioning */
#define DT_VERSYM 0x6ffffff0     /* Address of version symbol table */
#define DT_VERDEF 0x6ffffffc     /* Address of version definition table */
#define DT_VERDEFNUM 0x6ffffffd  /* Number of version definitions */
#define DT_VERNEED 0x6ffffffe    /* Address of version dependency table */
#define DT_VERNEEDNUM 0x6fffffff /* Number of version dependencies */

/* Misc */
#define DT_DEBUG 21    /* Used for debugging; set by dynamic linker */
#define DT_TEXTREL 22  /* Relocations may modify non-writable segment */
#define DT_ENCODING 32 /* Marks start of OS/proc-specific range */

/* DF_FLAGS values (for DT_FLAGS) */
#define DF_ORIGIN 0x1      /* $ORIGIN substitution allowed */
#define DF_SYMBOLIC 0x2    /* Search library before exe */
#define DF_TEXTREL 0x4     /* Relocs may modify read-only segments */
#define DF_BIND_NOW 0x8    /* Resolve all symbols at load time */
#define DF_STATIC_TLS 0x10 /* Uses static TLS */

/* DF_1 flags (for DT_FLAGS_1) */
#define DF_1_NOW 0x00000001       /* Perform complete relocation */
#define DF_1_NODELETE 0x00000008  /* Do not unload on dlclose */
#define DF_1_INITFIRST 0x00000020 /* Init first before others */
#define DF_1_NOOPEN 0x00000040    /* Can't be used with dlopen */
#define DF_1_ORIGIN 0x00000080    /* $ORIGIN processing needed */
#define DF_1_PIE 0x08000000       /* Identifies a PIE executable */

/* i386 relocation types */
#define R_386_NONE 0
#define R_386_32 1    /* S + A */
#define R_386_PC32 2  /* S + A - P */
#define R_386_GOT32 3 /* G + A */
#define R_386_PLT32 4 /* L + A - P */
#define R_386_COPY 5
#define R_386_GLOB_DAT 6  /* S */
#define R_386_JUMP_SLOT 7 /* S */
#define R_386_RELATIVE 8  /* B + A */
#define R_386_GOTOFF 9    /* S + A - GOT */
#define R_386_GOTPC 10    /* GOT + A - P */
#define R_386_TLS_GD 18
#define R_386_TLS_IE 15
#define R_386_TLS_LE 17
#define R_386_TLS_DTPMOD32 35
#define R_386_TLS_DTPOFF32 36
#define R_386_TLS_TPOFF32 37

int KAPI KeCheckElfHeader(Elf32_Ehdr *header);

int KAPI KeDynamicLink(uintptr_t image_base, const Elf32_Phdr *header);

#endif
