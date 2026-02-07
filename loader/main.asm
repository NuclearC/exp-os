org 0x1000
bits 16

CODE_SEG equ 08h
DATA_SEG equ 10h
KERNEL_BASE equ 100000h

section .text
start:
    cli ; clear interrupts 
    ; clear the segment registers 
    xor ax, ax
    mov ds, ax
    ; enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al
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
    ; setup the PIC
    call setup_PIC
    ; now we are in the protected mode, we can load the necessary module
    jmp ldr_entry

align 4
test_int:
    pusha
    call _v_clear
    mov eax, 0x0f
    push eax
    mov eax, 0x00
    push eax
    push eax
    mov eax, interrupt_st
    push eax
    call _v_print    
    add esp, 16
    popa
    iret

PIC1 equ 0x20
PIC2 equ 0xA0
PIC1_CMD equ PIC1
PIC1_DATA equ PIC1+1
PIC2_CMD equ PIC2
PIC2_DATA equ PIC2+1
; sets up the Programmable Interrupt Controller
setup_PIC:
    ; save the initial masks
    in al, PIC1_DATA
    mov cl, al
    in al, PIC2_DATA
    mov ch, al 

    mov al, 0x11
    out PIC1_CMD, al
    out PIC2_CMD, al

    mov al, 0x20
    out PIC1_DATA, al
    mov al, 0x28
    out PIC2_DATA, al

    mov al, 0x04
    out PIC1_DATA, al
    mov al, 0x02
    out PIC2_DATA, al

    mov al, 0x01
    out PIC1_DATA, al
    out PIC2_DATA, al

    ; restore masks
    mov al, cl
    or al, 0xff
    out PIC1_DATA, al
    mov al, ch
    or al, 0xff
    out PIC2_DATA, al   

    ret

; Protected Mode entry point
ldr_entry:  
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
    mov ebx, eax ; EBX = the sector number
    mov eax, edx
    xor edx, edx
    add eax, 511 ; ceil up division
    mov ecx, 512 
    div ecx ; ECX = the number of sectors of kernel
    mov ecx, eax 
    mov edi, KERNEL_BASE 
    .loop:
        push ecx
        push ebx ; argument sector number
        push edi ; argument destination buffer
        call _io_read_sector
        add esp, 8 ; clean up the stack
        add edi, 512 ; increment the pointer
        pop ecx
        inc ebx
        loop .loop

    jmp CODE_SEG:KERNEL_BASE

%include "disk_io.asm"
%include "v_io.asm"

section .data
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
    dw gdt_end - gdt - 1
    dd gdt
kernel_str db "kernel", 0
interrupt_st db "unhandled interrupt", 0

