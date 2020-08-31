#include "cortex-m7/systick.h"
#include "cortex-m7/nvic.h"
#include "stm32f7/gpio.h"
#include "stm32f7/usart.h"
#include "stm32f7/spi.h"
#include "stm32f7/tim.h"
#include "stm32f7/rcc.h"

#include "libc/string.h"
#include "spi_sd.h"
#include "debug.h"
#include "leds.h"
#include "util.h"

#define CLK_FREQ_HSI        16000000

void irq_systick(void) {
    led_toggle(LED_BLINK);
}

void irq_tim2(void) {}
void irq_spi1(void) {}

static void usart2_init(void) {
    // Use HSI for USART
    rcc->dckcfgr2 &= ~(0x3 << 2);
    rcc->dckcfgr2 |= 2 << 2;

    rcc->apb1enr |= RCC_APB1ENR_USART2;
    delay(1000);

    // PD5 USART2_TX
    gpiod->moder &= ~(0x3 << (5 * 2));
    gpiod->moder |= (0x2 << (5 * 2));
    gpiod->pupdr &= ~(0x3 << (5 * 2));
    gpiod->pupdr |= (1 << (5 * 2));
    gpiod->afrl &= ~(0xF << (5 * 4));
    gpiod->afrl |= (7 << (5 * 4));

    // PD6 USART2_RX
    gpiod->moder &= ~(0x3 << (6 * 2));
    gpiod->moder |= (0x2 << (6 * 2));
    gpiod->pupdr &= ~(0x3 << (6 * 2));
    gpiod->pupdr |= (0 << (6 * 2));
    gpiod->afrl &= ~(0xF << (6 * 4));
    gpiod->afrl |= (7 << (6 * 4));

    uint32_t div = CLK_FREQ_HSI / 115200;
    usart2->cr1 &= ~1;
    usart2->brr = div;
    // Enable TX, RX and USART itself
    usart2->cr1 |= (1 << 3) | (1 << 2);
    usart2->cr1 |= (1 << 0);

    delay(1000);
}

// TODO: Magic numbers
static void spi1_init(void) {
    rcc->apb2enr |= 1 << 12;

    gpioa->moder &= ~(0x3 << (15 * 2));
    gpioa->moder |= 0x1 << (15 * 2);
    gpiob->moder &= ~(0x3 << (3 * 2));
    gpiob->moder |= 0x2 << (3 * 2);
    gpiob->moder &= ~(0x3 << (5 * 2));
    gpiob->moder |= 0x2 << (5 * 2);
    gpiob->moder &= ~(0x3 << (4 * 2));
    gpiob->moder |= 0x2 << (4 * 2);

    gpioa->pupdr &= ~(0x3 << (15 * 2));
    gpioa->pupdr |= (0x1 << (15 * 2));
    gpiob->pupdr &= ~(0x3 << (3 * 2));
    gpiob->pupdr |= (0x1 << (3 * 2));
    gpiob->pupdr &= ~(0x3 << (5 * 2));
    gpiob->pupdr |= (0x1 << (5 * 2));
    gpiob->pupdr &= ~(0x3 << (4 * 2));
    gpiob->pupdr |= (0x1 << (4 * 2));

    gpiob->afrl &= ~(0xF << (3 * 4));
    gpiob->afrl |= (0x5 << (3 * 4));
    gpiob->afrl &= ~(0xF << (5 * 4));
    gpiob->afrl |= (0x5 << (5 * 4));
    gpiob->afrl &= ~(0xF << (4 * 4));
    gpiob->afrl |= (0x5 << (4 * 4));

    // Disable SPI1 first
    spi1->cr1 &= ~SPI_CR1_SPE;
    spi1->cr2 &= ~(0xF << 8);
    spi1->cr2 |= 7 << 8;
    spi1->cr1 |= SPI_CR1_MSTR | (0x7 << 3);
    spi1->cr1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    spi1->cr1 |= SPI_CR1_SPE;
}

static void spi1_select(uint32_t sel) {
    if (sel) {
        gpioa->odr &= ~(1 << 15);
    } else {
        gpioa->odr |= (1 << 15);
    }
}

