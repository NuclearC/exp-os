

bits 64 


COM1 equ 0x3f8
COM2 equ 0x2f8
COM3 equ 0x3e8
COM4 equ 0x2e8
COM5 equ 0x5f8
COM6 equ 0x4f8
COM7 equ 0x5e8
COM8 equ 0x4e8

RX_BUF equ 0
TX_BUF equ 0
INT_EN equ 1
BAUD_DIV_LOW equ 0
BAUD_DIV_HI equ 1
INT_ID equ 2
FIFO_CTRL equ 2
LINE_CTRL equ 3
MDM_CTRL equ 4
LINE_ST equ 5
MODEM_ST equ 6
SCR_REG equ 7

section .data

com_ports:
dw COM1
dw COM2
dw COM3
dw COM4
dw COM5
dw COM6
dw COM7
dw COM8

section .text
; SUBROUTINE initialize COM ports
; Parameter -   edi - the com port
global _com_init
_com_init:
    push rbp
    mov rsp, rbp

    mov rax, rdi ; com port
    
    xor rcx, rcx
    mov cx, [com_ports + rax] 

    lea rdx, [rcx + INT_EN]
    xor al, al
    out dx, al ; interrupts disable

    lea rdx, [rcx + LINE_CTRL]
    mov al, 0x80
    out dx, al ; enable DLAB

    lea rdx, [rcx + BAUD_DIV_LOW]
    mov al, 0x03
    out dx, al ; Baud rate 38400

    lea rdx, [rcx + BAUD_DIV_HI]
    xor al, al
    out dx, al

    lea rdx, [rcx + LINE_CTRL]
    mov al, 0x03
    out dx, al ; 8 bits, no parity, one stop bit

    lea rdx, [rcx + FIFO_CTRL]
    mov al, 0xc7
    out dx, al ; enable fifo, clear them, with 14 byte threshold

    lea rdx, [rcx + MDM_CTRL]
    mov al, 0x0b
    out dx, al
    mov al, 0x1e
    out dx, al

    ; Test the shit
    lea rdx, [rcx + TX_BUF]
    mov al, 0xAE
    out dx, al

    in al, dx
    cmp al, 0xAE
    jne .fail

    lea rdx, [rcx + MDM_CTRL]
    mov al, 0x0f
    out dx, al
    xor rax, rax
    jmp .skip

    .fail:
    mov rax, 1 
    .skip:

    pop rbp
    ret

; SUBROUTINE - transmit a byte over COM
; Parameters - the com port (u16)
;              the byte (u8)
global _com_write_byte
_com_write_byte:
    push rbp
    mov rbp, rsp

    mov rax, rdi ; com port
    xor rcx, rcx
    mov cx, [com_ports + rax]

    lea rdx, [rcx + LINE_ST]
    .wait:
        in al, dx
        test al, 0x20
        jz .wait

    .continue:

    mov ax, si
    lea rdx, [rcx + TX_BUF]
    out dx, al

    pop rbp
    ret

; SUBROUTINE - receive a byte over COM
; Parameters - the com port
; Return value - the received byte
global _com_read_byte
_com_read_byte:
    push rbp
    mov rbp, rsp

    mov rax, rdi 
    xor rcx, rcx
    mov cx, [com_ports + rax]

    lea rdx, [rcx + LINE_ST]
    .wait:
        in al, dx
        test al, 0x01
        jz .wait
    
    xor rax, rax
    lea rdx, [rcx + RX_BUF]
    in al, dx

    pop rbp
    ret

