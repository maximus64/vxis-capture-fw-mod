.module start
.globl  _main

.area FW_HEADER (ABS)
.org 0xc000
    .byte 0x89 ; Magic header bootrom check this
    .byte 0x26
