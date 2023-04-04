#config pins for the screen
config-pin P8_10 gpio       # RST (reset)
config-pin P9_15 gpio       # CS (chip select)
config-pin P9_18 spi        # MOSI (master out slave in)
config-pin P9_21 spi        # MISO (master in slave out)
config-pin P9_22 spi_sclk   # CLK (clock)
config-pin P9_23 gpio       # D/C (data/command)

#config pins for the buttons
sudo echo 66 > /sys/class/gpio/export
sudo echo 67 > /sys/class/gpio/export
sudo echo 69 > /sys/class/gpio/export
sudo echo 68 > /sys/class/gpio/export
sudo echo 45 > /sys/class/gpio/export

config-pin p8.7 gpio # gpio 66
config-pin p8.8 gpio # gpio 67
config-pin p8.9 gpio # gpio 69
config-pin p8.10 gpio # gpio 68
config-pin p8.11 gpio # gpio 45
