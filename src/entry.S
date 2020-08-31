.syntax unified
.cpu cortex-m7
.thumb

.section .text
.global _start
.type _start, %function
_start:
    ldr r1, =__data_start_ram
    ldr r2, =__data_start_rom
    ldr r3, =__data_end_rom
    cmp r2, r3
    beq 2f
1:
    ldr r0, [r1], #4
    str r0, [r2], #4
    cmp r2, r3
    bne 1b
2:

    ldr r1, =__bss_start
    ldr r2, =__bss_end
    mov r0, #0
    cmp r1, r2
    beq 4f
3:
    str r0, [r1], #4
    cmp r1, r2
    bne 3b
4:

    bl main
    b .
.size _start, . - _start
