org 0x1000
bits 16

SMAP_MAGIC equ 0x534D4150 ; 'SMAP'

%include "util/seg.asm"

section .text
start:
    ; load the memory map
    xor ebx, ebx
    mov es, bx ; for loading the memory map
    mov di, memory_map_data
    ; ES:DI points to memory_map_data
    .loop:
        mov edx, SMAP_MAGIC
        mov eax, 0xE820
        mov ecx, 20 ; number of bytes
        int 0x15
        jc .break
        ; we check the return value
        cmp eax, SMAP_MAGIC 
        jne .break

        mov eax, [memory_map_entry_count]
        add al, 1 
        mov [memory_map_entry_count], eax

        add di, 24 
        cmp di, memory_map_data_end
        jae .break

        test ebx, ebx
        jnz .loop

    .break:
    
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
        cmp eax, PT_LOAD
        jne .continue

        mov eax, [edi + ELF_P_FILESZ]
        test eax, eax
        jz .continue

        mov eax, [edi + ELF_P_MEMSZ]
        add eax, [edi + ELF_P_PADDR]
        cmp eax, [kernel_image_end]
        jle .skip2
            mov [kernel_image_end], eax
        .skip2:

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
        cmp eax, [kernel_image_begin]
        ja .skip
            mov [kernel_image_begin], eax
        .skip:

        push eax ; dest buffer
        call _io_read_bytes
        add esp, 16 ; restore the args
        pop esi ; restore this bitch too  

        .continue:
        pop eax
        add esi, eax 
        pop ecx
        loop .ph_load 

    ; TODO setup the paging
    mov eax, page_directory
    mov cr3, eax
    ; enable paging
    mov eax, cr0
    or eax, (1<<31)|1 ; paging
    mov cr0, eax

    mov esp, STACK_BASE
    mov ebp, esp ; reset the stack
    
    push kernel_params ; kernel params
    ; jump to KeMain
    call ebx 
    .loop: ; wtf?
        hlt
        jmp .loop

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

kernel_params:
    dd memory_map
    dd kernel_image

kernel_image:
kernel_image_begin: dd 0xffffffff
kernel_image_end:   dd 0
kernel_stack_begin: dd 0
kernel_stack_end:   dd KERNEL_PBASE + STACK_SIZE

memory_map:
memory_map_entry_count:
    dd 0
memory_map_entry_pointer:
    dd memory_map_data

memory_map_data:
    times 192 dd 0
memory_map_data_end:

align 4096
page_directory:
    dd page_table_0 + 0x003 ; Entry 0: identity map via page table
    times 767 dd 0
    dd page_table_1 + 0x003 ; Entry 768: higher-half via same page table
    times 255 dd 0 

; Identity map: from 0 to 1024 * 4096 (first 4MB)
page_table_0:
    %assign i 0
    %rep 1024
        dd (i << 12) | 0x003    ; Map 4KB pages (present, writable)
        %assign i i+1
    %endrep 

; Kernel map: 0xc0000000 -> 0x00100000
page_table_1:
    %assign i 0
    %rep 1024
        dd ((i << 12) + 0x100000) | 0x003    ; Map 4KB pages (present, writable)
        %assign i i+1
    %endrep 

gdt:
    .gdt_null:
        dq 0
    .gdt_su_code:
        dw 0FFFFh
        dw 0
        db 0
        db 10011010b
        db 11001111b
        db 0
    .gdt_su_data:
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
error_str db "Failed to load the kernel -- ", 0
