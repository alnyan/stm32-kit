.syntax unified
.cpu cortex-m7
.thumb

.section .text
.global _start
.type _start, %function
_start:
    bl main
    b .
.size _start, . - _start

.section .vectors, "a", %progbits
.type _vectors, %object
_vectors:
    .word __stack_top                 // Stack pointer, not needed yet
    .word _start            // Reset vector

    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
    .word 0
.size _vectors, . - _vectors
