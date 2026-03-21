
bits 64


section .text


; SUBROUTINE load global descriptor table from given address
; Parameters    rdi - the address
global _gdt_load
_gdt_load:
    lgdt [rdi]
    ret

; SUBROUTINE load Task State Segment
; Parameters    di - the entry in GDT
global _tss_load
_tss_load:
    mov ax, di
    ltr ax
    ret

