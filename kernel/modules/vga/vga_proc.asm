
bits 64
section .text

; SUBROUTINE - enable vga cursor
; Parameters -  rdi - cursor_start
;               rsi - cursor_end
global _vga_cursor_enable
_vga_cursor_enable:
    push rbp
    mov rbp, rsp

    mov r8, rdi
    mov r9, rsi

    mov dx, 0x3d4
    mov al, 0x0a
    out dx, al

    mov dx, 0x3d5
    in al, dx
    and al, 0xc0
    or al, r8b
    out dx, al

    mov dx, 0x3d4
    mov al, 0x0b
    out dx, al

    mov dx, 0x3d5
    in al, dx
    and al, 0xe0
    or al, r9b
    out dx, al

    pop rbp
    ret

; SUBROUTINE - disable vga cursor
; Paremeters - none
global _vga_cursor_disable
_vga_cursor_disable:
    push rbp
    mov rbp, rsp

    mov dx, 0x3d4
    mov al, 0x0a
    out dx, al

    mov dx, 0x3d5
    mov al, 0x20
    out dx, al

    pop rbp
    ret

; SUBROUTINE - set vga cursor
; Parameters - rdi - offset
global _vga_cursor_set
_vga_cursor_set:
    push rbp
    mov rbp, rsp

    mov rcx, rdi

    mov dx, 0x03d4
    mov al, 0x0f
    out dx, al

    inc dx
    mov al, cl
    out dx, al

    dec dx
    mov al, 0x0e
    out dx, al
    
    inc dx
    mov al, ch
    out dx, al 

    pop rbp
    ret

