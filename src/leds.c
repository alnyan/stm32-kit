#include "stm32f7/gpio.h"

#include "leds.h"

void led_toggle(int which) {
    gpiob->odr ^= which;
}

void led_set(int which, int state) {
    if (state) {
        gpiob->odr |= which;
    } else {
        gpiob->odr &= ~which;
    }
}

void leds_init(void) {
    // PB1:     LED_ACTIVITY
    // PB7:     LED_BLINK
    // PB14     LED_ERROR

    gpiob->moder &= ~(0x3 << (7 * 2));
    gpiob->moder |= (0x1 << (7 * 2));
    gpiob->pupdr &= ~(0x3 << (7 * 2));
    gpiob->pupdr |= (0x2 << (7 * 2));
    gpiob->otyper &= ~(1 << 7);

    gpiob->moder &= ~(0x3 << (14 * 2));
    gpiob->moder |= (0x1 << (14 * 2));
    gpiob->pupdr &= ~(0x3 << (14 * 2));
    gpiob->pupdr |= (0x2 << (14 * 2));
    gpiob->otyper &= ~(1 << 14);
}

