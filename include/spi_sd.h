#pragma once
#include <stdint.h>

#define ERR_SD_INIT_CMD0        (-1)
#define ERR_SD_INIT_CMD8        (-2)
#define ERR_SD_INIT_NOT_V2      (-3)
#define ERR_SD_INIT_CMD58       (-4)
#define ERR_SD_INIT_ACMD41      (-5)
#define ERR_SD_INIT_TIMEOUT     (-6)

struct stm32_spi;

struct spi_sd {
    volatile struct stm32_spi *spi;
    void (*select)(uint32_t sel);
};

int spi_sd_init(struct spi_sd *sd);
int spi_sd_read_block(struct spi_sd *sd, uint32_t sector, uint8_t *buf);
