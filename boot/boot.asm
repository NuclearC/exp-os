
org 0x7c00
bits 16

start:
    mov ax, cs
    mov ds, ax
    mov ax, 0x090
    mov ss, ax
    mov sp, 0x100
    push dx
boot_scnd:
    mov ah, 2
    mov al, 1 ; number of sectors to read
    mov ch, 0 ; cyllinder number
    mov cl, 2 ; starting sector
    mov dh, 0 ; head number

    mov bx, 0x100 
    mov es, bx
    xor bx, bx

    int 13h ; read the sectors
    jc error ; jump if error
 
    .loop:
        mov dx, es:[bx] ; sector
        mov cx, es:[bx + 2] ; len
        test cx, cx
        jz error ; error because we still havent found it
        cmp bx, 512
        jae error ; still error if we havent found it in sector 2
     
        push dx
        push cx
        add bx, 4 ; pointer += 4 
      
        mov si, boot_loader ; filename
        mov di, bx          ; the pointer
        .loop2:
            mov al, [si]
            inc si
            mov dl, es:[di]
            inc di ; increment the ptr
            cmp al, dl ; al and dl are the characters
            jnz .cmp_uneq ; unequal
    
            ; they are equal
            test al, al
            jz .end_str ; jump if string terminated
            jmp .loop2
        .cmp_uneq:
            add bx, 12
            add sp, 4
            jmp .loop
        .end_str:
    
    ; load the second bootloader
    pop ax 
    xor dx, dx
    add ax, 511
    mov cx, 512
    div cx ; now ax is ceil divided number of sectors 
    pop cx
    pop dx ; from WAAAAAAAAAY above
    
    mov dh, 0
    mov ah, 2
    ; mov al, 1 ; number of sectors to read
    mov ch, 0 ; cyllinder number
    mov dh, 0 ; head number

    mov bx, 0x100
    mov es, bx
    xor bx, bx ; data buffer with es:

    int 13h ; read the sectors
    jc error ; jump if error

    ; now we can jump to that shit
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

boot_loader db "loader", 0
boot_error db "Failed to load second bootloader", 0

times 510-($-$$) db 0
dw 0xaa55

