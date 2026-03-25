
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
    wrmsr

    inc rcx ; LSTAR
    rdmsr
    mov rax, rdi
    wrmsr

    ret

; SUBROUTINE - privilege jump
; Parameters -  rdi - address of jump
;               rsi - jump context
global _sys_exit
_sys_exit:
    mov r11, [rsi]

    ; Preserve registers
    mov rbx, [rsi + 8]
    mov rsp, [rsi + 16]
    mov rbp, [rsi + 24]
    mov r12, [rsi + 32]
    mov r13, [rsi + 40]
    mov r14, [rsi + 48]
    mov r15, [rsi + 56]

    mov rcx, rdi
    sysret


; SUBROUTINE - kernel syscall entry
global _sys_entry
_sys_entry:
    mov rax, rsp

    push r15
    push r14
    push r13
    push r12
    push rbp
    push rax
    push rbx 
    push r11 ; flags

    mov rdi, rcx
    mov rsi, rsp
    extern PrivilegeJumpEntry
    jmp PrivilegeJumpEntry





