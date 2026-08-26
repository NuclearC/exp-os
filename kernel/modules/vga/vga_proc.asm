
bits 64
section .text

VGA_MISC_WRITE      equ 0x3C2
VGA_MISC_READ       equ 0x3CC
VGA_SEQ_INDEX       equ 0x3C4
VGA_SEQ_DATA        equ 0x3C5
VGA_CRTC_INDEX      equ 0x3D4   ; (or 0x3B4 if mono)
VGA_CRTC_DATA       equ 0x3D5
VGA_GC_INDEX        equ 0x3CE
VGA_GC_DATA         equ 0x3CF
VGA_AC_INDEX        equ 0x3C0   ; write index+data, read index
VGA_AC_READ         equ 0x3C1
VGA_INSTAT_READ     equ 0x3DA   ; reading this resets AC flip-flop
VGA_DAC_WRITE_INDEX equ 0x3C8
VGA_DAC_DATA        equ 0x3C9

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

