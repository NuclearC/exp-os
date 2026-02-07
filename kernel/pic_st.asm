

bits 32

PIC1 equ 0x20
PIC2 equ 0xA0
PIC1_CMD equ PIC1
PIC1_DATA equ PIC1+1
PIC2_CMD equ PIC2
PIC2_DATA equ PIC2+1
PIC_EOI equ 0x20

ICW1_INIT equ 0x10
ICW1_ICW4 equ 0x01
ICW1_SINGLE equ 0x02
ICW1_INTERVAL4 equ 0x04
ICW1_LEVEL equ 0x08

ICW4_8086 equ 0x01
ICW4_AUTO equ 0x02
ICW4_BUF_SLAVE equ 0x08
ICW4_BUF_MASTER equ 0x0c
ICW4_SFNM equ 0x10

CASCADE_IRQ equ 2

section .text
; sets up the Programmable Interrupt Controller
global _pic_setup
_pic_setup:
    enter 0
    ; save the initial masks
    in al, PIC1_DATA
    mov cl, al
    in al, PIC2_DATA
    mov ch, al 

    mov al, (ICW1_INIT | ICW1_ICW4) 
    out PIC1_CMD, al
    out PIC2_CMD, al

    mov al, 0x20 ; offset 1
    out PIC1_DATA, al
    mov al, 0x28 ; offset 2
    out PIC2_DATA, al

    mov al, (1 << CASCADE_IRQ)
    out PIC1_DATA, al
    mov al, CASCADE_IRQ
    out PIC2_DATA, al

    mov al, ICW4_8086
    out PIC1_DATA, al
    out PIC2_DATA, al

    leave
    ret

global _pic_getmask
_pic_getmask:
    enter 0
    in al, PIC1_DATA
    mov cl, al
    in al, PIC2_DATA
    mov ch, al
    mov eax, ecx
    leave
    ret  
  
global _pic_setmask
_pic_setmask:
    enter 0
    mov eax, [ebp + 8]
    out PIC1_DATA, al
    mov al, ah
    out PIC2_DATA, al
    leave
    ret

global _pic1_int_end
_pic1_int_end:
    mov al, PIC_EOI
    out PIC1_CMD, al
    ret

global _pic_int_end
_pic_int_end:
    mov al, PIC_EOI
    out PIC2_CMD, al
    out PIC1_CMD, al
    ret




