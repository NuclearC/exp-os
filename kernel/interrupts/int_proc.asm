
; x86-64 assembly specific procedures for handling interrupts

bits 64

section .text

global _idt_setup
global _idt_enable

; SUBROUTINE - setup the interrupt descriptor table
; Parameters -  rdi - the address of IDT
_idt_setup:
    lidt [rdi]
    ret
; SUBROUTINE - enable hardware interrupts
; Parameters - none
_idt_enable:
    sti
    ret
