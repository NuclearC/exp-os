
bits 32
section .text


; SUBROUTINE: strcmp
; Parameters: the first string
;             the second string
; Return value: eax - 0 if equal up to the null terminator
global _s_strcmp
_s_strcmp:
    enter 0
     
    cld ; for lodsb
    push esi
    mov esi, [ebp + 8] ; the first string
    mov edx, [ebp + 12] ; the second string
    xor eax, eax
    .loop:
        lodsb
        cmp al, [edx]
        jnz .uneq ; not equal, return
        test al, al ; check for both equal to 0, terminate then
        jz .exit
        inc edx ; otherwise, increment the pointer
        jmp .loop ; loop back
    .uneq:
        mov eax, 1 
    .exit:
    pop esi
    leave
    ret






