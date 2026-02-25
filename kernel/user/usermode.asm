


; SUBROUTINE - jump to usermode
; parameters - the address to jump to
global _user_jmp
_user_jmp:
    ; no stack frame
    mov ecx, [esp + 4]
    mov edx, [esp + 8] ; the stack pointer

    mov ax, (4 * 8) | 3 ; ring 3 segment with RPL 3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax 
    ; ss handled by iret

    push (4 * 8) | 3 ; the data selector for usermode
    push edx
    
    pushf
    push (3 * 8) | 3 ; code selector
    push ecx ; the target address
    iret ; do the jump




