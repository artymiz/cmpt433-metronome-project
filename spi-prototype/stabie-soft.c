// copied from: http://www.stabie-soft.com/beagle/beagleSpi.html

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

typedef struct cmdStruct
{
    char *name;
    unsigned char cmd;      /* the command byte code */
    unsigned char w;        /* 1: write command, 0: read command*/
    unsigned char numParam; /* 255 means variable number of parameters */
    unsigned char numDummy; /* For read commands */
} cmdStruct;

cmdStruct commands[] = {
    {"NOOP",     0x0, 1, 0, 0},
    {"RESET",    0x1, 1, 0, 0},
    {"RDID",     0x4, 0, 3, 0},
    {"RSTATUS",  0x9, 0, 4, 0},
    {"RPWR",     0xa, 0, 1, 0},
    {"RMAD",     0xb, 0, 1, 0},
    {"RPixFmt",  0xc, 0, 1, 0},
    {"RDImFmt",  0xd, 0, 1, 0},
    {"RSigMde",  0xe, 0, 1, 0},
    {"RDiag",    0xf, 0, 1, 0},
    {"Sleep",    0x10, 1, 0, 0},
    {"Wake",     0x11, 1, 0, 0},
    {"Partial",  0x12, 1, 0, 0},
    {"Normal",   0x13, 1, 0, 0},
    {"InvOff",   0x20, 1, 0, 0},
    {"InvOn",    0x21, 1, 0, 0},
    {"GammaSet", 0x26, 1, 1, 0},
    {"DOff",     0x28, 1, 0, 0},
    {"DOn",      0x29, 1, 0, 0},
    {"ColSet",   0x2a, 1, 4, 0},
    {"RowSet",   0x2b, 1, 4, 0},
    {"MemWrt",   0x2c, 1, 255, 0},
    {"ClrSet",   0x2d, 1, 9, 0},
    {"MemRd",    0x2e, 0, 255, 9},
    {"ParArea",  0x30, 1, 4, 0},
    {"VScroll",  0x33, 1, 6, 0},
    {"TearOff",  0x34, 1, 0, 0},
    {"TearOn",   0x35, 1, 1, 0},
    {"MemAcc",   0x36, 1, 1, 0},
    {"VscrollStart",     0x37, 1, 2, 0},
    {"IdleOff",  0x38, 1, 0, 0},
    {"IdleOn",   0x39, 1, 0, 0},
    {"PixFmt",   0x3a, 1, 1, 0},
    {"MemWrtC",  0x3c, 1, 255, 0},
    {"MemRdC",   0x3e, 0, 255, 9},
    {"SetTear",  0x44, 1, 2, 0},
    {"GetScanL", 0x45, 0, 2, 1},
    {"DBright",  0x51, 1, 1, 0},
    {"RDBright", 0x52, 0, 1, 1},
    {"CTRLDisp", 0x53, 1, 1, 0},
    {"RCTRLDisp",0x54, 0, 1, 0},
    {"CACBright",0x55, 1, 1, 0},
    {"RCACBright",   0x56, 0, 1, 1},
    {"CACMinBright", 0x5e, 1, 1, 0},
    {"RCACMinBright",0x5f, 0, 1, 1},
    {"RID1",     0xda, 0, 1, 1},
    {"RID2",     0xdb, 0, 1, 1},
    {"RID3",     0xdc, 0, 1, 1},
    {"RGBIf",    0xb0, 1, 1, 0},
    {"FmCtlN",   0xb1, 1, 2, 0},
    {"FmCtlI",   0xb2, 1, 2, 0},
    {"FmCtlP",   0xb3, 1, 2, 0},
    {"DispInv",  0xb4, 1, 1, 0},
    {"BlkPorch",     0xb5, 1, 4, 0},
    {"DispFuncCtl",  0xb6, 1, 4, 0},
    {"EntryMode",    0xb7, 1, 1, 0},
    {"BLight1",  0xb8, 1, 1, 0},
    {"BLight2",  0xb9, 1, 1, 0},
    {"BLight3",  0xba, 1, 1, 0},
    {"BLight4",  0xbb, 1, 1, 0},
    {"BLight5",  0xbc, 1, 1, 0},
    {"BLight7",  0xbe, 1, 1, 0},
    {"BLight8",  0xbf, 1, 1, 0},
    {"PwrCtl1",  0xc0, 1, 2, 0},
    {"PwrCtl2",  0xc1, 1, 1, 0},
    {"PwrCtl3",  0xc2, 1, 1, 0},
    {"PwrCtl4",  0xc3, 1, 1, 0},
    {"PwrCtl5",  0xc4, 1, 1, 0},
    {"VComCtl1", 0xc5, 1, 2, 0},
    {"VComCtl2", 0xc7, 1, 1, 0},
    {NULL, 0, 0, 0}};
