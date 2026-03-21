
bits 64

section .text

; SUBROUTINE - load Page tables 
; Parameters -  rdi - the page tables pointer
global _pg_load
_pg_load:
    mov rax, rdi
    mov cr3, rax
    ret

global _pg_invld
_pg_invld:
    invlpg [rdi]
    ret

