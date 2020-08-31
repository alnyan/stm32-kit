#pragma once

struct stm32_tim {
    uint32_t cr1;       // 0x00
    uint32_t cr2;       // 0x04
    uint32_t smcr;      // 0x08
    uint32_t dier;      // 0x0C
    uint32_t sr;        // 0x10
    uint32_t egr;       // 0x14
    uint32_t ccmr1;     // 0x18
    uint32_t ccmr2;     // 0x1C
    uint32_t ccer;      // 0x20
    uint32_t cnt;       // 0x24
    uint32_t psc;       // 0x28
    uint32_t arr;       // 0x2C
    uint32_t __res0;    // 0x30
    uint32_t ccr1;      // 0x34
    uint32_t ccr2;      // 0x38
    uint32_t ccr3;      // 0x3C
    uint32_t ccr4;      // 0x40
    uint32_t __res1;    // 0x44
    uint32_t dcr;       // 0x48
    uint32_t dmar;      // 0x4C
    uint32_t _or;       // 0x50
};

static volatile struct stm32_tim *const tim2 = (struct stm32_tim *) 0x40000000;

#define TIM_CR1_CEN         (1 << 0)
#define TIM_CR1_UDIS        (1 << 1)
#define TIM_CR1_DIR         (1 << 4)

#define TIM_DIER_TIE        (1 << 6)
#define TIM_DIER_UIE        (1 << 0)
