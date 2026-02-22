
bits 32


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
; Parameter - the com port
global _com_init
_com_init:
    enter 0
    mov eax, [ebp + 8] ; com port
    xor ecx, ecx
    mov cx, [com_ports + eax] 

    lea edx, [ecx + INT_EN]
    xor al, al
    out dx, al ; interrupts disable

    lea edx, [ecx + LINE_CTRL]
    mov al, 0x80
    out dx, al ; enable DLAB

    lea edx, [ecx + BAUD_DIV_LOW]
    mov al, 0x03
    out dx, al ; Baud rate 38400

    lea edx, [ecx + BAUD_DIV_HI]
    xor al, al
    out dx, al

    lea edx, [ecx + LINE_CTRL]
    mov al, 0x03
    out dx, al ; 8 bits, no parity, one stop bit

    lea edx, [ecx + FIFO_CTRL]
    mov al, 0xc7
    out dx, al ; enable fifo, clear them, with 14 byte threshold

    lea edx, [ecx + MDM_CTRL]
    mov al, 0x0b
    out dx, al
    mov al, 0x1e
    out dx, al

    ; Test the shit
    lea edx, [ecx + TX_BUF]
    mov al, 0xAE
    out dx, al

    in al, dx
    cmp al, 0xAE
    jne .fail

    lea edx, [ecx + MDM_CTRL]
    mov al, 0x0f
    out dx, al
    xor eax, eax
    jmp .skip

    .fail
    mov eax, 1 
    .skip
    leave
    ret

; SUBROUTINE - transmit a byte over COM
; Parameters - the com port (u16)
;              the byte (u8)
global _com_write_byte
_com_write_byte:
    enter 0
    
    mov eax, [ebp + 8] ; com port
    xor ecx, ecx
    mov cx, [com_ports + eax]

    lea edx, [ecx + LINE_ST]
    .wait:
        in al, dx
        test al, 0x20
        jz .wait

    .continue:

    mov al, [ebp + 12] 
    lea edx, [ecx + TX_BUF]
    out dx, al

    leave
    ret

; SUBROUTINE - receive a byte over COM
; Parameters - the com port
; Return value - the received byte
global _com_read_byte
_com_read_byte:
    enter 0

    mov eax, [ebp + 8]
    xor ecx, ecx
    mov cx, [com_ports + eax]

    lea edx, [ecx + LINE_ST]
    .wait:
        in al, dx
        test al, 0x01
        jz .wait
    
    xor eax, eax
    lea edx, [ecx + RX_BUF]
    in al, dx

    leave
    ret





