CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGSLIB = -Wall -g -c -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -pthread
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -pthread
DIR = 
OUTDIR = $(HOME)/cmpt433/work/cmpt433-metronome-project/obj


all:
	$(CC_C) $(CFLAGSLIB) main.c -o $(OUTDIR)/main.o
	$(CC_C) $(CFLAGSLIB) Metronome.c -o $(OUTDIR)/Metronome.o
