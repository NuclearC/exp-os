
bits 32

global _test


section .text

_test:
    mov eax, 1
    mov ebx, test
    mov edx, 0x05
    int 80h

    mov eax, 1
    mov ebx, test2
    mov edx, 0x04
    int 80h

    xor eax, eax
    ret

section .data
    
test: db "hello", 10, 0
test2: db "wtf", 10, 0


