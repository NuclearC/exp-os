

; VGA I/O subroutines

VGA_BASE equ 0xb8000
VGA_WIDTH equ 80
VGA_HEIGHT equ 25

section .text

; SUBROUTINE - prints a string on the screen
; Parameters: the pointer to string (zero-terminated)
;             the row on the screen
;             the column on the screen
global _v_print
_v_print:
    push ebp
    mov ebp, esp

    mov ecx, VGA_BASE
    mov eax, [ebp + 16] ; the column
    imul eax, 2
    add ecx, eax

    mov eax, [ebp + 12] ; the row
    imul eax, 2 * VGA_WIDTH
    add ecx, eax

    push esi
    mov esi, [ebp + 8] ; the string

    xor eax, eax
    cld        
    .loop:
        lodsb
        test al, al
        jz .exit
        mov [ecx], al
        mov [ecx + 1], 03h
        add ecx, 2
        jmp .loop
    .exit:
    pop esi
    pop ebp
    ret

; SUBROUTINE - clear the screen
; Parameters - none
global _v_clear
_v_clear:    
    push eax
    push ecx
    mov ecx, VGA_HEIGHT*VGA_WIDTH*2
    mov eax, VGA_BASE 
    .loop:
        mov [eax], 0x0000
        add eax, 2
        sub ecx, 2
        test ecx, ecx
        jnz .loop
    pop ecx
    pop eax
    ret
        
    













