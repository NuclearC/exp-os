
bits 64

section .text

; SUBROUTINE - setup syscalls
; Parameters -  rdi - syscall entry for kernel
;               rsi - corresponding segments (0-31: EIP, 32-47: Kernel CS, 48-63: User CS)
global _sys_setup
_sys_setup:
    mov rcx, 0xC0000080 ; EFER
    rdmsr
    or rax, 0x1 ; enable syscall/sysret
    wrmsr

    inc rcx ; STAR
    rdmsr
    mov rax, rsi
    wdmsr

    inc rcx ; LSTAR
    rdmsr
    mov rax, rdi
    wdmsr
    ret

; SUBROUTINE - kernel syscall entry
global _sys_entry
_sys_entry:
    ret




