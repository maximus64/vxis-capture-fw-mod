	.area CSEG    (CODE)

__sdcc_gsinit_startup::
        lcall   __sdcc_external_startup
        mov     a,dpl
        jz      __sdcc_init_data
        ljmp    __sdcc_program_startup
__sdcc_init_data:
