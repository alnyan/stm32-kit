#pragma once
#include <stdint.h>

struct stm32_spi {
    uint32_t cr1;       // 0x00
    uint32_t cr2;       // 0x04
    uint32_t sr;        // 0x08
    uint32_t dr;        // 0x0C
    uint32_t crcpr;     // 0x10
    uint32_t rxcrcr;    // 0x14
    uint32_t txcrcr;    // 0x18
    uint32_t i2scfgr;   // 0x1C
    uint32_t i2spr;     // 0x20
};

static volatile struct stm32_spi *const spi1 = (struct stm32_spi *) 0x40013000;
static volatile struct stm32_spi *const spi4 = (struct stm32_spi *) 0x40013400;
static volatile struct stm32_spi *const spi2 = (struct stm32_spi *) 0x40013800;
static volatile struct stm32_spi *const spi3 = (struct stm32_spi *) 0x40013C00;

#define SPI_CR1_SSM                 (1 << 9)
#define SPI_CR1_SSI                 (1 << 8)
#define SPI_CR1_SPE                 (1 << 6)
#define SPI_CR1_MSTR                (1 << 2)
#define SPI_CR1_CPOL                (1 << 1)
#define SPI_CR1_CPHA                (1 << 0)

#define SPI_CR2_TXEIE               (1 << 7)
#define SPI_CR2_RXNEIE              (1 << 6)
#define SPI_CR2_NSSP                (1 << 3)
#define SPI_CR2_SSOE                (1 << 2)

#define SPI_SR_TXE                  (1 << 1)
#define SPI_SR_RXNE                 (1 << 0)

static inline uint8_t spi_txrx(volatile struct stm32_spi *spi, uint8_t word) {
    *(volatile uint8_t *) &spi->dr = word;
    while (!(spi->sr & SPI_SR_TXE));
    word = *(volatile uint8_t *) &spi->dr;
    while (spi->sr & (1 << 7));
    return word;
}
