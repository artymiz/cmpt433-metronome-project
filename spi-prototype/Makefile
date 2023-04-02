CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -g -Werror -Wshadow

OUTDIR = $(HOME)/cmpt433/public/cmpt433-metronome-project

all:
	$(CC_C) $(CFLAGS) GPIO.c spi-read-setmemory.c -o $(OUTDIR)/spi-read-setmemory
	$(CC_C) $(CFLAGS) spi-loopback-test.c -o $(OUTDIR)/spi-looback-test
	$(CC_C) $(CFLAGS) GPIO.c Display.c Display_test.c -o $(OUTDIR)/Display_test
	$(CC_C) $(CFLAGS) GPIO.c spi-prototype.c -o $(OUTDIR)/spi-prototype
	chmod +x config-pin-script.sh
	cp config-pin-script.sh $(OUTDIR)
