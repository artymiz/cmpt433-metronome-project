3/18:

LOOPBACK TEST
- Removed Zen Cape
- Ran the following: 
```
config-pin P9_17 spi_cs
config-pin P9_18 spi
config-pin P9_21 spi
config-pin P9_22 spi_sclk
```
- Bridged P9_18 and P9_21 (MOSI and MISO)
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

3/19:


