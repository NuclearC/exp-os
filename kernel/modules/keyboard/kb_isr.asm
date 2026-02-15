

bits 32
section .text
; for handling PIC irq
extern _pic1_int_end
extern _pic_int_end
;

; ISR keyboard handler
global _isr_keyboard
_isr_keyboard:
    int_begin
    extern _IsrKeyboardHandler
    
    xor eax, eax
    in al, 60h ; read the scancode
    push eax ; push it to the stack
    call _IsrKeyboardHandler
    add esp, 4 ; clean the stack
    call _pic1_int_end ; acknowledge the interrupt
    int_end
    iret

