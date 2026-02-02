org 0x1000
bits 16
start:
    cli ; clear interrupts 
    ; clear the segment registers 
    xor ax, ax
    mov ds, ax
    ; load the Global Descriptor Table 
    lgdt [gdt_desc]    
    ; enable Protected Mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    ; Perform far jump
    jmp 08h:clear_pipe

bits 32
clear_pipe:
    ; clear the segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ; setup the stack
    mov ebp, 0x90000
    mov esp, ebp
    ; now we are in the protected mode, we can load the necessary modules
    jmp ldr_entry

gdt:
    .gdt_null:
        dq 0
    .gdt_code:
        dw 0FFFFh
        dw 0
        db 0
        db 10011010b
        db 11001111b
        db 0
    .gdt_data:
        dw 0FFFFh
        dw 0
        db 0
        db 10010010b
        db 11001111b
        db 0
gdt_end:
gdt_desc:
    dw gdt_end - gdt
    dd gdt
CODE_SEG equ 08h
DATA_SEG equ 10h

; Protected Mode entry point
ldr_entry:
    call v_clear
    mov esi, test_str
    mov eax, 0 ; the row
    mov ebx, 0 ; the column
    call v_print
    mov esi, test_str2
    mov eax, 1
    call v_print
    jmp $    

test_str db "Hello world", 0
test_str2 db "Welcome to some shit", 0

%include "disk_io.asm"
%include "v_io.asm"

