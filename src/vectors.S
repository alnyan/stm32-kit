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
