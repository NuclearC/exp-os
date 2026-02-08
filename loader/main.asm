org 0x1000
bits 16

%include "util/seg.asm"

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
    ; call setup_PIC
    ; now we are in the protected mode, we can load the necessary module
    jmp ldr_entry
; Protected Mode entry point
ldr_entry:  
    ; allocate 512 bytes for the sector
    sub esp, 512
    mov edi, esp ; save destination address    

    mov eax, 1 ; number of sectors    
    push eax 
    mov eax, 1 ; initial sector
    push eax
    push edi
    call _io_read_sector
    add esp, 12 ; clean up the stack   

    push edi ; wtf the filename to find 
    push kernel_str ; the file table pointer
    call _io_find_file
    add esp, 8 ; clean up the stack   

    test eax, eax
    jz error

    ; eax now contains the sector of the kernel
    ; edx contains the length of the kernel in bytes
    ; now we load the elf header

    mov esi, eax
    shl esi, 9 ; esi contains the absolute byte address of the kernel on disk
    sub esp, 4
    mov [ebp], esi
 
    push ELF_HDRSZ
    push 0
    push eax
    push edi 
    call _io_read_bytes
    add esp, 16 ; clean up the stack

    ; edi contains the ptr to elf header 
    push edi
    call _elf_parse
    add esp, 4
    cmp eax, -1
    je error  

    mov ebx, eax ; ebx contains the entry point to kernel
    ; now we load the program headers

    xor eax, eax
    xor ecx, ecx
    mov cx, [edi + ELF_PHNUM]
    mov ax, [edi + ELF_PHENTSIZE]

    add esi, [edi + ELF_PHOFF] ; EAX - pointer to PH Table (local to file)

    ; ECX - number of program headers
    ; EAX - size of program header entry
    ; EBX - entry point to kernel
    ; ESI - absolute address to program header table
    ; EDI - 512 byte buffer
    .ph_load:
        push ecx
        push eax ; number of bytes to read
       
        mov edx, esi
        and edx, (512 - 1) ; offset within file
        push edx
        
        mov edx, esi 
        shr edx, 9 ; divide by 512, get initial sector
        push edx 

        push edi ; the buffer
        call _io_read_bytes
        add esp, 12 ; pop the stack EXCEPT the fucking eax (len of entry)

        ; we have read the PH entry to EDI, now we can do shit
        mov eax, [edi + ELF_P_TYPE]
        test eax, PT_LOAD
        jz .continue

        mov eax, [edi + ELF_P_MEMSZ]
        test eax, eax
        jz .continue 
        mov eax, [edi + ELF_P_FILESZ]
        test eax, eax
        jz .continue
        ; we can load that bitch
        push esi ; store this
        ; now we push the args
        push eax ; number of bytes to read (filesz)
        mov esi, [ebp] ; kernel file offset
        add esi, [edi + ELF_P_OFFSET]
        mov eax, esi
        and eax, (512 - 1)
        push eax ; offset within file
        mov eax, esi
        shr eax, 9
        push eax ; initial sector
        mov eax, [edi + ELF_P_PADDR]
        push eax ; dest buffer
        call _io_read_bytes
        add esp, 16 ; restore the args
        pop esi ; restore this bitch too  

        .continue:
        pop eax
        add esi, eax 
        pop ecx
        loop .ph_load 

    mov esp, STACK_BASE
    mov ebp, esp ; reset the stack
    jmp CODE_SEG:KERNEL_BASE 

error:
    push 0x4
    push 0
    push 0
    push error_str
    call _v_print
    add esp, 16 
    .loop:
        hlt
        jmp .loop

%include "disk_io.asm"
%include "v_io.asm"
%include "elf_ldr.asm"

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
error_str db "Failed to loader the kernel -- ", 0
