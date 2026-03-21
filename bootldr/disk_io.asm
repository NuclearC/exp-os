
bits 64 

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

; SUBROUTINE: initialize the hard drive for reading data
; Params: cl - number of sectors
;         ebx - initial sector
; Modifies: dx, ax      
_io_read_init:
    push rbp
    mov rbp, rsp
    
    mov rdx, ATA1 + ATA_STAT
    .busy_check:
        in al, dx
        test al, 0x80 ; BSY
        jnz .busy_check

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

    pop rbp
    ret

; SUBROUTINE: read number of sectors from a hard drive (blocking)
; Parameters: rdi - destination buffer
;             rsi - the initial sector
;             rdx - the number of sectors
; Return value: rax - number of bytes read or 0 if failed  
_io_read_sector:
    push rbp
    mov rbp, rsp

    push rbx

    push rdx
    mov rcx, rdx
    mov rbx, rsi
    call _io_read_init
    pop rdx

    mov rbx, rdx
    .read_sector_loop:
        mov rdx, ATA1 + ATA_STAT
        .busy_check:
            in al, dx
            test al, 0x80 ; BSY
            ; jnz .busy_check
            test al, 0x08 
            jz .busy_check
        
        mov rcx, 256 ; mul by 256, number of words
        mov rdx, ATA1 + ATA_DATA
        rep insw
        dec rbx
        jnz .read_sector_loop

    pop rbx
    pop rbp
    ret

; SUBROUTINE: read number of sectors from hard drive starting from some offset (blocking)
; Parameters:   rdi - destination buffer
;               rsi - initial sector
;               rdx - number of bytes to read 
;               rcx - offset within first sector
_io_read_bytes:
    push rbp
    mov rbp, rsp

    push rbx

    mov r8, rdx
    mov r9, rcx

    mov rcx, rdx
    add rcx, r9
    add rcx, 511
    shr rcx, 9 ; ceil divide by 512

    mov r10, rcx ; number of sectors 

    mov rbx, rsi
    call _io_read_init
    
    shr r8, 1 ; number of words to read
    shr r9, 1 ; number of words to skip

    xor rbx, rbx ; return value
    .read_sector_loop:
        mov rdx, ATA1 + ATA_STAT
        .busy_check:
            in al, dx
            test al, 0x08 ; data available 
            jz .busy_check
       
        mov rdx, ATA1 + ATA_DATA
        mov rcx, 256
        .read_word_loop:
            in ax, dx ; read the word
            
            cmp rbx, r8 ; nothing left to read
            jae .skip
                
            test r9, r9 ; we gotta skip
            jz .read
            dec r9
            loop .read_word_loop

            .read:
            mov [rdi + 2*rbx], ax ; write to mem location
            inc rbx ; read bytes += 2

            .skip:
            loop .read_word_loop 
        dec r10
        ja .read_sector_loop

    .exit:
    lea rax, [2*rbx] ; return value

    pop rbx
    pop rbp
    ret








