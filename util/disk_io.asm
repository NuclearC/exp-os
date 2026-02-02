
; SUBROUTINE: read number of sectors from a hard drive
; Parameters: edi - destination buffer
;             al - the sector number
; Return value: eax - number of bytes read or 0 if failed  
io_read_sector:
    push edx
    push ebx
    mov bl, al
    push eax

    mov dx, 1f6h ; drive and head port
    mov al, 0a0h ; drive 0 head 0
    out dx, al

    mov dx, 1f2h ; sector count port
    mov al, bl    ; read 1 sector
    out dx, al

    mov dx, 1f3h ; sector number port
    mov al, bl    ; read the first sector
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
    mov cx, 512 / 2
    mov dx, 1f0h ; data port
    rep insw
    
    pop eax
    pop ebx
    pop edx
    ret

