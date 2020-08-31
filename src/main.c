#include "cortex-m7/systick.h"
#include "cortex-m7/nvic.h"
#include "stm32f7/gpio.h"
#include "stm32f7/usart.h"
#include "stm32f7/spi.h"
#include "stm32f7/tim.h"
#include "stm32f7/rcc.h"

//#include "spi_sd.h"
#include "leds.h"
#include "util.h"

#define CLK_FREQ_HSI        16000000

void irq_systick(void) {
    led_toggle(LED_BLINK);
}

void irq_tim2(void) {}
void irq_spi1(void) {}

// TODO: Move *printf from yggdrasil here?
//static void usart2_init(void) {
//    // Use HSI for USART
//    rcc->dckcfgr2 &= ~(0x3 << 2);
//    rcc->dckcfgr2 |= 2 << 2;
//
//    rcc->apb1enr |= RCC_APB1ENR_USART2;
//    delay(1000);
//
//    // PD5 USART2_TX
//    gpiod->moder &= ~(0x3 << (5 * 2));
//    gpiod->moder |= (0x2 << (5 * 2));
//    gpiod->pupdr &= ~(0x3 << (5 * 2));
//    gpiod->pupdr |= (1 << (5 * 2));
//    gpiod->afrl &= ~(0xF << (5 * 4));
//    gpiod->afrl |= (7 << (5 * 4));
//
//    uint32_t div = CLK_FREQ_HSI / 115200;
//    usart2->cr1 &= ~1;
//    usart2->brr = div;
//    // Enable TX and USART itself
//    usart2->cr1 |= (1 << 3);
//    usart2->cr1 |= (1 << 0);
//
//    delay(1000);
//}

// TODO: Magic numbers
//static void spi1_init(void) {
//    rcc->apb2enr |= 1 << 12;
//
//    gpioa->moder &= ~(0x3 << (15 * 2));
//    gpioa->moder |= 0x1 << (15 * 2);
//    gpiob->moder &= ~(0x3 << (3 * 2));
//    gpiob->moder |= 0x2 << (3 * 2);
//    gpiob->moder &= ~(0x3 << (5 * 2));
//    gpiob->moder |= 0x2 << (5 * 2);
//    gpiob->moder &= ~(0x3 << (4 * 2));
//    gpiob->moder |= 0x2 << (4 * 2);
//
//    gpioa->pupdr &= ~(0x3 << (15 * 2));
//    gpioa->pupdr |= (0x1 << (15 * 2));
//    gpiob->pupdr &= ~(0x3 << (3 * 2));
//    gpiob->pupdr |= (0x1 << (3 * 2));
//    gpiob->pupdr &= ~(0x3 << (5 * 2));
//    gpiob->pupdr |= (0x1 << (5 * 2));
//    gpiob->pupdr &= ~(0x3 << (4 * 2));
//    gpiob->pupdr |= (0x1 << (4 * 2));
//
//    gpiob->afrl &= ~(0xF << (3 * 4));
//    gpiob->afrl |= (0x5 << (3 * 4));
//    gpiob->afrl &= ~(0xF << (5 * 4));
//    gpiob->afrl |= (0x5 << (5 * 4));
//    gpiob->afrl &= ~(0xF << (4 * 4));
//    gpiob->afrl |= (0x5 << (4 * 4));
//
//    // Disable SPI1 first
//    spi1->cr1 &= ~SPI_CR1_SPE;
//    spi1->cr2 &= ~(0xF << 8);
//    spi1->cr2 |= 7 << 8;
//    spi1->cr1 |= SPI_CR1_MSTR | (0x7 << 3);
//    spi1->cr1 |= SPI_CR1_SSM | SPI_CR1_SSI;
//    spi1->cr1 |= SPI_CR1_SPE;
//}
//
//static void spi1_select(uint32_t sel) {
//    if (sel) {
//        gpioa->odr &= ~(1 << 15);
//    } else {
//        gpioa->odr |= (1 << 15);
//    }
//}

_Noreturn void main(void) {
    systick->rvr = 0xF00000;
    systick->csr = (1 << 2) | (1 << 1) | (1 << 0);

    // Use HSI for AHB clock
    rcc->cr |= (1 << 0) /* RCC_CR_HSION */;
    rcc->cfgr &= ~(0xF << 4) /* Clear RCC_CFGR_HPRE */;
    // APB1 prescaler = 2
    rcc->cfgr &= ~(0x7 << 10);

    rcc->ahb1enr |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIODEN;

    leds_init();
    //usart2_init();
    //spi1_init();
    //spi1_select(0);
    //delay(30000);

    led_set(LED_ERROR, 1);

    while (1) {
        asm volatile ("wfi");
    }
}
