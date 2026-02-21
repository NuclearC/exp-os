
bits 32

global _test

_test:

    mov eax, 1
    int 80h

    xor eax, eax
    ret



