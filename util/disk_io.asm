
bits 32

%ifidn __?OUTPUT_FORMAT?__,bin
%include "string.asm"
%else
extern _s_strcmp
%endif

ATA1 equ 0x1f0
ATA2 equ 0x170

ATA_DATA    equ 0x0
ATA_ERR     equ 0x1
ATA_FT      equ 0x1
ATA_SECCNT  equ 0x2
ATA_SECNUM  equ 0x3
ATA_CYLLOW  equ 0x4
ATA_CYLHI   equ 0x5
ATA_DRHD    equ 0x6
ATA_STAT    equ 0x7
ATA_CMD     equ 0x7

CMD_READ_RETRY  equ 0x20
CMD_READ        equ 0x21

section .text

; SUBROUTINE: initialize the hard drive for reading data
; Params: cl - number of sectors
;         ebx - initial sector
; Modifies: dx, ax      
io_read_init:
    mov dx, ATA1 + ATA_DRHD ; drive and head port
    mov eax, ebx
    shr eax, 24 ; bits 24-27 in al
    or al, 11100000b ; LBA mode
    out dx, al

    mov dx, ATA1 + ATA_SECCNT ; sector count port
    mov al, cl ; number of sectors (PARAM3)
    out dx, al

    mov dx, ATA1 + ATA_SECNUM ; sector number port
    mov eax, ebx     ; bits 0-7
    out dx, al

    mov dx, ATA1 + ATA_CYLLOW ; cyllinder low port
    mov eax, ebx
    shr eax, 8 ; bits 8-15
    out dx, al

    mov dx, ATA1 + ATA_CYLHI ; cyllinder high port
    shr eax, 8 ; bits 16-23
    out dx, al
    
    mov dx, ATA1 + ATA_CMD ; command port
    mov al, CMD_READ_RETRY  ; CMD read with retry
    out dx, al
    ret


; SUBROUTINE: read number of bytes from a hard drive (blocking)
; Parameters: destination buffer
;             the initial sector
;             offset within the sector
;             length of the buffer
; Return value: eax - number of bytes read or 0 if failed
global _io_read_bytes
_io_read_bytes:
    enter 8 
    push edi
    push ebx
    
    mov eax, [ebp + 20] ; length of the buffer
    mov [ebp - 4], eax  ; store it

    add eax, 511 ; ceil divide
    xor edx, edx
    mov ecx, 512
    div ecx ; eax contains the number of sectors
   
    mov cl, al 
    ; the destination buffer 
    mov edi, [ebp + 8] ; the destination buffer
    ; first we initialize the read
    mov ebx, [ebp + 12] ; the initial sector
    call io_read_init

    mov eax, [ebp + 16] ; offset within the sector
    mov [ebp - 8], eax
    
    ; now we can freely use all registers EXCEPT EBX and EDI
    ; convert EBX to words
    xor ebx, ebx
    mov bl, cl ; ebx contains the number of sectors to read 
    .read_sector_loop:
        mov dx, ATA1 + ATA_STAT
        .poll:
            in al, dx
            ; test al, 0x80 ; BSY 
            ; jnz .poll ; this shit hangs wtf?
            test al, 0x08
            jz .poll    
        
        ; we need to check if there is anything to skip within this sector
        mov ecx, [ebp - 8]    
        shr ecx, 1
        test ecx, ecx
        jz .read ; nothing to skip, head to reading part
        ; otherwise we need to skip ecx number of word
        mov dx, ATA1 + ATA_DATA
        .dummy:
            in ax, dx
            loop .dummy
        .read:
            mov ecx, [ebp - 4] ; the stored value 
            cmp ecx, 512 ; max
            jbe .proceed
            mov ecx, 512 ; clamp ecx with 512
            sub ecx, [ebp - 8] ; skip the number of bytes to read
            .proceed: 
                shr ecx, 1 ; divide by 2
                sub [ebp - 4], ecx
                sub [ebp - 4], ecx ; fuck me in the ass
                mov dx, ATA1 + ATA_DATA ; data port
                rep insw

        xor ecx, ecx
        mov [ebp - 8], ecx
 
        dec ebx
        jnz .read_sector_loop
    
    ; read the remaining data
    .dummy2:  
        mov dx, ATA1 + ATA_STAT
        in al, dx
        test al, 0x08 ; drq
        jz .exit
        mov dx, ATA1 + ATA_DATA 
        in ax, dx
        jmp .dummy2
 
    .exit:
    pop ebx
    pop edi
    leave
    ret 

; SUBROUTINE: read number of sectors from a hard drive (blocking)
; Parameters: destination buffer
;             the initial sector
;             the number of sectors
; Return value: eax - number of bytes read or 0 if failed  
global _io_read_sector
_io_read_sector:
    enter 0
    push ebx
    mov ecx, [ebp + 16] ; num sectors
    mov ebx, [ebp + 12] ; initial sectors
    call io_read_init

    mov ebx, ecx ; num sectors  
    push edi 
    mov edi, [ebp + 8] ; the destination buffer
.read_sector_loop:
    mov dx, ATA1 + ATA_STAT
    .poll:
        in al, dx
        test al, 0x80 ; BSY 
        ; jnz .poll ; this shit hangs wtf?
        test al, 0x08
        jz .poll    

    mov ecx, 256
    mov dx, ATA1 + ATA_DATA ; data port
    rep insw
    
    dec ebx
    jnz .read_sector_loop

    pop edi
    pop ebx

    leave
    ret

; SUBROUTINE: find a file by name
; Parameters: the filename string
;             pointer to the Master File Table
; Return value: eax - the sector of the file or -1 if not found
;               edx - the length of the file
global _io_find_file
_io_find_file:
    enter 0
    xor eax, eax
    xor edx, edx
    
    push edi
    mov edi, [ebp + 8]
    mov ebx, [ebp + 12]
    
    .loop:  
        ; the format:
        ; | SECTOR uint16 | LEN uint16 | name 12 bytes |
        mov ax, [ebx]
        mov dx, [ebx + 2]

        test dx, dx
        jz .exit ; len is 0, exit
        
        push eax
        push edx ; store the length
        add ebx, 4 ; p += 4
        push edi
        push ebx
        call _s_strcmp
        add esp, 8 ; clean the stack

        add ebx, 12 ; p += 12
        test eax, eax ; the return value
        pop edx
        pop eax ; restore the sector
        jnz .loop

    .exit:
    pop edi
    leave
    ret









