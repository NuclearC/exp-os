
%ifidn __?OUTPUT_FORMAT?__,bin
%include "string.asm"
%else
extern _s_strcmp
%endif

section .text


; SUBROUTINE: read number of sectors from a hard drive
; Parameters: destination buffer
;             the sector number
; Return value: eax - number of bytes read or 0 if failed  
global _io_read_sector
_io_read_sector:
    enter 0
    
    push ebx
    push edi
    mov edi, [ebp + 8] ; the destination buffer
    mov ecx, [ebp + 12] ; the sector number
    
    mov bl, al

    mov dx, 1f6h ; drive and head port
    mov al, 0a0h ; drive 0 head 0
    out dx, al

    mov dx, 1f2h ; sector count port
    mov al, 1    ; read 1 sector
    out dx, al

    mov dx, 1f3h ; sector number port
    mov eax, ecx    ; read the first sector
    out dx, al

    mov dx, 1f4h ; cyllinder low port
    mov al, 0    ; cyllinder 0
    out dx, al

    mov dx, 1f5h ; cyllinder high port
    mov al, 0    ; rest is 0
    out dx, al
    
    mov dx, 1f7h ; command port
    mov al, 20h  ; CMD read with retry
    out dx, al

    .loop:
        in al, dx
        test al, 8 ; buffer requires servicing
        jz .loop   ; loop otherwise
        
    ; we are ready
    mov ecx, 512 / 2
    mov dx, 1f0h ; data port
    rep insw

    mov eax, 512 ; TODO 
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









