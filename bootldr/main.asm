org 0x1000
bits 16

SMAP_MAGIC equ 0x534D4150 ; 'SMAP'

; Memory base for writing the page tables
MEM_BASE equ 0x9000

; MEMORY MAP:
;   0x9000 - 0xa000 - PML4
;   0xa000 - 0xb000 - Page Dir Ptr
;   0xb000 - 0xc000 - Page Dir
;   0xc000 - 0xd000 - Page Table
;   0xd000 - 0x10000 - stack after entering long mode
;   0x10000 - 0x11000 - the OS loader

STACK_TOP equ 0x10000
OSLDR_BASE equ 0x10000

; Page flags
PAGE_PRESENT equ 0x1
PAGE_WRITE equ 0x2

section .text

; the entry point
start:
    ; load VGA video modes


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

init_pages:
    mov di, MEM_BASE

    ; zero out this shit
    push di 
    mov ecx, 0x1000 ; 512 qword = 16kb
    xor eax, eax
    cld
    rep stosd
    pop di

    lea eax, [es:di + 0x1000] ; Topmost entry (Page Directory Pointer Table)
    or eax, PAGE_PRESENT | PAGE_WRITE
    mov [es:di], eax ; store the pointer

    lea eax, [es:di + 0x2000] ; mid entry (Page Directory)
    or eax, PAGE_PRESENT | PAGE_WRITE
    mov [es:di + 0x1000], eax

    lea eax, [es:di + 0x3000] ; bottom entry (Page Table)
    or eax, PAGE_PRESENT | PAGE_WRITE
    mov [es:di + 0x2000], eax

    push di
    lea di, [di + 0x3000]
    mov eax, PAGE_PRESENT | PAGE_WRITE

    .page_fill_loop:
        mov [es:di], eax
        add eax, 0x1000 ; points to the next shit
        add di, 8
        cmp eax, 0x200000 ; 2MB identity map check 
        jb .page_fill_loop

    pop di ; our page shit
    ; disable IRQ's
    mov al, 0xff ; the fucking mask
    out 0xa1, al
    out 0x21, al ; both of these fucks deserve this
    nop ; for da pipe 
    nop
    lidt [idt]

enter_protected_mode:
    mov eax, 10100000b ; PAE and PGE bits
    ; tells the processor not to invalidate the TLB entry corresponding to the page 
    ; upon a MOV to CR3 instruction.
    ; Bit 7 (PGE) in CR4 must be set to enable global pages.
    mov cr4, eax

    mov edx, edi
    mov cr3, edx ; the fucking PML4 paging shit (we donnt care about PML5 yet)

    mov ecx, 0xc0000080 ; EFER register 
    rdmsr
    or eax, 0x101 ; Long mode enable and SYSCALL/SYSRET support
    wrmsr

    mov ebx, cr0
    or ebx, 0x80000001 ; enable paging and protection
    mov cr0, ebx
    ; load the Global Descriptor Table 
    cli ; disable interrupts beforehand
    lgdt [gdt_desc]  

    ; Perform far jump
    jmp 08h:clear_pipe

align 0x10
memory_map:
memory_map_entry_count:
    dd 0
memory_map_entry_pointer:
    dd memory_map_data

memory_map_data:
    times 192 dd 0
memory_map_data_end:

gdt:
    .gdt_null:
        dq 0
    .gdt_su_code:
        dq 0x00209A0000000000 ; code exec/read 
    .gdt_su_data:
        dq 0x0000920000000000 ; data read/write 
gdt_end:

align 4
gdt_desc:
    dw gdt_end - gdt - 1
    dd gdt

idt:
    .length dw 0
    .base dd 0

; Long mode starts here
bits 64
clear_pipe: ; 
    ; clear the segment registers
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ; setup the stack
    mov rbp, STACK_TOP 
    mov rsp, rbp
    
; Protected Mode entry point
ldr_entry: 
    ; Blank out the screen to a blue color.
    mov edi, 0xB8000
    mov rcx, 500                      ; Since we are clearing uint64_t over here, we put the count as Count/4.
    mov rax, 0x1F201F201F201F20       ; Set the value to set the screen to: Blue background, white foreground, blank spaces.
    rep stosq                         ; Clear the entire screen. 

    mov rdi, loading_str
    xor rsi, rsi
    mov rdx, 0x1f
    call _v_print

    mov rdi, OSLDR_BASE ; dest
    mov rsi, 1          ; initial sector
    mov rdx, 1          ; num of sectors
    call _io_read_sector
    ; find the required file

    xor rbx, rbx
    xor rcx, rcx

    mov rdi, OSLDR_BASE
    .find_loop:
        mov bx, [rdi] ; the initial sector
        mov ecx, [rdi + 2] ; the length of the file
        test ecx, ecx
        jz .error
        add rdi, 6 ; ptr += 6

        push rdi ; arg1 str1
        mov rsi, loader_name ; str2
        mov rdx, 10 ; max string length to check
        call _s_strcmp
        pop rdi ; pop back rdi

        test rax, rax
        jz .found
        add rdi, 10 ; skip the record 
        jmp .find_loop
    .error:
        mov rdi, failed_str
        mov rsi, 80
        mov dl, 0x1f
        call _v_print
        jmp $

    .found:
   
    mov rdi, OSLDR_BASE ; dest
    mov rsi, rbx        ; initial
    mov rdx, rcx        ; num of sectors
    add rdx, 511
    shr rdx, 9 ; ceil divide
    call _io_read_sector
   
    mov rdi, memory_map 
    jmp OSLDR_BASE 
     

%include "string.asm"
%include "print.asm"
%include "disk_io.asm"

loader_name: db "osldr", 0
loading_str: db "Starting the OSLDR...", 0
failed_str: db "Failed to find the OSLDR", 0
