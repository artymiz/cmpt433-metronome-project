CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGSLIB = -Wall -g -c -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -pthread
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -pthread
OBJDIR = $(HOME)/cmpt433/work/cmpt433-metronome-project/obj
OUTDIR = $(HOME)/cmpt433/work/cmpt433-metronome-project/out

all:
	$(CC_C) $(CFLAGSLIB) Button.c -o $(OBJDIR)/Button.o
	$(CC_C) $(CFLAGSLIB) ButtonHistory.c -o $(OBJDIR)/ButtonHistory.o
	chmod +x config-pin-script.sh
	cp config-pin-script.sh $(OUTDIR)