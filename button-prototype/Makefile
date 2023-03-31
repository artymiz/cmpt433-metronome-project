CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGSLIB = -Wall -g -c -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -pthread
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -pthread
DIR = button-prototype
OUTDIR = $(HOME)/cmpt433/work/cmpt433-metronome-project/obj

all:
	$(CC_C) $(CFLAGSLIB) $(DIR)/button.c -o button.o
	$(CC_C) $(CFLAGSLIB) $(DIR)/GPIO.c -o GPIO.o
	$(CC_C) $(CFLAGSLIB) $(DIR)/button-test.c -o button-test.o
	chmod +x $(DIR)/config-pin-script.sh
	cp $(DIR)/button.o $(OUTDIR)
	cp $(DIR)/GPIO.o $(OUTDIR)
	cp $(DIR)/button-test.o $(OUTDIR)
	cp $(DIR)/config-pin-script.sh $(OUTDIR)
module:
	$(CC_C) $(CFLAGSLIB) button.c -o $(OUTDIR)/button.o
	$(CC_C) $(CFLAGSLIB) GPIO.c -o $(OUTDIR)/GPIO.o
	$(CC_C) $(CFLAGSLIB) button-test.c -o $(OUTDIR)/button-test.o
	cp button.o $(OUTDIR)
	cp GPIO.o $(OUTDIR)
	cp button-test.o $(OUTDIR)
	chmod +x config-pin-script.sh
	cp config-pin-script.sh $(OUTDIR)