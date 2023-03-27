#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <time.h>
#include "GPIO.h"

int spiInit(char* spiDevice)
{
    // Open Device
    int spiFileDesc = open(spiDevice, O_RDWR);
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

    return spiFileDesc;
}


void delayMs(long long ms)
{
    const long long NS_PER_MS = 1000000;
    const long long NS_PER_SECOND = 1000000000;
    long long ns = ms * NS_PER_MS;
    struct timespec ts = {ns / NS_PER_SECOND, ns % NS_PER_SECOND};
    nanosleep(&ts, (struct timespec *)NULL);
}

void spiSend(int spiFileDesc, uint8_t *sendBuf, uint8_t *receiveBuf, int length)
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

void printbuf(uint8_t *buf, int length)
{
    for (size_t i = 0; i < length; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

int main(void)
{
    // Setup D/C pin
    gpioInfo_t dcPin = {.pin={.header=9, .number=23}, .gpioNumber=49};
    GPIO_usePin(&dcPin, "out");

    /* ---- Power on sequence ---- */
    // Based on: https://cdn-shop.adafruit.com/datasheets/TM022HDH26_V1.0.pdf page 13.

    // Setup Reset pin: set it HIGH, then set it LOW for 10 microseconds (to reset), then HIGH.
    // Based on: https://cdn-shop.adafruit.com/datasheets/TM022HDH26_V1.0.pdf page 12.
    gpioInfo_t rstPin = {.pin={.header=8, .number=10}, .gpioNumber=68};
    GPIO_usePin(&rstPin, "out");
    GPIO_setValue(&rstPin, true);
    delayMs(100);
    GPIO_setValue(&rstPin, false);
    delayMs(10);
    GPIO_setValue(&rstPin, true);
    delayMs(20);

    // Setup CS pin
    gpioInfo_t csPin = {.pin={.header=9, .number=15}, .gpioNumber=48};
    GPIO_usePin(&csPin, "out");
    GPIO_setValue(&csPin, true);

    int spiFileDesc = spiInit("/dev/spidev0.0");

    GPIO_setValue(&dcPin, false);
    GPIO_setValue(&csPin, false);

    // Read power mode cmd: 0x08 (expected result!) Normal mode 
    uint8_t txBuf[2];
    uint8_t rxBuf[2];
    memset(txBuf, 0, 2);
    memset(rxBuf, 0, 2);
    txBuf[0] = 0xA;
    spiSend(spiFileDesc, txBuf, rxBuf, 2);
    printbuf(rxBuf, 2);

    // Wake up
    memset(txBuf, 0, 2);
    memset(rxBuf, 0, 2);
    txBuf[0] = 0x11;
    spiSend(spiFileDesc, txBuf, rxBuf, 2);
    printbuf(rxBuf, 2);

    // Read power mode cmd: 0x98 (expected result!) Normal mode, Sleep out, Booster on
    memset(txBuf, 0, 2);
    memset(rxBuf, 0, 2);
    txBuf[0] = 0xA;
    spiSend(spiFileDesc, txBuf, rxBuf, 2);
    printbuf(rxBuf, 2);

    return 0;
}
