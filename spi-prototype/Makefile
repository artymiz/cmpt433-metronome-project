CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -g -Werror -Wshadow

OBJDIR = $(HOME)/cmpt433/work/cmpt433-metronome-project/obj
OUTDIR = $(HOME)/cmpt433/work/cmpt433-metronome-project/out

all:
	$(CC_C) $(CFLAGS) spi-loopback-test.c -o $(OUTDIR)/spi-looback-test
	$(CC_C) $(CFLAGS) Display.c -o $(OBJDIR)/Display.o
	$(CC_C) $(CFLAGS) Display_test.c -o $(OBJDIR)/Display_test.o
	$(CC_C) $(CFLAGS) GPIO.c spi-prototype.c -o $(OUTDIR)/spi-prototype
	chmod +x config-pin-script.sh
	cp config-pin-script.sh $(OUTDIR)
