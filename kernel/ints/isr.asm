
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

global _isr_pf
_isr_pf:
    int_begin

    push esp ; first arg: pointer to everything
    extern _IsrPFault
    call _IsrPFault
    add esp, 4 ; pop
    
    int_end 
    add esp, 4 ; pop the error code
    iret

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

global _isr_syscall
_isr_syscall:
    int_begin
    push esp
    extern _IsrSyscall
    call _IsrSyscall
    add esp, 4
    int_end
    iret

