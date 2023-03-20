// All this program does is read some data from the LCD display

// Initialize TM022HDH26 display (Which includes ILI9340 as the driver IC)
// and read the power state of the display to make sure that everything is configured.
// The display is controlled via 4-wire serial protocol (a 
// slight extension of SPI) which goes directly to the ILI9340.

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <time.h>
#include "GPIO.h"

// Assume pins already configured for SPI
// E.g. for SPI0, CS0:
/*
config-pin P9_17 spi_cs
config-pin P9_22 spi_sclk
config-pin P9_18 spi
config-pin P9_21 spi
*/
#define SPI_DEV_BUS0_CS0 "/dev/spidev0.0"
#define SPI_DEV_BUS1_CS0 "/dev/spidev1.0"
#define SPI_DEV_BUS1_CS1 "/dev/spidev1.1"
#define SPI_MODE_DEFAULT 0
#define SPEED_HZ_DEFAULT 500000

#define READ_POWER_MODE 0x0A

void delayMs(long long ms)
{
    const long long NS_PER_MS = 1000000;
    const long long NS_PER_SECOND = 1000000000;
    long long ns = ms * NS_PER_MS;
    struct timespec ts = {ns / NS_PER_SECOND, ns % NS_PER_SECOND};
    nanosleep(&ts, (struct timespec *)NULL);
}

int SPI_initPort(char* spiDevice)
{
    // Open Device
    int spiFileDesc = open(spiDevice, O_RDWR);
    if (spiFileDesc < 0) {
        printf("Error: Can't open device %s\n", spiDevice);
        exit(1);
    }

    // Set port parameters
    // Set SPI mode: Necessary
    int spiMode = SPI_MODE_DEFAULT;
    int err = ioctl(spiFileDesc, SPI_IOC_WR_MODE, &spiMode);
    if (err < 0) {
        printf("Error: Set SPI mode failed\n");
        exit(1);
    }

    return spiFileDesc;
}

void SPI_transfer(int spiFileDesc, uint8_t *sendBuf, uint8_t *receiveBuf, int length)
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
        printf("Error: SPI Transfer failed\n");
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
    // Setup D/C pin, set it LOW
    gpioInfo_t selectDataOrCommand = {.pin={.header=9, .number=23}, .gpioNumber=49};
    GPIO_usePin(&selectDataOrCommand, "out");
    GPIO_setValue(&selectDataOrCommand, false);

    int spiFileDesc = SPI_initPort(SPI_DEV_BUS0_CS0);
    uint8_t txBuf[3];
    uint8_t rxBuf[3];

    /* ---- Run initialization commands ---- */

    // memset(txBuf, 0, 3);
    // memset(rxBuf, 0, 3);
    // txBuf[0] = 0x11; // wake up
    // SPI_transfer(spiFileDesc, txBuf, rxBuf, 3);
    // // printbuf(txBuf, 3);
    // // printbuf(rxBuf, 3);
    // delayMs(5);

    // memset(txBuf, 0, 3);
    // memset(rxBuf, 0, 3);
    // txBuf[0] = 0x29; // turn on display
    // SPI_transfer(spiFileDesc, txBuf, rxBuf, 3);
    // // printbuf(txBuf, 3);
    // // printbuf(rxBuf, 3);

    /* ---- Read info ---- */

    // Read display power mode (0xA): getting back 00 08 00 (display is ON)
    memset(txBuf, 0, 3);
    memset(rxBuf, 0, 3);
    txBuf[0] = 0xA;
    SPI_transfer(spiFileDesc, txBuf, rxBuf, 3);
    // printbuf(txBuf, 3);
    printbuf(rxBuf, 3);

    // Read display status (0x9): getting back 00 00 30 (18 bits / pixel)
    memset(txBuf, 0, 3);
    memset(rxBuf, 0, 3);
    txBuf[0] = 0x9;
    SPI_transfer(spiFileDesc, txBuf, rxBuf, 3);
    // printbuf(txBuf, 3);
    printbuf(rxBuf, 3);

    return 0;
}
