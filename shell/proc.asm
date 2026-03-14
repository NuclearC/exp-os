
bits 32

global _test


section .text

_test:
    mov eax, 1
    mov ebx, test
    mov edx, 0x05
    int 80h

    xor eax, eax
    ret

section .data
    
test: db "hello from userspace", 10, 0


