

bits 64

section .text

%include "string.asm"
%include "print.asm"
%include "disk_io.asm"

global _v_cursor
global _v_print
global _s_strcmp
global _io_read_sector
global _io_read_bytes


; SUBROUTINE - start the operating system kernel. Sets up the stack
;              and jumps to the entry point.
; Parameters -  rdi - the entry point to jump to
;               rsi - stack top
;               rdx - kernel params

global _kernel_start
_kernel_start:
    mov rsp, rsi
    mov rbp, rsi
    
    mov rax, rdi
    mov rdi, rdx

    jmp rax


