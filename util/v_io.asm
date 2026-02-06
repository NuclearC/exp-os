

; VGA I/O subroutines

VGA_BASE equ 0xb8000
VGA_WIDTH equ 80
VGA_HEIGHT equ 25

section .text

; SUBROUTINE - changes the cursor position
; Parameter - the cursor offset in abs. addr
global _v_cursor
_v_cursor:
    push ebp
    mov ebp, esp
    push ebx
    mov ebx, [ebp + 8] ; the offset of cursor
    
    mov dx, 0x03d4
    mov al, 0x0f
    out dx, al

    inc dl
    mov al, bl
    out dx, al

    dec dl
    mov al, 0x0e
    out dx, al
    
    inc dl
    mov al, bh
    out dx, al

    pop ebx
    pop ebp
    ret
    

; SUBROUTINE - prints a string on the screen
; Parameters: the pointer to string (zero-terminated)
;             the row on the screen
;             the column on the screen
;             the color
global _v_print
_v_print:
    push ebp
    mov ebp, esp

    mov edx, [ebp + 20] ; the color

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
        mov [ecx + 1], dl
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
        
    













