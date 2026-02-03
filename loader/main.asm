org 0x1000
bits 16

section .text

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
clear_pipe: ; 
    ; clear the segment registers
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ; setup the stack
    mov ebp, 0x9F000
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
KERNEL_BASE equ 100000h

kernel_str db "kernel", 0
test_str db "Found kernel on sector %u", 0

%include "disk_io.asm"
%include "v_io.asm"

; Protected Mode entry point
ldr_entry: ; 
    push ebp
    mov ebp, esp

    call _v_clear
    ; allocate 512 bytes for the sector
    sub esp, 512
    mov edi, esp ; save destination address    
    mov ebx, 2
    push ebx
    push edi
    call _io_read_sector
    add esp, 8 ; clean up the stack   

    push edi ; wtf the filename to find 
    push kernel_str ; the file table pointer
    call _io_find_file
    add esp, 4 ; clean up the stack   

    ; eax now contains the sector of the kernel
    ; edx contains the length of the kernel in bytes
    ; now we load it
    push eax
    mov eax, edx
    xor edx, edx
    add eax, 511 ; ceil up division
    mov ecx, 512
    div ecx
    mov ecx, eax ; number of sectors divided up
    pop eax
    
    mov edi, KERNEL_BASE
    
    .loop:
        push ecx 
        push eax ; argument sector number
        push edi ; argument destination buffer
        call _io_read_sector
        add esp, 8 ; clean up the stack
        pop ecx
        add edi, eax ; read this many bytes
        pop eax
        inc eax
        loop .loop

    jmp 08h:KERNEL_BASE