static int usart_gets(char *buf, size_t lim) {
    int ch;
    size_t n = 0;

    while (lim) {
        ch = usart_rx(usart2);

        if (ch == '\r') {
            usart_tx(usart2, '\r');
            usart_tx(usart2, '\n');
            break;
        } else if (ch == 127) {
            if (n) {
                --n;
                ++lim;
                usart_tx(usart2, '\033');
                usart_tx(usart2, '[');
                usart_tx(usart2, 'D');
                usart_tx(usart2, ' ');
                usart_tx(usart2, '\033');
                usart_tx(usart2, '[');
                usart_tx(usart2, 'D');
            }
        } else if (ch >= ' ') {
            usart_tx(usart2, ch);
            buf[n++] = ch;
            --lim;
        }
    }
    if (!lim) {
        return -1;
    }
    buf[n] = 0;
    return 0;
}

struct command {
    const char *name;
    int (*exec) (const char *arg);
};

static struct spi_sd sd0 = { .spi = spi1, .select = spi1_select };
static int sd0_init;

static int cmd_sd_init(const char *p) {
    (void) p;
    if (sd0_init) {
        printf("SD: already initialized\n");
        return 0;
    }
    led_set(LED_ACTIVITY, 1);
    if (spi_sd_init(&sd0) != 0) {
        printf("SD: card init failed\n");
        led_set(LED_ACTIVITY, 0);
        led_set(LED_ERROR, 1);
        return -1;
    }
    led_set(LED_ACTIVITY, 0);
    sd0_init = 1;
    return 0;
}

static uint32_t atou(const char *p) {
    char c;
    uint32_t r = 0;
    while ((c = *p++) >= '0' && c <= '9') {
        r *= 10;
        r += c - '0';
    }
    return r;
}

static int cmd_sd_read(const char *p) {
    (void) p;
    if (!*p) {
        printf("Usage:\n\tsd:read SECTOR\n");
        return -1;
    }

    uint32_t sector = atou(p);
    uint8_t data[512];

    led_set(LED_ACTIVITY, 1);
    if (spi_sd_read_block(&sd0, sector, data) != 0) {
        printf("SD: read failed: sector=%u\n", sector);
        led_set(LED_ACTIVITY, 0);
        led_set(LED_ERROR, 1);
        return -1;
    }
    led_set(LED_ACTIVITY, 0);

    debug_dump(data, 512);

    return 0;
}

static int cmd_help(const char *p);

static const struct command commands[] = {
    { "help",       cmd_help },
    { "sd:init",    cmd_sd_init },
    { "sd:read",    cmd_sd_read },
};

static int cmd_help(const char *p) {
    (void) p;
    printf("Commands:\n");
    for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); ++i) {
        printf(" - %s\n", commands[i].name);
    }
    return 0;
}

static int cmd_exec(char *line) {
    char *p = strchr(line, ' ');
    if (!p) {
        p = line + strlen(line);
    } else {
        *p++ = 0;
    }

    for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); ++i) {
        if (!strcmp(line, commands[i].name)) {
            return commands[i].exec(p);
        }
    }

    return 1;
}

static _Noreturn void main_loop(void) {
    char line[256];

    while (1) {
        printf("> ");
        if (usart_gets(line, sizeof(line)) != 0) {
            continue;
        }

        if (cmd_exec(line) == 1) {
            printf("%s: no such command\n", line);
        }
    }
}

_Noreturn void main(void) {
    systick->rvr = 0xF00000;
    systick->csr = (1 << 2) | (1 << 1) | (1 << 0);

    // Use HSI for AHB clock
    rcc->cr |= (1 << 0) /* RCC_CR_HSION */;
    rcc->cfgr &= ~(0xF << 4) /* Clear RCC_CFGR_HPRE */;
    // APB1 prescaler = 2
    rcc->cfgr &= ~(0x7 << 10);

    rcc->ahb1enr |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIODEN;

    usart2_init();
    leds_init();
    spi1_init();
    spi1_select(0);
    delay(30000);

    printf("\n\n");
    printf("Initializing\n");

    main_loop();
}
