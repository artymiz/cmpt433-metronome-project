# Makefile for building embedded application.
# by Brian Fraser

# Edit this file to compile extra C files into their own programs.

OBJDIR = $(HOME)/cmpt433/work/cmpt433-metronome-project/obj
OUTDIR = $(HOME)/cmpt433/work/cmpt433-metronome-project/out
CROSS_TOOL = arm-linux-gnueabihf-
CC_C = $(CROSS_TOOL)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror
CFLAGSLIB = -Wall -g -c -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

all: 
	$(CC_C) $(CFLAGSLIB) Audio.c -o $(OBJDIR)/Audio.o  $(LFLAGS) -lpthread -lasound
	$(CC_C) $(CFLAGSLIB) State.c -o $(OBJDIR)/State.o  $(LFLAGS) -lpthread -lasound
	$(CC_C) $(CFLAGSLIB) State_test.c -o $(OBJDIR)/State_test.o  $(LFLAGS) -lpthread -lasound
	$(CC_C) $(CFLAGSLIB) Ticker.c -o $(OBJDIR)/Ticker.o  $(LFLAGS) -lpthread -lasound
	$(CC_C) $(CFLAGSLIB) Ticker_test.c -o $(OBJDIR)/Ticker_test.o  $(LFLAGS) -lpthread -lasound
	cp metronome-tick.wav $(OUTDIR)/metronome-tick.wav
	cp State.txt $(OUTDIR)/State.txt
