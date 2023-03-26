CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -g -Werror -Wshadow

OUTDIR = $(HOME)/cmpt433/public/cmpt433-metronome-project

all:
	$(CC_C) $(CFLAGS) GPIO.c spi-read.c -o $(OUTDIR)/spi-read
	$(CC_C) $(CFLAGS) spi-test.c -o $(OUTDIR)/spi-test
	chmod +x config-pin-script.sh
	cp config-pin-script.sh $(OUTDIR)
