CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGSLIB = -Wall -g -c -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -pthread
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -pthread
OBJDIR = $(HOME)/cmpt433/work/cmpt433-metronome-project/obj
OUTDIR = $(HOME)/cmpt433/work/cmpt433-metronome-project/out

all:
	$(CC_C) $(CFLAGSLIB) GPIO.c -o $(OBJDIR)/GPIO.o
	$(CC_C) $(CFLAGSLIB) KillSignal.c -o $(OBJDIR)/KillSignal.o
	$(CC_C) $(CFLAGSLIB) Timing.c -o $(OBJDIR)/Timing.o
	$(CC_C) $(CFLAGSLIB) UDPMessageController.c -o $(OBJDIR)/UDPMessageController.o
	cp config-pin-script.sh $(OUTDIR)/config-pin-script.sh