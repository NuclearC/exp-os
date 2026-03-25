
bits 64

section .text

global _pg_exec_check
_pg_exec_check:
    mov rcx, 0xC0000080 ; EFER
    rdmsr
    or rax, (1 << 11) 
    wrmsr
    ret

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

