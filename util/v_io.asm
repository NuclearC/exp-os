

; VGA I/O subroutines

VGA_BASE equ 0xb8000
VGA_WIDTH equ 80
VGA_HEIGHT equ 25

; SUBROUTINE - prints a string on the screen
; Parameters: esi - the pointer to string (zero-terminated)
;             eax - the row on the screen
;             ebx - the column on the screen
v_print:
    push ecx
    mov ecx, VGA_BASE
    imul eax, 2*VGA_WIDTH
    imul ebx, 2
    add ecx, eax
    add ecx, ebx
    cld        
    .loop:
        lodsb
        test al, al
        jz .exit
        mov [ecx], al
        mov [ecx + 1], 0Fh
        add ecx, 2
        jmp .loop
    .exit:
        pop ecx
        ret

; SUBROUTINE - clear the screen
; Parameters - none
v_clear:    
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
        
    













