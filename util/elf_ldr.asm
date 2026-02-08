
bits 32

section .text
; subroutines for working with ELF files
%include "elf_def.asm"
; SUBROUTINE validate ELF header
; Parameters - pointer to ELF header
global _elf_valid
_elf_valid:
    enter 0
    push esi
    mov esi, [ebp + 8] ; param 1 - ptr to elf file
    mov al, [esi]
    cmp al, ELF_MAGIC_0
    jne .invalid

    mov al, [esi + 1]
    cmp al, ELF_MAGIC_1
    jne .invalid
    
    mov al, [esi + 2]
    cmp al, ELF_MAGIC_2
    jne .invalid
    
    mov al, [esi + 3]
    cmp al, ELF_MAGIC_3
    jne .invalid

    ; FUCK BIG ENDIAN all my homies hate it
    mov al, [esi + 5]
    cmp al, ELFDATA2LSB
    jne .invalid

    xor eax, eax
    pop esi
    leave
    ret    

    .invalid:
    mov eax, 1
    pop esi
    leave
    ret

; SUBROUTINE parse ELF header
; Parameters - pointer to ELF file
; Return value - the entry point AFTER the program had been loaded or -1 if failed
global _elf_parse
_elf_parse:
    enter 0
    push esi

    mov esi, [ebp + 8] ; Param 1 - pointer to elf file

    ; verify the magic number 
    push esi
    call _elf_valid
    add esp, 4
    test eax, eax
    jnz .invalid
    
    ; we only support little endian and x86
    mov ax, [esi + ELF_MACHINE]
    cmp ax, EM_386
    jne .invalid

    mov eax, [esi + ELF_ENTRY]
    pop esi
    leave
    ret
    .invalid:
    mov eax, -1
    pop esi
    leave
    ret

; SUBROUTINE load program header from a ELF file
; Parameters - pointer to ELF header
;              index of the program header
; Return value - offset to the program header
global _elf_load_ph
_elf_load_ph:     
    enter 0
    mov esi, [ebp + 8] ; param 1 - elf ptr
    mov edx, [ebp + 16] ; param 2 - index
    mov eax, [esi + ELF_PHENTSIZE] ; program header size
    mul edx
    add eax, [esi + ELF_PHOFF] ; progrram header offset
    leave
    ret






