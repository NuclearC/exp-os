
; Procedures for IDT

bits 32

%include "util/seg.asm"

section .text

; SUBROUTINE setup the interrupt descriptor table
; Parameters none
global _idt_setup
_idt_setup:
    lidt [idt_desc]
    mov eax, idt
    ret

; SUBROUTINE get the address of the IDT
global _idt_addr
_idt_addr:
    mov eax, idt
    ret

; SUBROUTINE enable hardware interrupts 
global _idt_enable
_idt_enable:
    sti
    ret

section .data
align 8
idt:
    times 256 dq 0 
idt_end:
idt_desc:
    dw idt_end - idt - 1
    dd idt

