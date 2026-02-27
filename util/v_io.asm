

; VGA I/O subroutines

VGA_BASE equ 0xb8000
VGA_WIDTH equ 80
VGA_HEIGHT equ 25

section .text

; SUBROUTINE - changes the cursor position
; Parameter - the cursor offset in abs. addr
global _v_cursor
_v_cursor:
    enter 0
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
    leave
    ret
    

; SUBROUTINE - prints a string on the screen
; Parameters: the pointer to string (zero-terminated)
;             the row on the screen
;             the column on the screen
;             the color
global _v_print
_v_print:
    enter 0

    mov edx, [ebp + 20] ; the color

    mov ecx, VGA_BASE
    mov eax, [ebp + 16] ; the column
    mul eax, 2
    add ecx, eax

    mov eax, [ebp + 12] ; the row
    mul eax, 2 * VGA_WIDTH
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
        cmp ecx, VGA_BASE + VGA_WIDTH * VGA_HEIGHT * 2
        jge .exit

        add ecx, 2
        jmp .loop
    .exit:
    pop esi
    leave
    ret


; SUBROUTINE - print a character on the screen
; parameters: the char
;             thr row
;             the col
;             the color of the character
global _v_print_char
_v_print_char:
    enter 0
    push ebx
    mov ecx, [ebp + 20] ; the color
    
    mov ebx, VGA_BASE
    mov eax, [ebp + 16] ; the column
    shl eax, 1 ; mul by 2
    add ebx, eax
   
    mov eax, [ebp + 12] ; the row
    mov edx, VGA_WIDTH * 2
    mul edx
    add ebx, eax

    mov eax, [ebp + 8] ; the character
    mov [ebx], eax
    mov [ebx + 1], ecx
    pop ebx
    xor eax, eax
    leave
    ret

; SUBROUTINE - clear the screen
; Parameters - none
global _v_clear
_v_clear:    
    enter 0
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
    leave
    ret
        
    













