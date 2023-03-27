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

// Assume pins already configured for SPI and GPIO (Reset, D/C).
// To do this, run `config-pin-script.sh`.
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
        perror("Error: Set SPI mode failed\n");
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
    // Setup D/C pin, set it LOW.
    gpioInfo_t selectData = {.pin={.header=9, .number=23}, .gpioNumber=49};
    GPIO_usePin(&selectData, "out");
    GPIO_setValue(&selectData, false);

    /* ---- Power on sequence ---- */
    // Based on: https://cdn-shop.adafruit.com/datasheets/TM022HDH26_V1.0.pdf page 13.

    // Setup Reset pin: set it HIGH, then set it LOW for 10 microseconds (to reset), then HIGH.
    // Based on: https://cdn-shop.adafruit.com/datasheets/TM022HDH26_V1.0.pdf page 12.
    gpioInfo_t reset = {.pin={.header=8, .number=10}, .gpioNumber=68};
    GPIO_usePin(&reset, "out");
    GPIO_setValue(&reset, true);
    delayMs(100);
    GPIO_setValue(&reset, false);
    delayMs(10);
    GPIO_setValue(&reset, true);
    delayMs(20);

    int spiFileDesc = SPI_initPort(SPI_DEV_BUS0_CS0);
    // 0x9 returns 4 bytes, and the first byte is not going to get written to, so we need buffer of size BUFFSIZE
    #define BUFFSIZE 5
    uint8_t txBuf[BUFFSIZE];
    uint8_t rxBuf[BUFFSIZE];

    memset(txBuf, 0, BUFFSIZE);
    memset(rxBuf, 0, BUFFSIZE);
    txBuf[0] = 0x11; // wake up
    SPI_transfer(spiFileDesc, txBuf, rxBuf, 1);
    delayMs(60);

    memset(txBuf, 0, BUFFSIZE);
    memset(rxBuf, 0, BUFFSIZE);
    txBuf[0] = 0x29; // turn on display
    SPI_transfer(spiFileDesc, txBuf, rxBuf, 1);

    /* ---- Read info ---- */

    // Read display power mode (0xA): getting back 00 9c 00 00 00 with updated power on sequence.
    // 9c (1 byte return), -> 10011100 which means (MSB first)
    // Booster OK, Sleep OUT, Normal mode, Display ON
    memset(txBuf, 0, BUFFSIZE);
    memset(rxBuf, 0, BUFFSIZE);
    txBuf[0] = 0xA;
    SPI_transfer(spiFileDesc, txBuf, rxBuf, BUFFSIZE);
    printbuf(rxBuf, BUFFSIZE);

    // Read display status (0x9): getting back 00 c0 31 82 00.
    // c0 31 82 00 (4 byte return) -> 11000000 00110001 10000010 00000000 which means
    // Booster ON, Bottom to Top row address order, 18-bit/pixel, Normal mode on, Tearing effect line ON
    memset(txBuf, 0, BUFFSIZE);
    memset(rxBuf, 0, BUFFSIZE);
    txBuf[0] = 0x9;
    SPI_transfer(spiFileDesc, txBuf, rxBuf, BUFFSIZE);
    printbuf(rxBuf, BUFFSIZE);

    /* ---- Write image data ---- */ 
    // Set the screen to black, then wait for 2 seconds, then set it to white, then wait for 2 seconds, then set it to red.

    // Memory write command (0x2C), 
    memset(txBuf, 0, BUFFSIZE);
    memset(rxBuf, 0, BUFFSIZE);
    txBuf[0] = 0x2C;
    SPI_transfer(spiFileDesc, txBuf, rxBuf, 1);

    for (size_t i = 0; i < 64; i++) // 320 / 5 = 64: make the screen black.
    {
        // Send data: D/C high and 20 lines of black
        // 3 * 240 bytes (240 pixels) = 720 bytes per line
        #define LINEBYTES 720
        #define NUMLINES 5 // 20  gives "Error: SPI Transfer failed: Message too long"
        #define DATABUFLEN LINEBYTES * NUMLINES
        uint8_t databuf[DATABUFLEN];
        memset(databuf, 0, DATABUFLEN);
        GPIO_setValue(&selectData, true);
        SPI_transfer(spiFileDesc, databuf, NULL, DATABUFLEN);
    }

    // Signal end of data transmission by sending any command. NEED THIS OTHERWISE CONTINUES FILLING REST OF SCREEN.
    GPIO_setValue(&selectData, false);
    memset(txBuf, 0, BUFFSIZE);
    memset(rxBuf, 0, BUFFSIZE);
    txBuf[0] = 0x00; // NOP
    SPI_transfer(spiFileDesc, txBuf, rxBuf, 1);

    delayMs(2000);

    // Column address set: 
    // ! DOESN'T WORK, YOU NEED TO CONTROL D/C MID-TRANSFER
    memset(txBuf, 0, BUFFSIZE);
    memset(rxBuf, 0, BUFFSIZE);
    txBuf[0] = 0x2A; // Column address set (columns go left to right, 2 bytes per column, MSB first byte)
    // Column 100 -> 200 only: hex(100) = 0x64
    txBuf[1] = 0x00;
    txBuf[2] = 0x64;
    // hex(200) = 0xC8
    txBuf[3] = 0x00;
    txBuf[4] = 0xC8;
    SPI_transfer(spiFileDesc, txBuf, NULL, BUFFSIZE);

    // Memory write command (0x2C), 
    memset(txBuf, 0, BUFFSIZE);
    memset(rxBuf, 0, BUFFSIZE);
    txBuf[0] = 0x2C;
    SPI_transfer(spiFileDesc, txBuf, rxBuf, 1);

    for (size_t i = 0; i < 64; i++) // make the screen white.
    {
        // Send data: D/C high and 20 lines of white
        // 3 * 240 bytes (240 pixels) = 720 bytes per line
        #define LINEBYTES 720
        #define NUMLINES 5 // 20  gives "Error: SPI Transfer failed: Message too long"
        #define DATABUFLEN LINEBYTES * NUMLINES
        uint8_t databuf[DATABUFLEN];
        memset(databuf, 0xFF, DATABUFLEN);
        GPIO_setValue(&selectData, true);
        SPI_transfer(spiFileDesc, databuf, NULL, DATABUFLEN);
    }

    // Signal end of data transmission by sending any command.
    GPIO_setValue(&selectData, false);
    memset(txBuf, 0, BUFFSIZE);
    memset(rxBuf, 0, BUFFSIZE);
    txBuf[0] = 0x00; // NOP
    SPI_transfer(spiFileDesc, txBuf, rxBuf, 1);

    delayMs(2000);

    // Memory write command (0x2C), 
    memset(txBuf, 0, BUFFSIZE);
    memset(rxBuf, 0, BUFFSIZE);
    txBuf[0] = 0x2C;
    SPI_transfer(spiFileDesc, txBuf, rxBuf, 1);

    for (size_t i = 0; i < 64; i++) // 320 / 5 = 64: make the screen blue.
    {
        // Send data: D/C high and 20 lines of black
        // 3 * 240 bytes (240 pixels) = 720 bytes per line
        #define LINEBYTES 720
        #define NUMLINES 5 // 20  gives "Error: SPI Transfer failed: Message too long"
        #define DATABUFLEN LINEBYTES * NUMLINES // 5 lines
        uint8_t databuf[DATABUFLEN];
        memset(databuf, 0, DATABUFLEN);
        for (size_t j = 0; j < DATABUFLEN; j+=3) // BGR!
        {
            databuf[j] = 0xFF;
        }
        GPIO_setValue(&selectData, true);
        SPI_transfer(spiFileDesc, databuf, NULL, DATABUFLEN);
    }

    // Signal end of data transmission by sending any command. NEED THIS OTHERWISE CONTINUES FILLING REST OF SCREEN.
    GPIO_setValue(&selectData, false);
    memset(txBuf, 0, BUFFSIZE);
    memset(rxBuf, 0, BUFFSIZE);
    txBuf[0] = 0x00; // NOP
    SPI_transfer(spiFileDesc, txBuf, rxBuf, 1);

    delayMs(2000);

    ///////////////////////////////////////////////

    // Memory write command (0x2C), 
    memset(txBuf, 0, BUFFSIZE);
    memset(rxBuf, 0, BUFFSIZE);
    txBuf[0] = 0x2C;
    SPI_transfer(spiFileDesc, txBuf, rxBuf, 1);

    for (size_t i = 0; i < 64; i++) // 320 / 5 = 64: make the screen torquoise.
    {
        // Send data: D/C high and 20 lines of black
        // 3 * 240 bytes (240 pixels) = 720 bytes per line
        #define LINEBYTES 720
        #define NUMLINES 5 // 20  gives "Error: SPI Transfer failed: Message too long"
        #define DATABUFLEN LINEBYTES * NUMLINES // 5 lines
        uint8_t databuf[DATABUFLEN];
        memset(databuf, 0, DATABUFLEN);
        for (size_t j = 0; j < DATABUFLEN; j+=3) // BGR!
        {
            databuf[j] = 0xFF;
            databuf[j+1] = 0xFF;
        }
        GPIO_setValue(&selectData, true);
        SPI_transfer(spiFileDesc, databuf, NULL, DATABUFLEN);
    }

    // Signal end of data transmission by sending any command. NEED THIS OTHERWISE CONTINUES FILLING REST OF SCREEN.
    GPIO_setValue(&selectData, false);
    memset(txBuf, 0, BUFFSIZE);
    memset(rxBuf, 0, BUFFSIZE);
    txBuf[0] = 0x00; // NOP
    SPI_transfer(spiFileDesc, txBuf, rxBuf, 1);

    return 0;
}
