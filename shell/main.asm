
bits 32

global _start

_start:

    mov eax, 1
    int 80h

    xor eax, eax
    ret



