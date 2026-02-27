
%include "util/seg.asm"

bits 32
section .text

global _isr_syscall
_isr_syscall:
    pushad ; save the state to the stack

    mov edx, esp
    push ds
    push es
    push fs
    push gs

    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax    

    push edx
    extern KeSyscall
    call KeSyscall 
    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds
    popad

    iret

