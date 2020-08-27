#pragma once
#include <stdint.h>

struct cortex_m7_systick {
    uint32_t csr;
    uint32_t rvr;
    uint32_t cvr;
    uint32_t calib;
};

static volatile struct cortex_m7_systick *const systick = (struct cortex_m7_systick *) 0xE000E010;
