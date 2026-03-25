
bits 64

section .text

%macro isr_push_ctx 0
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro isr_pop_ctx 0
    pop  r15
    pop  r14
    pop  r13
    pop  r12
    pop  r11
    pop  r10
    pop  r9
    pop  r8
    pop  rdi
    pop  rsi
    pop  rbp
    pop  rbx
    pop  rdx
    pop  rcx
    pop  rax
%endmacro

global _isr_handle_de
; DIVISION error handler
_isr_handle_de:
    isr_push_ctx

    mov rdi, rsp
    jmp $ 

    isr_pop_ctx
    iretq

global _isr_handle_df
; DOUBLE FAULT handler
_isr_handle_df:
    isr_push_ctx
    mov rdi, rsp

    extern IsrHandleDoubleFault
    call IsrHandleDoubleFault

    isr_pop_ctx
    add rsp, 8 ; pop error code
    iretq

global _isr_handle_pf
; PAGE FAULT handler
_isr_handle_pf:
    isr_push_ctx
    
    mov rax, cr2
    push rax 

    mov rdi, rsp

    extern IsrHandlePageFault
    call IsrHandlePageFault

    isr_pop_ctx
    add rsp, 8 ; pop error code
    iretq

global _isr_handle_gpf
; GENERAL PROTECTION FAULT handler
_isr_handle_gpf:
    isr_push_ctx

    mov rdi, rsp

    extern IsrHandleGeneralProtectionFault
    call IsrHandleGeneralProtectionFault

    isr_pop_ctx
    add rsp, 8 ; pop error code
    iretq



