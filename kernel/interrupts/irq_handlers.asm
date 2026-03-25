
bits 64

%macro irq_push_ctx 0
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

%macro irq_pop_ctx 0
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

section .text

extern _pic_int_end
extern _pic1_int_end

global _irq_handle_timer
global _irq_handle_keyboard

_irq_handle_keyboard:
    irq_push_ctx

    xor rax, rax
    in al, 60h
    mov rdi, rax
    extern IrqHandleKeyboard
    call IrqHandleKeyboard

    irq_pop_ctx
    call _pic1_int_end
    iretq


_irq_handle_timer:
    irq_push_ctx

    extern IrqHandleTimer
    call IrqHandleTimer

    irq_pop_ctx
    call _pic1_int_end
    iretq

