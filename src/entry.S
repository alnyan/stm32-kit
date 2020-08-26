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
