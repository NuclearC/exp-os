
bits 64 

global _test


section .text

_test:
    push rbp
    mov rbp, rsp
    
    mov rax, 1
    mov rdi, 0
    mov rsi, test
    mov rdx, test_end - test
    syscall

    pop rbp
    ret

section .data
    
test: db "hello from userspace", 10, 0
test_end:

