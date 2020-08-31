#pragma once
#include <stdint.h>

static inline void delay(uint32_t d) {
    for (uint32_t i = 0; i < d; ++i) {
        asm volatile ("");
    }
}

