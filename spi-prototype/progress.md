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
