#include "stm32f7/spi.h"

#include "spi_sd.h"
#include "util.h"

#define SD_R1_IDLE      (1 << 0)
#define SD_R1_ERST      (1 << 1)
#define SD_R1_INVC      (1 << 2)
#define SD_R1_CRCE      (1 << 3)
#define SD_R1_ESQE      (1 << 4)
#define SD_R1_ADDE      (1 << 5)
#define SD_R1_PARE      (1 << 6)

#define SD_CMD0     (0x40 + 0)
#define SD_CMD1     (0x40 + 1)
#define SD_CMD8     (0x40 + 8)
#define SD_CMD17    (0x40 + 17)
#define SD_ACMD41   (0x40 + 41)
#define SD_CMD55    (0x40 + 55)
#define SD_CMD58    (0x40 + 58)

int spi_sd_cmd(struct spi_sd *sd, uint8_t cmd, uint8_t crc, uint32_t arg, uint8_t *res) {
    uint8_t tmp;

    // cmd
    (void) spi_txrx(sd->spi, cmd);

    // arg
    (void) spi_txrx(sd->spi, (arg >> 24) & 0xFF);
    (void) spi_txrx(sd->spi, (arg >> 16) & 0xFF);
    (void) spi_txrx(sd->spi, (arg >> 8) & 0xFF);
    (void) spi_txrx(sd->spi, arg & 0xFF);

    // CRC
    (void) spi_txrx(sd->spi, crc);

    for (int i = 0; i < 10; ++i) {
        tmp = spi_txrx(sd->spi, 0xFF);
        if (!(tmp & 0x80)) {
            *res = tmp;
            return 0;
        }
    }
    return -1;
}

int spi_sd_read_block(struct spi_sd *sd, uint32_t addr, uint8_t *buf) {
    int res;
    uint8_t tmp;

    res = -1;

    sd->select(1);
    delay(10000);

    if (spi_sd_cmd(sd, 0x40 + 17, 0x00, addr, &tmp) != 0) {
        goto end;
    }

    int attempt = 10;
    while (attempt) {
        tmp = spi_txrx(sd->spi, 0xFF);
        if (tmp != 0xFF) {
            break;
        }
        --attempt;
    }
    if (!attempt) {
        goto end;
    }

    if (tmp == 0xFE) {
        for (int i = 0; i < 512; ++i) {
            buf[i] = spi_txrx(sd->spi, 0xFF);
        }

        (void) spi_txrx(sd->spi, 0xFF);
        (void) spi_txrx(sd->spi, 0xFF);

        res = 0;
    }

end:
    sd->select(0);
    delay(10000);
    return res;
}

int spi_sd_init(struct spi_sd *sd) {
    uint8_t res;
    int ret;
    uint8_t b[4];

    sd->select(1);

    // Send dummy bytes
    for (int i = 0; i < 10; ++i) {
        (void) spi_txrx(sd->spi, 0xFF);
    }
    // CMD0: Software reset
    if (spi_sd_cmd(sd, SD_CMD0, 0x95, 0, &res) != 0) {
        ret = ERR_SD_INIT_CMD0;
        goto end;
    }
    spi_txrx(sd->spi, 0xFF);
    if ((res & 0x7F) != 0x1) {
        ret = ERR_SD_INIT_CMD0;
        goto end;
    }
    delay(10000);

    // CMD8: Set voltage to 2.7-3.3V
    if (spi_sd_cmd(sd, SD_CMD8, 0x86, 0x000001AA, &res) != 0) {
        ret = ERR_SD_INIT_CMD8;
        goto end;
    }
    if (res == 0x01) {
        for (int i = 0; i < 4; ++i) {
            b[i] = spi_txrx(sd->spi, 0xFF);
            // TODO: validate that the device responds with 1AA
        }
    }
    spi_txrx(sd->spi, 0xFF);
    if (res != 0x1) {
        ret = ERR_SD_INIT_NOT_V2;
        goto end;
    }
    delay(10000);

    // CMD58: Get operating conditions
    if (spi_sd_cmd(sd, 0x40 + 58, 0, 0, &res) != 0) {
        ret = ERR_SD_INIT_CMD58;
        goto end;
    }
    if (res <= 0x01) {
        for (int i = 0; i < 4; ++i) {
            b[i] = spi_txrx(sd->spi, 0xFF);
        }
    }
    spi_txrx(sd->spi, 0xFF);
    delay(100000);

    // ACMD41: First try
    if (spi_sd_cmd(sd, SD_CMD55, 0x00, 0x00, &res) != 0) {
        ret = ERR_SD_INIT_ACMD41;
        goto end;
    }
    if (spi_sd_cmd(sd, SD_ACMD41, 0x00, 0x40000000, &res) != 0) {
        ret = ERR_SD_INIT_ACMD41;
        goto end;
    }
    delay(100000);

    // Try ACMD41 until R1 = 0
    for (int i = 0; i < 10; ++i) {
        if (spi_sd_cmd(sd, SD_CMD55, 0x00, 0x00, &res) != 0) {
            ret = ERR_SD_INIT_ACMD41;
            goto end;
        }
        if (spi_sd_cmd(sd, SD_ACMD41, 0x00, 0x40000000, &res) != 0) {
            ret = ERR_SD_INIT_ACMD41;
            goto end;
        }

        if (res == 0x00) {
            ret = 0;
            goto end;
        }
        delay(1000000);
    }

    (void) b;

    ret = ERR_SD_INIT_TIMEOUT;
end:
    sd->select(0);
    return ret;
}

