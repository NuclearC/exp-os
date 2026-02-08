
ELF_HDRSZ       equ 52

; some constants
ELF_IDENT        equ 0      ; e_ident[16]
ELF_TYPE         equ 16     ; e_type (2 bytes)
ELF_MACHINE      equ 18     ; e_machine (2 bytes)
ELF_VERSION      equ 20     ; e_version (4 bytes)
ELF_ENTRY        equ 24     ; e_entry (4 bytes)
ELF_PHOFF        equ 28     ; e_phoff (4 bytes) - program header offset
ELF_SHOFF        equ 32     ; e_shoff (4 bytes) - section header offset
ELF_FLAGS        equ 36     ; e_flags (4 bytes)
ELF_EHSIZE       equ 40     ; e_ehsize (2 bytes)
ELF_PHENTSIZE    equ 42     ; e_phentsize (2 bytes)
ELF_PHNUM        equ 44     ; e_phnum (2 bytes)
ELF_SHENTSIZE    equ 46     ; e_shentsize (2 bytes)
ELF_SHNUM        equ 48     ; e_shnum (2 bytes)
ELF_SHSTRNDX     equ 50     ; e_shstrndx (2 bytes)

; ELF Magic bytes
ELF_MAGIC_0      equ 0x7F
ELF_MAGIC_1      equ 0x45   ; 'E'
ELF_MAGIC_2      equ 0x4C   ; 'L'
ELF_MAGIC_3      equ 0x46   ; 'F'

; ELF Classes
ELFCLASS32       equ 1
ELFCLASS64       equ 2

; ELF Data encoding
ELFDATA2LSB      equ 1      ; Little endian
ELFDATA2MSB      equ 2      ; Big endian

; ELF Types
ET_NONE          equ 0
ET_REL           equ 1      ; Relocatable
ET_EXEC          equ 2      ; Executable
ET_DYN           equ 3      ; Shared object
ET_CORE          equ 4      ; Core file

; ELF Machine types
EM_NONE          equ 0
EM_386           equ 3      ; Intel 80386
EM_X86_64        equ 62     ; AMD x86-64

; Program Header Types
PT_NULL          equ 0
PT_LOAD          equ 1      ; Loadable segment
PT_DYNAMIC       equ 2      ; Dynamic linking info
PT_INTERP        equ 3      ; Interpreter path
PT_NOTE          equ 4      ; Auxiliary info
PT_SHLIB equ 5
PT_HDR equ 6
PT_TLS equ 7

; Progrram Header Flags
PF_X equ 0x1
PF_W equ 0x2
PF_R equ 0x4

; Program Header offsets
ELF_P_TYPE equ 0x00 ; 4 bytes
ELF_P_OFFSET equ 0x04 ; 4 bytes
ELF_P_VADDR equ 0x08 ; 4 bytes
ELF_P_PADDR equ 0x0C ; 4 bytes
ELF_P_FILESZ equ 0x10 ; 4 bytes
ELF_P_MEMSZ equ 0x14 ; 4 bytes
ELF_P_FLAGS equ 0x18 ; 4 bytes
ELF_P_ALIGN equ 0x1c ; 4 bytes





