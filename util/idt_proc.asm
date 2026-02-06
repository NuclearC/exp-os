
; Procedures for IDT




bits 32

section .text
global _idt_setup
_idt_setup:
    push ebp
    mov ebp, esp
    lidt [idt_desc]
    sti
    mov eax, idt
    pop ebp
    ret

global _idt_addr
_idt_addr:
    mov eax, idt_desc
    ret

section .data
align 8
idt:
    times 256 dq 0 
idt_end:
idt_desc:
    dw idt_end - idt - 1
    dd idt

