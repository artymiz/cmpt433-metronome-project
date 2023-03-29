CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -pthread

OUTDIR = $(HOME)/cmpt433/public/cmpt433-metronome-project

all:
	$(CC_C) $(CFLAGS) button.c GPIO.c button-test.c -o $(OUTDIR)/button_test
	chmod +x config-pin-script.sh
	cp config-pin-script.sh $(OUTDIR)
clean:
	rm $(OUTDIR)/button_test