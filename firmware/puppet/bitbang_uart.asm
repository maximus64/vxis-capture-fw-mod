.module bitbang_serial
.globl _bitbang_putc

.area CSEG (CODE)

CYCLE_CNT .equ 0x96   ;tuned for 9600 baud rate
REG_ADDR  .equ 0x204c ;this register control the blue LED on capture card
SET_HIGH  .equ 3
SET_LOW  .equ 2

_bitbang_putc:
    push acc
    push psw
    mov a, dpl
    mov DPTR, #REG_ADDR
    ; start bit
    mov r7, a
    mov a, #SET_LOW
    movx @DPTR, a
    mov a, r7
    acall __delay
    mov r1, #8
__loop:
    jnb A.0, __low
    ; toggle high
    mov r7, a
    mov a, #SET_HIGH
    sjmp __skip
__low:
    mov r7, a
    mov a, #SET_LOW
__skip:
    movx @DPTR, a
    mov a, r7
    acall __delay
    rr a
    djnz r1, __loop
    ; stop bit
    mov a, #SET_HIGH
    movx @DPTR, a
    acall __delay
    pop psw
    pop acc
    ret

; delay for each bit pulse width
__delay:
    mov r0, #CYCLE_CNT
00001$:
    nop
    djnz r0, 00001$
    ret