/* NV not listed */

static void pabort(const char *s)
{
    perror(s);
    abort();
}

static const char *device = "/dev/spidev1.0";
static uint8_t mode = 0;
static uint8_t bits = 9;
static uint32_t speed = 5000000;
int fd;

/* len to spi is 2X size since we are writing 9 bits. Note due to ARM byte order, even bytes
** store the ninth bit instead of the odd bytes */
/* max of 19 parameters */
/* page 34 of https://cdn-shop.adafruit.com/datasheets/ILI9340.pdf describes the structure of data transmission. */
/* using 3-line serial interface protocol (D/CX not used) */
void writeCmd(int cmd, unsigned int *parameters, unsigned int numParam)
{
    struct spi_ioc_transfer tr[2];
    unsigned char tx[40];
    int i, j;

    memset(tr, 0, sizeof(struct spi_ioc_transfer) * 2);
    tr[0].tx_buf = (unsigned long)tx;
    tr[0].len = 2 * (1 + numParam);
    tx[1] = 0; /* A command so first bit is 0 -- D/C (data/command) select bit */
    tx[0] = cmd & 0xff;
    for (i = 0, j = 2; i < numParam; i++)
    {
        tx[j++] = parameters[i];
        tx[j++] = 1; /* Mark as data */
    }
    /* Send 1 (1) message */
    i = ioctl(fd, SPI_IOC_MESSAGE(1), tr);
    if (i < 1)
        pabort("can't send spi message");
}

void writePixels(unsigned char cmd, int numPixels, unsigned char *red, unsigned char *green, unsigned char *blue)
{
    static unsigned char buf[4096]; /* 682 pixels x 3 + 1(for wr cmd) *2 for 2 bytes per byte */
    struct spi_ioc_transfer tr[2];
    int ret, i, j;

    memset(tr, 0, sizeof(struct spi_ioc_transfer) * 2);
    tr[0].tx_buf = (unsigned long)buf;
    buf[1] = 0;   /* A command so first bit is 0 */
    buf[0] = cmd; /* Write or write continue command */

    for (i = 0, j = 2; i < numPixels; i++)
    {
        buf[j++] = red[i];
        buf[j++] = 1; /* data word */
        buf[j++] = green[i];
        buf[j++] = 1; /* data word */
        buf[j++] = blue[i];
        buf[j++] = 1; /* data word */
    }
    tr[0].len = j;
    /* Send 1 (1) message */
    ret = ioctl(fd, SPI_IOC_MESSAGE(1), tr);
    if (ret < 1)
        pabort("can't send spi message");
}

void writeRepeatedPixels(int numPixels, unsigned char red, unsigned char green, unsigned char blue)
{
    unsigned char rbuf[682], gbuf[682], bbuf[682];
    unsigned char cmd;

    memset(rbuf, red, 682);
    memset(gbuf, green, 682);
    memset(bbuf, blue, 682);
    cmd = 0x2c;
    while (numPixels > 682)
    {
        writePixels(cmd, 682, rbuf, gbuf, bbuf);
        numPixels -= 682;
        cmd = 0x3c;
    }
    if (numPixels)
        writePixels(cmd, numPixels, rbuf, gbuf, bbuf);
}

/* Max of 2000 parameters, note max transaction size is 4K so can't do much more anyway */
/* Pixel reads can use the continue command to continue reading pixels */
/* numBitSkip is how many bits should be ignored on the read back */
/* A memory read skips 9 clocks for example, and a 0x09 skips 1. */
void readCmd(int cmd, int numRX, int numBitSkip)
{
    struct spi_ioc_transfer tr[2];
    unsigned char tx[20];
    unsigned char rx[4096], txrx[4096];
    int bitIndex1, bitIndex2, byteIndex;
    unsigned int byteMask1, byteMask2, byte;
    int i, ret;

    memset(tr, 0, sizeof(struct spi_ioc_transfer) * 2);
    tr[0].tx_buf = (unsigned long)tx;
    tr[0].len = 2;
    tx[1] = 0; /* A command so first bit is 0 */
    tx[0] = cmd & 0xff;

    /* Weird, reads do 8 bits not 9 */
    memset(txrx, 1, numRX + numBitSkip / 8);
    tr[1].tx_buf = (unsigned long)txrx;
    tr[1].rx_buf = (unsigned long)rx;
    tr[1].len = numRX + numBitSkip / 8;
    tr[1].bits_per_word = 8;

    /* Send 2 (2) messages */
    ret = ioctl(fd, SPI_IOC_MESSAGE(2), tr);
    if (ret < 1)
        pabort("can't send spi message");
    bitIndex1 = (numBitSkip % 8);
    bitIndex2 = 8 - (numBitSkip % 8);
    byteIndex = numBitSkip / 8;

    if (bitIndex1 == 0) /* Easy just return as is */
    {
        for (i = 0; i < numRX; i++)
            printf(" %x",    rx[i + byteIndex]);
    }
    else
    { /* Must shift */
        byteMask1 = byteMask2 = 0;
        for (i = 0; i < bitIndex2; i++)
            byteMask1 |= 1 << i;
        for (i = 0; i < bitIndex1; i++)
            byteMask2 |= 1 << i;
        for (i = 0; i < numRX; i++)
        {
            byte = (byteMask1 & rx[i + byteIndex]) << bitIndex1;
            byte = byte | (byteMask2 & (rx[i + 1 + byteIndex] >> bitIndex2));
            printf(" %x",    byte);
        }
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    int i, ret = 0;
    unsigned int cmd, cnt, values[100];

    fd = open(device, O_RDWR);
    if (fd < 0)
        pabort("can't open device");

    /*
     * spi mode
     */
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
        pabort("can't set spi mode");

    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
        pabort("can't get spi mode");

    /*
     * bits per word
     */
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't set bits per word");

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't get bits per word");

    /*
     * max speed hz
     */
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't set max speed hz");

    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't get max speed hz");

    printf("mode: %d\n",     mode);
    printf("bits per word: %d\n",    bits);
    printf("max speed: %d Hz (%d KHz)\n",    speed, speed / 1000);
    printf("Calling init routine\n");

    printf("Reset\n");
    writeCmd(0x01, NULL, 0);
    sleep(1);
    printf("Wake\n");
    writeCmd(0x11, NULL, 0); /* Exit sleep */
    sleep(1);
    printf("Display on\n");
    writeCmd(0x29, NULL, 0);
    printf("Write command to read display status\n");
    readCmd(0x09, 5, 0);

    printf(" Command hex value to execute : ");
    while (scanf("%x",   &cmd) == 1)
    {
        // Loops through commands until the correct
        // command is found (i is set).
        for (i = 0; commands[i].name; i++)
            if (commands[i].cmd == cmd)
                break;

        if (commands[i].name == NULL)
        {
            printf("Unknown command\n");
            printf(" Command hex value to execute : ");
            continue;
        }

        if (commands[i].w == 0)
        { /* Read command */
            if (commands[i].numParam == 255)
            {
                printf("Number to retrieve for %s ",     commands[i].name);
                if (scanf("%x",  &cnt) != 1)
                    exit(0);
                readCmd(cmd, cnt, commands[i].numDummy);
            }
            else
            {
                printf("Result of %s read :",    commands[i].name);
                readCmd(cmd, commands[i].numParam, commands[i].numDummy);
            }
        }

        // commands[i].w == 1, Write command
        else
        {
            if (commands[i].numParam == 0)
            {
                printf("Execute %s\n",   commands[i].name);
                writeCmd(cmd, NULL, 0);
            }
            else if (commands[i].numParam != 255)
            { /* Pixels are always just written as groups of BGR */
                printf("Collect parameters for %s\n",    commands[i].name);
                for (cnt = 0; cnt < commands[i].numParam; cnt++)
                {
                    printf("Param #%d : ",   cnt);
                    if (scanf("%x",  &values[cnt]) != 1)
                        exit(0);
                    writeCmd(cmd, values, commands[i].numParam);
                }
            }
            else
            { /* Pixels */
                printf("Collect pixels for %s\n",    commands[i].name);
                printf("RepeatTransmit bytes(BGR) : ");
                if (scanf("%x %x %x",    &values[0], &values[1], &values[2]) != 3)
                    exit(0);
                printf("How many repititions : ");
                if (scanf("%d",  &cnt) != 1)
                    exit(0);
                writeRepeatedPixels(cnt, values[0], values[1], values[2]);
            }
        }
        printf(" Command hex value to execute : ");
    }
    exit(1);
}
