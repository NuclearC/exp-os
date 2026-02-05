
org 0x7c00
bits 16

start:
    mov ax, cs
    mov ds, ax

boot_scnd: 
    mov ah, 2
    mov al, 6 ; number of sectors to read
    mov ch, 0 ; cyllinder number
    mov cl, 3 ; starting sector
    mov dh, 0 ; head number

    mov bx, 0x100
    mov es, bx
    xor bx, bx ; data buffer with es:

    int 13h
    jc error 
    jmp 0x100:0

error:
    mov si, boot_error
    cld
    mov ah, 0eh
    .loop:
        lodsb
        or al, al
        jz .end_loop
        int 10h
        jmp .loop

    .end_loop:
        hlt
   
boot_error db "Failed to load second bootloader", 0

times 510-($-$$) db 0
dw 0xaa55

