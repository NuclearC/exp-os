
; Procedures for IDT

bits 32

%include "util/seg.asm"

section .text

; SUBROUTINE setup the interrupt descriptor table
; Parameters - the address of IDDT
global _idt_setup
_idt_setup:
    enter 0
    mov eax, [ebp + 8]
    lidt [eax]
    leave
    ret

; SUBROUTINE enable hardware interrupts 
global _idt_enable
_idt_enable:
    sti
    ret

