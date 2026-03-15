
bits 64

; SUBROUTINE: strcmp
; Parameters: rsi - the first string
;             rdi - the second string
;             rdx - the max length to check
; Return value: rax - 0 if equal up to the null terminator
; Modifies: rax, rdi, rdx
_s_strcmp:
    push rbp
    mov rbp, rsp

    xor rax, rax 
    .loop:
        lodsb ; load from RSI
        cmp al, [rdi]
        jnz .uneq ; not equal, return
        test al, al ; check for both equal to 0, terminate then
        jz .exit
        xor rax, rax
        inc rdi ; otherwise, increment the pointer
        dec rdx
        jz .exit
        jmp .loop ; loop back
    .uneq:
        mov rax, 1 
    .exit:
    
    pop rbp
    ret






