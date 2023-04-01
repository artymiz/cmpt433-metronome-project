CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGSLIB = -Wall -g -c -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -pthread
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -pthread
DIR = button-prototype
OBJDIR = $(HOME)/cmpt433/work/cmpt433-metronome-project/obj
OUTDIR = $(HOME)/cmpt433/work/cmpt433-metronome-project/out

all:
	$(CC_C) $(CFLAGSLIB) $(DIR)/button.c -o $(OBJDIR)/button.o
	$(CC_C) $(CFLAGSLIB) $(DIR)/GPIO.c -o $(OBJDIR)/GPIO.o
	$(CC_C) $(CFLAGSLIB) $(DIR)/button-history.c -o $(OBJDIR)/button-history.o
	chmod +x $(DIR)/config-pin-script.sh
	cp $(DIR)/config-pin-script.sh $(OUTDIR)
module:
	$(CC_C) $(CFLAGSLIB) button.c -o $(OBJDIR)/button.o
	$(CC_C) $(CFLAGSLIB) GPIO.c -o $(OBJDIR)/GPIO.o
	$(CC_C) $(CFLAGSLIB) button-history.c -o $(OBJDIR)/button-history.o
	chmod +x config-pin-script.sh
	cp config-pin-script.sh $(OUTDIR)