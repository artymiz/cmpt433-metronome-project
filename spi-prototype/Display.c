#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include "../utility/GPIO.h"
#include "Display.h"

#define CS_PIN 48
#define CS_LOW GPIO_setValue(CS_PIN, false);
#define CS_HIGH GPIO_setValue(CS_PIN, true);

#define RST_PIN 44
#define RST_LOW GPIO_setValue(RST_PIN, false);
#define RST_HIGH GPIO_setValue(RST_PIN, true);

#define DC_PIN 49
#define DC_LOW GPIO_setValue(DC_PIN, false);
#define DC_HIGH GPIO_setValue(DC_PIN, true);

#define SPI_MAX_LEN 4096 // max len bytes in `spi_ioc_transfer`
#define SPI_DEVICE "/dev/spidev0.0"

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
// static const command_t READ_DISPLAY_STATUS = {0x09, 0, 4};

static void delayMs(long long ms)
{
    const long long NS_PER_MS = 1000000;
    const long long NS_PER_SECOND = 1000000000;
    long long ns = ms * NS_PER_MS;
    struct timespec ts = {ns / NS_PER_SECOND, ns % NS_PER_SECOND};
    nanosleep(&ts, (struct timespec *)NULL);
}

static void initGpio()
{
    GPIO_pinMode(CS_PIN, true);
    GPIO_pinMode(RST_PIN, true);
    GPIO_pinMode(DC_PIN, true);
    // These are the default states of the pins,
    // all functions must return them to this state
    // when they exit.
    CS_HIGH
    DC_HIGH
    RST_HIGH
}

static void hardwareReset()
{
    // Based on: https://cdn-shop.adafruit.com/datasheets/TM022HDH26_V1.0.pdf page 12.
    delayMs(100);
    RST_LOW
    delayMs(10);
    RST_HIGH
    delayMs(20);
}

static void spiOpen()
{
    // Open Device
    spiFileDesc = open(SPI_DEVICE, O_RDWR);
    if (spiFileDesc < 0) {
        printf("Error: Can't open device %s\n", SPI_DEVICE);
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

static void spiClose()
{
    close(spiFileDesc);
}

// Does not touch CS or D/C
static void spiTransfer(uint8_t *sendBuf, uint8_t *receiveBuf, int length)
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

// Commands starting with READ require rxBuf to be freed (non null return from sendCommand)
static uint8_t *sendCommand(command_t c, uint8_t *params)
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
void setWriteArea(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h)
{
    static uint16_t old_x0 = 0xffff, old_x1 = 0xffff;
    static uint16_t old_y0 = 0xffff, old_y1 = 0xffff;

    uint16_t x1 = x0 + w - 1;
    uint16_t y1 = y0 + h - 1;

    if (x0 != old_x0 || x1 != old_x1)
    {
        assert(x0 < x1);
        assert(x1 < COL_MAX); // 0 - 239 (0 indexed)
        old_x0 = x0;
        old_x1 = x1;

        // Convert from little endian to big endian.
        uint16_t x0_be = reverse16(x0);
        uint16_t x1_be = reverse16(x1);

        // Restrict columns to write.
        uint8_t colStartEnd[4];
        memset(colStartEnd, 0, 4);
        memcpy(colStartEnd, &x0_be, 2);
        memcpy(colStartEnd + 2, &x1_be, 2);
        sendCommand(COL_ADDR_SET, colStartEnd);
    }

    if (y0 != old_y0 || y1 != old_y1)
    {
        assert(y0 < y1);
        assert(y1 < ROW_MAX); // 0 - 239 (0 indexed)
        old_y0 = y0;
        old_y1 = y1;

        // Convert from little endian to big endian.
        uint16_t y0_be = reverse16(y0);
        uint16_t y1_be = reverse16(y1);

        // Restrict columns to write.
        uint8_t rowStartEnd[4];
        memset(rowStartEnd, 0, 4);
        memcpy(rowStartEnd, &y0_be, 2);
        memcpy(rowStartEnd + 2, &y1_be, 2);
        sendCommand(PAGE_ADDR_SET, rowStartEnd);
    }
}

// Setup all pins, turn on display, clear its memory and make it all white.
void Display_init()
{
    system("./config-pin-script.sh > /dev/null");
    initGpio();
    spiOpen();
    hardwareReset();
    sendCommand(SLEEP_OUT, NULL);
    delayMs(60);
    sendCommand(DISPLAY_ON, NULL);

    // Make sure everything is working.
    uint8_t *powerMode = sendCommand(READ_POWER_MODE, NULL);
    assert(powerMode[0] == 0x9C);
    free(powerMode);

    // Make the screen white.
    const int whiteBufLen = COL_MAX * ROW_MAX * 3;
    uint8_t whiteBuf[whiteBufLen];
    memset(whiteBuf, 0xFF, whiteBufLen);
    Display_memoryWrite(whiteBuf, 0, 0, COL_MAX, ROW_MAX);
}

// copy n elements from src to dst
// srcOffset is incremented for every element copied
static size_t copy_n(uint8_t *dst, uint8_t *src,
        size_t *srcOffset, size_t srcLen, size_t n)
{
    size_t src_i = *srcOffset;
    size_t dst_i = 0;

    for (; src_i < srcLen && dst_i < n; dst_i++, src_i++) {
        dst[dst_i] = src[src_i];
    }
    *srcOffset = src_i;

    return dst_i;
}

void Display_memoryWrite(uint8_t *buff, uint16_t x0, uint16_t y0, uint16_t w, uint16_t h)
{
    setWriteArea(x0, y0, w, h);

    sendCommand(MEMORY_WRITE, NULL);

    const size_t buffSize = w * h * 3;
    size_t buffIdx = 0;
    uint8_t wrtBuf[SPI_MAX_LEN];
    size_t wrtBytes;

    while ((wrtBytes = copy_n(wrtBuf, buff, &buffIdx, buffSize, SPI_MAX_LEN))) {
        CS_LOW
        spiTransfer(wrtBuf, NULL, wrtBytes);
        CS_HIGH
    }

    sendCommand(NOP, NULL);
}

void Display_cleanup()
{
    spiClose();
}
