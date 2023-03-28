#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "GPIO.h"

#define CS_PIN 48
#define CS_LOW GPIO_setValue(CS_PIN, false);
#define CS_HIGH GPIO_setValue(CS_PIN, true);

#define RST_PIN 68
#define RST_LOW GPIO_setValue(RST_PIN, false);
#define RST_HIGH GPIO_setValue(RST_PIN, true);

#define DC_PIN 49
#define DC_LOW GPIO_setValue(DC_PIN, false);
#define DC_HIGH GPIO_setValue(DC_PIN, true);

#define INIT_GPIO GPIO_pinMode(CS_PIN, true); GPIO_pinMode(RST_PIN, true); GPIO_pinMode(DC_PIN, true);

#define COL_MAX 240
#define ROW_MAX 320

#define SPI_MAX_LEN 4096 // bytes

static int spiFileDesc = -1;

int spiInit(char* spiDevice)
{
    // Open Device
    spiFileDesc = open(spiDevice, O_RDWR);
    if (spiFileDesc < 0) {
        printf("Error: Can't open device %s\n", spiDevice);
        exit(1);
    }

    // Set port parameters
    // Set SPI mode: Necessary
    int spiMode = SPI_MODE_0;
    int err = ioctl(spiFileDesc, SPI_IOC_WR_MODE, &spiMode);
    if (err < 0) {
        perror("Error: Set SPI mode failed\n");
        exit(1);
    }
}

// Does not touch CS or D/C
void spiTransfer(uint8_t *sendBuf, uint8_t *receiveBuf, int length)
{
    // Setting transfer this way ensures all other fields are 0
    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long) sendBuf,
        .rx_buf = (unsigned long) receiveBuf,
        .len = length
    };

    const int NUM_TRANSFERS = 1;
    // SPI_IOC_MESSAGE does https://linuxtv.org/downloads/v4l-dvb-internals/device-drivers/API-spi-sync.html
    int status = ioctl(spiFileDesc, SPI_IOC_MESSAGE(NUM_TRANSFERS), &transfer);
    if (status < 0) {
        perror("Error: SPI Transfer failed");
    }
}

int main(int argc, char const *argv[])
{
    spiInit("/dev/spidev0.0");
    uint8_t txBuf[SPI_MAX_LEN];
    uint8_t rxBuf[SPI_MAX_LEN];
    spiTransfer(txBuf, rxBuf, SPI_MAX_LEN);
    return 0;
}
