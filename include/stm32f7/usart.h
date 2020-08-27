#pragma once
#include <stdint.h>

struct stm32_usart {
    uint32_t cr1;       // 0x00
    uint32_t cr2;       // 0x04
    uint32_t cr3;       // 0x08
    uint32_t brr;       // 0x0C
    uint32_t gtpr;      // 0x10
    uint32_t rtor;      // 0x14
    uint32_t rqr;       // 0x18
    uint32_t isr;       // 0x1C
    uint32_t icr;       // 0x20
    uint32_t rdr;       // 0x24
    uint32_t tdr;       // 0x28
};

void usart_tx(volatile struct stm32_usart *usart, int ch);

static volatile struct stm32_usart *const usart1 = (struct stm32_usart *) 0x40011000;
static volatile struct stm32_usart *const usart2 = (struct stm32_usart *) 0x40004400;
