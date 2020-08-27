#pragma once

struct stm32_rcc {
    uint32_t cr;            // 0x00
    uint32_t pllcfgr;       // 0x04
    uint32_t cfgr;          // 0x08
    uint32_t cir;           // 0x0C
    uint32_t ahb1rstr;      // 0x10
    uint32_t ahb2rstr;      // 0x14
    uint32_t ahb3rstr;      // 0x18
    uint32_t __res0;        // 0x1C
    uint32_t apb1rstr;      // 0x20
    uint32_t apb2rstr;      // 0x24
    uint32_t __res1;        // 0x28
    uint32_t __res2;        // 0x2C
    uint32_t ahb1enr;       // 0x30
    uint32_t ahb2enr;       // 0x34
    uint32_t ahb3enr;       // 0x38
    uint32_t __res3;        // 0x3C
    uint32_t apb1enr;       // 0x40
    uint32_t apb2enr;       // 0x44
    uint32_t __res4;        // 0x48
    uint32_t __res5;        // 0x4C
    uint32_t ahb1lpenr;     // 0x50
    uint32_t ahb2lpenr;     // 0x54
    uint32_t ahb3lpenr;     // 0x58
    uint32_t __res6;        // 0x5C
    uint32_t apb1lpenr;     // 0x60
    uint32_t apb2lpenr;     // 0x64
    uint32_t __res7;        // 0x68
    uint32_t __res8;        // 0x6C
    uint32_t bdcr;          // 0x70
    uint32_t csr;           // 0x74
    uint32_t __res9;        // 0x78
    uint32_t __res10;       // 0x7C
    uint32_t sscgr;         // 0x80
    uint32_t plli2scfgr;    // 0x84
    uint32_t pllsaicfgr;    // 0x88
    uint32_t dckcfgr1;      // 0x8C
    uint32_t dckcfgr2;      // 0x90
};

static volatile struct stm32_rcc *const rcc = (struct stm32_rcc *) 0x40023800;

#define RCC_AHB1ENR_GPIODEN     (1 << 3)
#define RCC_AHB1ENR_GPIOCEN     (1 << 2)
#define RCC_AHB1ENR_GPIOBEN     (1 << 1)
#define RCC_AHB1ENR_GPIOAEN     (1 << 0)

#define RCC_APB1ENR_USART2      (1 << 17)
