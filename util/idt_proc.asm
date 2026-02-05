
; Procedures for IDT


bits 32
; SUBROUTINE Load IDT
; Parameters - IDT as 6 bytes on the stack
global _idt_load
_idt_load:
    push ebp
    mov ebp, esp
    lidt [ebp + 8]
    pop ebp
    ret

; SUBROUTINE enable hardware interrupts
; Parameters - none
global _idt_enable_hw
_idt_enable_hw:
    sti
    ret
