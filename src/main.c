#include "cortex-m7/systick.h"
#include "stm32f7/gpio.h"
#include "stm32f7/usart.h"
#include "stm32f7/rcc.h"

#define CLK_FREQ_HSI        16000000

void irq_systick(void) {
    static uint32_t counter;
    static uint32_t on;

    ++counter;

    if (on && counter % 100 == 0) {
        gpiob->odr &= ~(1 << 7);
        on = 0;
    }

    if (counter % 1000 == 0) {
        gpiob->odr |= (1 << 7);
        on = 1;
    }
}

_Noreturn void main(void) {
    systick->rvr = 0xF00000 / 1000;
    systick->csr = (1 << 2) | (1 << 1) | (1 << 0);

    // Use HSI for AHB clock
    rcc->cr |= (1 << 0) /* RCC_CR_HSION */;
    rcc->cfgr &= ~0x3 /* Clear RCC_CFGR_SW */;
    rcc->cfgr &= ~(0xF << 4) /* Clear RCC_CFGR_HPRE */;

    // Use HSI for USART
    rcc->dckcfgr2 &= ~(0x3 << 2);
    rcc->dckcfgr2 |= 2 << 2;

    rcc->ahb1enr |= RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIODEN;
    rcc->apb1enr |= RCC_APB1ENR_USART2;

    gpiod->moder &= ~(0x3 << (5 * 2));
    gpiod->moder |= (0x2 << (5 * 2));
    gpiod->pupdr &= ~(0x3 << (5 * 2));
    gpiod->pupdr |= (1 << (5 * 2));
    gpiod->afrl &= ~(0xF << (5 * 4));
    gpiod->afrl |= (7 << (5 * 4));

    gpiob->moder &= ~(0x3 << (7 * 2));
    gpiob->moder |= (0x1 << (7 * 2));
    gpiob->pupdr &= ~(0x3 << (7 * 2));
    gpiob->pupdr |= (0x2 << (7 * 2));
    gpiob->otyper &= ~(1 << 7);

    uint32_t div = CLK_FREQ_HSI / 115200;
    usart2->cr1 &= ~1;
    usart2->brr = div;
    // Enable TX and USART itself
    usart2->cr1 |= (1 << 3);
    usart2->cr1 |= (1 << 0);

    while (1) {
        asm volatile ("wfi");
    }
}
