#pragma once

#define NVIC_IRQ_TIM2               28
#define NVIC_IRQ_SPI1               35

static volatile uint32_t *const nvic_iser = (uint32_t *) 0xE000E100;
static volatile uint32_t *const nvic_icpr = (uint32_t *) 0xE000E280;

static inline void nvic_irq_enable(uint32_t n) {
    nvic_iser[n >> 5] = 1 << (n & 0x1F);
}

static inline void nvic_irq_clear_pending(uint32_t n) {
    nvic_icpr[n >> 5] = 1 << (n & 0x1F);
}
