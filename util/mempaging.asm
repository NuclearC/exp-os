
bits 32


; SUBROUTINE - load the page directory
; Parameters - the pointer to page directory
global _mem_pg_load
_mem_pg_load:
    enter 0
    mov eax, [ebp + 8]
    mov cr3, eax
    leave
    ret

global _mem_pg_invld
_mem_pg_invld:
    enter 0
    invlpg [ebp + 8]   
    leave
    ret

global _mem_seg_load
_mem_seg_load:
    enter 0
    mov eax, [ebp + 8]
    lgdt [eax]
    leave
    ret

global _mem_flush_tss
_mem_flush_tss:
    mov ax, (5*8)|0
    ltr ax
    ret

