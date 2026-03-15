
bits 64

VGA_TEXT_BASE equ 0x00b8000

; SUBROUTINE - set cursor position in VGA text mode
; Parameters - rdi - the offset
_v_cursor:
    push rbp
    mov rbp, rsp

    mov rcx, rdi
    
    mov dx, 0x03d4
    mov al, 0x0f
    out dx, al

    inc dl
    mov al, cl
    out dx, al

    dec dl
    mov al, 0x0e
    out dx, al
    
    inc dl
    mov al, ch
    out dx, al

    pop rbp
    ret

; SUBROUTINE - print to screen in VGA text mode
; Parameters -  rdi - String to print
;               rsi - offset
;               dl - the color
_v_print:
    push rbp
    mov rbp, rsp
    cld
    mov rax, rsi
    shl rax, 1
    mov rsi, rdi
    mov rdi, VGA_TEXT_BASE
    add rdi, rax
    .loop:
        lodsb
        test al, al
        jz .itsover 
        mov [rdi], al
        mov [rdi + 1], dl        
        add rdi, 2
        jmp .loop

    .itsover:
    pop rbp
    ret

