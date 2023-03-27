#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <time.h>
#include "GPIO.h"

gpioInfo_t csPin = {.pin={.header=9, .number=15}, .gpioNumber=48};
#define CS_LOW GPIO_setValue(&csPin, false);
#define CS_HIGH GPIO_setValue(&csPin, true);

gpioInfo_t rstPin = {.pin={.header=8, .number=10}, .gpioNumber=68};
#define RST_LOW GPIO_setValue(&rstPin, false);
#define RST_HIGH GPIO_setValue(&rstPin, true);

gpioInfo_t dcPin = {.pin={.header=9, .number=23}, .gpioNumber=49};
#define DC_LOW GPIO_setValue(&dcPin, false);
#define DC_HIGH GPIO_setValue(&dcPin, true);

#define INIT_GPIO GPIO_usePin(&csPin, "out"); GPIO_usePin(&rstPin, "out"); GPIO_usePin(&dcPin, "out");

static int spiFileDesc = -1;

typedef struct command {
    uint8_t command_code;
    uint8_t num_params;
    uint8_t response_size;
} command_t;

command_t NOP = {0x00, 0, 0}; // No operation
command_t SLEEP_OUT = {0x11, 0, 0}; // Wake up
command_t COL_ADDR_SET = {0x2A, 4, 0}; // Column address set (for memory write)
command_t PAGE_ADDR_SET = {0x2A, 4, 0}; // Row address set (for memory write)
command_t MEMORY_WRITE = {0x2C, 0, 0}; // Memory write (signal start of pixel data)
command_t READ_POWER_MODE = {0x0A, 0, 1};
command_t READ_DISPLAY_STATUS = {0x09, 0, 4};

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


void delayMs(long long ms)
{
    const long long NS_PER_MS = 1000000;
    const long long NS_PER_SECOND = 1000000000;
    long long ns = ms * NS_PER_MS;
    struct timespec ts = {ns / NS_PER_SECOND, ns % NS_PER_SECOND};
    nanosleep(&ts, (struct timespec *)NULL);
}

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

void printbuf(uint8_t *buf, int length)
{
    for (size_t i = 0; i < length; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

void printCommandResponse(command_t c, uint8_t *buf)
{
    printbuf(buf, c.response_size);
}

// Commands starting with READ require rxBuf to be freed (non null return from sendCommand)
uint8_t *sendCommand(command_t c, uint8_t *params)
{   
    size_t txBufSize = c.num_params;
    size_t rxBufSize = c.response_size;
    size_t bufSize = txBufSize > rxBufSize ? txBufSize : rxBufSize; // max
    uint8_t *txBuf = malloc(bufSize);
    uint8_t *rxBuf = malloc(bufSize);    
    memset(txBuf, 0, bufSize);
    memset(rxBuf, 0, bufSize);

    for (size_t i = 0; i < c.num_params; i++)
    {
        txBuf[i] = params[i];
    }

    CS_LOW
    DC_LOW
    spiTransfer(&c.command_code, NULL, 1);
    DC_HIGH
    spiTransfer(txBuf, rxBuf, bufSize);
    CS_HIGH

    free(txBuf);
    if (c.response_size == 0)
    {
        free(rxBuf);
        return NULL;
    }
    return rxBuf;
}

int main(void)
{
    system("./config-pin-script.sh");
    
    // Setup RST, and default states for CS (high), DC (high)
    INIT_GPIO
    CS_HIGH
    DC_HIGH

    /* ---- Power on sequence ---- */
    // Based on: https://cdn-shop.adafruit.com/datasheets/TM022HDH26_V1.0.pdf page 13.

    // Setup Reset pin: set it HIGH, then set it LOW for 10 microseconds (to reset), then HIGH.
    // Based on: https://cdn-shop.adafruit.com/datasheets/TM022HDH26_V1.0.pdf page 12.
    // gpioInfo_t rstPin = {.pin={.header=8, .number=10}, .gpioNumber=68};
    RST_HIGH
    delayMs(100);
    RST_LOW
    delayMs(10);
    RST_HIGH
    delayMs(20);

    spiInit("/dev/spidev0.0");

    // Read power mode cmd: 0x08 (expected result!) Normal mode
    uint8_t *powerMode;
    powerMode = sendCommand(READ_POWER_MODE, NULL);
    printCommandResponse(READ_POWER_MODE, powerMode);
    free(powerMode);

    // Wake up
    sendCommand(SLEEP_OUT, NULL);

    // Read power mode cmd: 0x98 (expected result!) Normal mode, Sleep out, Booster on
    powerMode = sendCommand(READ_POWER_MODE, NULL);
    printCommandResponse(READ_POWER_MODE, powerMode);
    free(powerMode);

    return 0;
}
