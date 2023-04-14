CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -g -c -Werror -Wshadow -Wall -std=c99 -D _POSIX_C_SOURCE=200809L

OBJDIR = $(HOME)/cmpt433/work/cmpt433-metronome-project/obj
OUTDIR = $(HOME)/cmpt433/work/cmpt433-metronome-project/out
PUBLIC = $(HOME)/cmpt433/public/cmpt433-metronome-project

all:
	#$(CC_C) $(CFLAGS) spi-loopback-test.c -o $(PUBLIC)/spi-looback-test
	# $(CC_C) $(CFLAGS) GPIO.c Display.c Display_test.c -c $(PUBLIC)/Display_test
	# $(CC_C) $(CFLAGS) GPIO.c Graphics.c Display.c Graphics_test.c -o $(PUBLIC)/Graphics_test
	# $(CC_C) $(CFLAGS) GPIO.c Graphics.c Display.c Ui.c Ui_test.c -o $(PUBLIC)/Ui_test
	# $(CC_C) $(CFLAGS) GPIO.c spi-prototype.c -o $(PUBLIC)/spi-prototype
	$(CC_C) $(CFLAGS) Display.c -o $(OBJDIR)/Display.o
	$(CC_C) $(CFLAGS) Graphics.c -o $(OBJDIR)/Graphics.o
	$(CC_C) $(CFLAGS) Ui.c -o $(OBJDIR)/Ui.o
	# cp $(OUTDIR)/config-pin-script.sh $(PUBLIC)
