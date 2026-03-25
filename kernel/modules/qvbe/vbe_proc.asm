
bits 64

section .text

VBE_DISPI_IOPORT_INDEX equ 0x01ce
VBE_DISPI_IOPORT_DATA equ 0x01cf

VBE_DISPI_INDEX_ID equ (0)
VBE_DISPI_INDEX_XRES equ (1)
VBE_DISPI_INDEX_YRES equ (2)
VBE_DISPI_INDEX_BPP equ (3)
VBE_DISPI_INDEX_ENABLE equ (4)
VBE_DISPI_INDEX_BANK equ (5)
VBE_DISPI_INDEX_VIRT_WIDTH equ (6)
VBE_DISPI_INDEX_VIRT_HEIGHT equ (7)
VBE_DISPI_INDEX_X_OFFSET equ (8)
VBE_DISPI_INDEX_Y_OFFSET equ (9)

global _vbe_out
; SUBROUTINE - output VBE commands
; Parameters -  rdi - index
;               rsi - data
_vbe_out:
    mov dx, VBE_DISPI_IOPORT_INDEX
    mov ax, di
    out dx, ax

    mov dx, VBE_DISPI_IOPORT_DATA
    mov ax, si
    out dx, ax
    ret

global _vbe_in
; SUBROUTINE - output VBE commands
; Parameters -  rdi - index
; Return value - the read number
_vbe_in:
    mov dx, VBE_DISPI_IOPORT_INDEX
    mov ax, di
    out dx, ax

    xor rax, rax
    mov dx, VBE_DISPI_IOPORT_DATA
    in ax, dx
    ret


