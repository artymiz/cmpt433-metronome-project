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

static int spiFileDesc = -1;

typedef struct command {
    uint8_t command_code;
    uint8_t num_params;
    uint8_t response_size;
} command_t;

static const command_t NOP                 = {0x00, 0, 0};  // No operation
static const command_t SLEEP_OUT           = {0x11, 0, 0};  // Wake up
static const command_t DISPLAY_ON          = {0x29, 0, 0};  // Wake up
static const command_t COL_ADDR_SET        = {0x2A, 4, 0};  // Column address set (for memory write)
static const command_t PAGE_ADDR_SET       = {0x2B, 4, 0};  // Row address set (for memory write)
static const command_t MEMORY_WRITE        = {0x2C, 0, 0};  // Memory write (signal start of pixel data)
static const command_t READ_POWER_MODE     = {0x0A, 0, 1};
static const command_t READ_DISPLAY_STATUS = {0x09, 0, 4};

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
    uint8_t *txBuf = malloc(bufSize); // command parameters
    uint8_t *rxBuf = malloc(bufSize); // data sent back from display
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

// https://codereview.stackexchange.com/questions/151049/endianness-conversion-in-c
uint16_t reverse16(uint16_t value)
{
    return (((value & 0x00FF) << 8) |
            ((value & 0xFF00) >> 8));
}

// https://cdn-shop.adafruit.com/datasheets/ILI9340.pdf page 14: 
// Column address set, Row address set, then memory write.
// Also same as: https://github.com/adafruit/Adafruit_ILI9341/blob/master/Adafruit_ILI9341.cpp (setAddrWindow)
void setMemoryWriteArea(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h)
{
    uint16_t x1 = x0 + w - 1;
    uint16_t y1 = y0 + h - 1;

    assert(x0 < x1);
    assert(x1 < COL_MAX); // 0 - 239 (0 indexed)
    assert(y0 < y1);
    assert(y1 < ROW_MAX); // 0 - 319 (0 indexed)
    
    // Convert from little endian to big endian.
    x0 = reverse16(x0);
    x1 = reverse16(x1);
    y0 = reverse16(y0);
    y1 = reverse16(y1);

    // Restrict columns to write.
    uint8_t colStartEnd[4];
    memset(colStartEnd, 0, 4);
    memcpy(colStartEnd, &x0, 2);
    memcpy(colStartEnd + 2, &x1, 2);
    sendCommand(COL_ADDR_SET, colStartEnd);

    // Restrict rows to write.
    uint8_t rowStartEnd[4];
    memset(rowStartEnd, 0, 4);
    memcpy(rowStartEnd, &x0, 2);
    memcpy(rowStartEnd + 2, &x1, 2);
    sendCommand(PAGE_ADDR_SET, rowStartEnd);
}

int main(void)
{
    system("./config-pin-script.sh > /dev/null");
    
    // Setup RST, and default states for CS (high), DC (high)
    INIT_GPIO
    CS_HIGH
    DC_HIGH

    // Power on sequence.
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
    sendCommand(SLEEP_OUT, NULL);
    delayMs(60);
    sendCommand(DISPLAY_ON, NULL);

    // Read power mode cmd: 0x9c (expected result!) Normal mode, Sleep out, Booster on, Display on
    uint8_t *powerMode = sendCommand(READ_POWER_MODE, NULL);
    // printCommandResponse(READ_POWER_MODE, powerMode);
    assert(powerMode[0] == 0x9C);
    free(powerMode);

    /* ---- Write white screen. ---- */
    
    sendCommand(MEMORY_WRITE, NULL);
    
    const int byteLineCount = COL_MAX * 3; // 720 bytes per line
    uint8_t databufLine[byteLineCount];
    memset(databufLine, 0xFF, byteLineCount);
    for (size_t i = 0; i < ROW_MAX; i++)
    {
        CS_LOW
        spiTransfer(databufLine, NULL, byteLineCount);
        CS_HIGH
    }
    
    sendCommand(NOP, NULL);
    
    /* ---- Write 1 black square pixels. ---- */

    const int squareEdge = 20;
    setMemoryWriteArea(squareEdge, squareEdge, squareEdge, squareEdge);
    
    sendCommand(MEMORY_WRITE, NULL);

    const int byteCount = squareEdge * squareEdge * 3; // 3 bytes per pixel
    uint8_t databufSquare[byteCount];
    memset(databufSquare, 0x00, byteCount);
    CS_LOW
    spiTransfer(databufSquare, NULL, byteCount);
    CS_HIGH

    sendCommand(NOP, NULL);

    return 0;
}
