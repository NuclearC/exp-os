
; Some interrupt handlers

bits 32

%include "util/seg.asm"
; for handling PIC irq
extern _pic1_int_end
extern _pic_int_end
; some helper macros to preserve our sanity
%macro int_begin 0
    pushad ; save the state to the stack
    push ds
    push es
    push fs
    push gs

    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
%endmacro

%macro int_end 0
    ; restore the state from stack
    pop gs
    pop fs
    pop es
    pop ds
    popad
%endmacro

global _isr_zero_divide
_isr_zero_divide:
    int_begin
    push esp
    extern _IsrZeroDivide
    call _IsrZeroDivide 
    add esp, 4
    int_end
    add esp, 4
    iret

global _isr_gp
_isr_gp:
    int_begin
    push esp
    extern _IsrGPFault
    call _IsrGPFault
    .loop:
        hlt
        jmp .loop 

global _isr_df
_isr_df:
    int_begin
    push esp
    extern _IsrDFault
    call _IsrDFault
    .loop:
        hlt
        jmp .loop 

global _isr_keyboard
_isr_keyboard:
    int_begin
    extern _IsrKeyboard
    
    xor eax, eax
    in al, 60h ; read the scancode
    push eax
    call _IsrKeyboard 
    add esp, 4 ; clean the stack
    call _pic1_int_end
    int_end
    iret

