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

#define COL_MAX 240
#define ROW_MAX 320

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
static const command_t READ_DISPLAY_STATUS = {0x09, 0, 4};

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

static int spiOpen()
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

static int spiClose()
{
    close(spiFileDesc);
}

static void delayMs(long long ms)
{
    const long long NS_PER_MS = 1000000;
    const long long NS_PER_SECOND = 1000000000;
    long long ns = ms * NS_PER_MS;
    struct timespec ts = {ns / NS_PER_SECOND, ns % NS_PER_SECOND};
    nanosleep(&ts, (struct timespec *)NULL);
}

// Setup all pins, turn on display, clear its memory and make it all white.
void DisplayInit()
{
    system("./config-pin-script.sh > /dev/null");
    initGpio();
    spiOpen();
    hardwareReset();
}