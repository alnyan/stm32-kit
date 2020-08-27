#pragma once
#include <stdint.h>

struct stm32_gpio {
    uint32_t moder;         // 0x00
    uint32_t otyper;        // 0x04
    uint32_t ospeedr;       // 0x08
    uint32_t pupdr;         // 0x0C
    uint32_t idr;           // 0x10
    uint32_t odr;           // 0x14
    uint32_t bsrr;          // 0x18
    uint32_t lckr;          // 0x1C
    uint32_t afrl;          // 0x20
    uint32_t afrh;          // 0x24
};

static volatile struct stm32_gpio *const gpiob = (struct stm32_gpio *) 0x40020400;
static volatile struct stm32_gpio *const gpioc = (struct stm32_gpio *) 0x40020800;
static volatile struct stm32_gpio *const gpiod = (struct stm32_gpio *) 0x40020C00;
