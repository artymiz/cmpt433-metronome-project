#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

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
#define SPEED_HZ_DEFAULT 500000 // Arbitrary, but 500000 is reasonable

#define READ_POWER_MODE 0x0A

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

int main(void)
{
    int spiFileDesc = SPI_initPort(SPI_DEV_BUS0_CS0);
    uint32_t send = 0b000001010 << (32 - 9); // assuming MSB first, 9 bits. 00001010 is the command, one leading zero added.
    uint32_t receive = 0xFF;
    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long)&send,
        .rx_buf = (unsigned long)&receive,
        .len = 4
    };
    int status = ioctl(spiFileDesc, SPI_IOC_MESSAGE(1), &transfer);
    if (status < 0) {
        printf("Error: SPI Transfer failed\n");
    }
    printf("send = %x", send);
    printf("receive = %x", receive);
}
