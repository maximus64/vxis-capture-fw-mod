.area CSEG (ABS, CODE)
.org 0xc000
    .byte 0x89 ; Magic header bootrom check this
    .byte 0x26
.org 0xc006
entry:
    clr EA ;turn off interrupt
    mov DPTR, #0x204c
blink:
    mov A, #3
    movx @DPTR, A ;turn led on
    acall delay
    mov A, #2
    movx @DPTR, A ;turn led off
    acall delay
    sjmp blink

delay:
    mov r1, #0xff
loopa:
    mov r2, #0xff
loopb:
    mov r3, #0x10
loopc:
    djnz r3, loopc
    djnz r2, loopb
    djnz r1, loopa
    ret
