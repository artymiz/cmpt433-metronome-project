#config pins for the screen
# config-pin P9_17 spi_cs     # CS (chip select)
config-pin P8_10 gpio       # RST (reset)
config-pin P9_15 gpio       # CS (chip select)
config-pin P9_18 spi        # MOSI (master out slave in)
config-pin P9_21 spi        # MISO (master in slave out)
config-pin P9_22 spi_sclk   # CLK (clock)
config-pin P9_23 gpio       # D/C (data/command)

#config pins for the buttons
config-pin P8_07 gpio #gpio 66
sudo echo 66 > /sys/class/gpio/export
#config-pin P8_08 gpio #gpio 67
#config-pin P8_09 gpio #gpio 69