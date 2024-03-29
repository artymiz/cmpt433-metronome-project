*3/18:*

LOOPBACK TEST
- Removed Zen Cape
- Ran the following: 
```
config-pin P9_17 spi_cs
config-pin P9_18 spi
config-pin P9_21 spi
config-pin P9_22 spi_sclk
```
- Bridged P9_18 and P9_21 (MOSI and MISO) NOTE: https://community.element14.com/products/devtools/single-board-computers/next-genbeaglebone/b/blog/posts/beaglebone-enable-spi-with-overlay-and-from-command-line has them swapped and that causes all reads to be 0xfffffffff..
- Ran spi-test.c (loopback test): 
```
sudo ./spi-test -D /dev/spidev0.0
```
- Worked as expected!
- https://community.element14.com/products/devtools/single-board-computers/next-genbeaglebone/b/blog/posts/beaglebone-enable-spi-with-overlay-and-from-command-line
- Removing the wire connecting P9_18 and P9_21 stopped the test from working as expected (which is good, test is not a fluke).

WRITING CODE FOR 8-BIT READ (Read Display Power Mode -- 0xA) in spi-read.c
(as described in https://cdn-shop.adafruit.com/datasheets/ILI9340.pdf page 36)
- Doesn't work

*3/19:*

SANITY CHECK
- Without zen cape, and pins configured, used Arduino (serial monitor) to verify that
chip select was indeed being set low during the SPI transfer. `ioctl(spiFileDesc, SPI_IOC_MESSAGE(NUM_TRANSFERS), &transfer);`
sets chip select as part of its execution. NOTE that this is all while
gpioinfo says that spio0_cs0 is an input that is used. But according to tests, it is outputting
low and high voltages! spi clock also definitely does something: changing the pinmode from spi_sclk 
to default makes crazy oscilations go away, and those crazy oscilations are less crazy than a floating 
voltage reading. 

TRYING 4-LINE SERIAL INTERFACE, (not what stabie-soft uses)

- Looked at https://cdn-shop.adafruit.com/datasheets/TM022HDH26_V1.0.pdf. THEY HAVE NO MENTION OF 3-LINE SERIAL INTERFACE,
which is what stabie-soft was trying to use!
- Going to try to use 4-line serial interface, which means that D/C has to be set to low/high depending on whether
the signal being transferred is command/data. Command is low! 
FREE PINS FROM https://opencoursehub.cs.sfu.ca/bfraser/grav-cms/cmpt433/guides/files/GPIOGuide.pdf page 4
```
• P8-7 = Linux #66
• P8-8 = Linux #67
• P8-9 = Linux #69
• P8-10 = Linux #68
• P9-15 = Linux #48
• P9-23 = Linux #49
```
- 4-line serial interface reads 8 for 0xA command! This only happends when D/C pin is set LOW. 

- Nothing is working as expected, maybe the dummy return byte actually means that i'm reading one byte early? Nope, the 3rd byte is empty. 

- I think everything is actually ok, in terms of the wiring and the protocol setup, because the bytes that are being read look right,
and running vs. commenting  out  the initialization code lines changes the output of the state read!

With initialization:
```
debian@BeagleBone:/mnt/remote$ sudo ./spi-read 
00 9c 00 
00 c0 31 
```

No initialization:
```
debian@BeagleBone:/mnt/remote$ sudo ./spi-read 
00 0c 00 
00 00 30 
```

4-LINE SERIAL INTERFACE WORKS

*3/25*

THE RESET PIN IS STRONG
- Setting reset to low makes ./spi-read give all zeroes (just like when we couldn't get anything to work in the beginning) 
- Setup Reset pin: set it HIGH, then set it LOW for 10 microseconds (to reset), then HIGH. The reset pin works! Use the reset pulse to clear the screen, I don't know what else it does. 

ABLE TO WRITE DATA TO THE DISPLAY!
```
/* ---- Write image data ---- */ 
    
    // Memory write command (0x2C), 
    memset(txBuf, 0, BUFFSIZE);
    memset(rxBuf, 0, BUFFSIZE);
    txBuf[0] = 0x2C;
    SPI_transfer(spiFileDesc, txBuf, rxBuf, 1);

    for (size_t i = 0; i < 32; i++) // 320 / 5 = 64: make half the screen black.
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
    
    // Signal end of data transmission by sending any command.
    GPIO_setValue(&selectData, false);
    memset(txBuf, 0, BUFFSIZE);
    memset(rxBuf, 0, BUFFSIZE);
    txBuf[0] = 0x00; // NOP
    SPI_transfer(spiFileDesc, txBuf, rxBuf, 1);
```

- It is critical that Memory write command and NOP command are outside of the for loop, otherwise it will repeatedly
draw the first 5 lines instead of drawing half the screen.

- Data can only be overwritten: if you send an empty data write command it will not clear memory.
- Data does not persist after power disconnected, or after HW reset.

- Data is drawn as it is being sent: it's not like it gets stored in a buffer and updated when the NOP command is sent.
- The screen pixels default to being line-ey, need to write white pixels in order to make the screen white.

WHAT DO COLUMN ADDRESS SET (2Ah) AND PAGE ADDRESS SET (2Bh) DO?
- Page line in memory: A row?
- Define area where MCU can access.

```
void Adafruit_SPITFT::sendCommand(uint8_t commandByte, uint8_t *dataBytes,
                                  uint8_t numDataBytes) {
  SPI_BEGIN_TRANSACTION();
  if (_cs >= 0)
    SPI_CS_LOW();

  SPI_DC_LOW();          // Command mode
  spiWrite(commandByte); // Send the command byte

  SPI_DC_HIGH();
  for (int i = 0; i < numDataBytes; i++) {
    if ((connection == TFT_PARALLEL) && tft8.wide) {
      SPI_WRITE16(*(uint16_t *)dataBytes);
      dataBytes += 2;
    } else {
      spiWrite(*dataBytes); // Send the data bytes
      dataBytes++;
    }
  }

  if (_cs >= 0)
    SPI_CS_HIGH();
  SPI_END_TRANSACTION();
}
```

You need to have control of D/C midway transfer

According to https://github.com/adafruit/Adafruit_ILI9341/blob/master/Adafruit_ILI9341.cpp
>  @brief   Set the "address window" - the rectangle we will write to RAM with the next chunk of      SPI data writes. The ILI9341 will automatically wrap the data as each row is filled

- Set address window is used heavily in Arudino library.

BRIDGING CS TO GND DOESN'T WORK
- We need it to move down and up

*3/26*

Using p9.15 to control CS: need to do this because write cycle sequence on page 32 of https://cdn-shop.adafruit.com/datasheets/ILI9340.pdf 

Able to do everything? that could be done previously using this method!

*3/27*

By setting D/C low mid-transfer, we can use commands with parameters, for example "column address set".

Column address set and page address set effectively move the draw cursor to a certain position and cause it to wrap around differently.

spi-prototype.c: draws a square!
