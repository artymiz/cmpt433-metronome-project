# Makefile for building embedded application.
# by Brian Fraser

# Edit this file to compile extra C files into their own programs.

OBJDIR = $(HOME)/cmpt433/work/cmpt433-metronome-project/obj
OUTDIR = $(HOME)/cmpt433/work/cmpt433-metronome-project/out
CROSS_TOOL = arm-linux-gnueabihf-
CC_C = $(CROSS_TOOL)gcc
DIR = ticker-prototype
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror
CFLAGSLIB = -Wall -g -c -std=c99 -D _POSIX_C_SOURCE=200809L -Werror


# Asound process:
# get alibsound2 lib on target:
# 	# apt-get install libasound2
# Copy target's /usr/lib/arm-linux-gnueabihf/libasound.so.2.0.0 
#      to host  ~/public/asound_lib_BBG/libasound.so
# Copy to just base library:
LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBG

# -pg for supporting gprof profiling.
#CFLAGS += -pg

all: Ticker_test

Ticker_test:
	touch $(OUTDIR)/state.bin
	$(CC_C) $(CFLAGSLIB) $(DIR)/Audio.c -o $(OBJDIR)/Audio.o  $(LFLAGS) -lpthread -lasound
	$(CC_C) $(CFLAGSLIB) $(DIR)/State.c -o $(OBJDIR)/State.o  $(LFLAGS) -lpthread -lasound
	$(CC_C) $(CFLAGSLIB) $(DIR)/Ticker.c -o $(OBJDIR)/Ticker.o  $(LFLAGS) -lpthread -lasound
	cp $(DIR)/metronome-tick.wav $(OUTDIR)/metronome-tick.wav
module:
	touch $(OUTDIR)/state.bin
	$(CC_C) $(CFLAGSLIB) Audio.c -o $(OBJDIR)/Audio.o  $(LFLAGS) -lpthread -lasound
	$(CC_C) $(CFLAGSLIB) State.c -o $(OBJDIR)/State.o  $(LFLAGS) -lpthread -lasound
	$(CC_C) $(CFLAGSLIB) Ticker.c -o $(OBJDIR)/Ticker.o  $(LFLAGS) -lpthread -lasound
	cp metronome-tick.wav $(OUTDIR)/metronome-tick.wav
# ticker:
# 	$(CC_C) $(CFLAGS) Timing.c Audio.c ticker.c -o $(OUTDIR)/ticker  $(LFLAGS) -lpthread -lasound

# Copy wav file
wav:
	cp metronome-tick.wav $(OUTDIR)/metronome-tick.wav

State_test: State.c State_test.c
	gcc -g State.c State_test.c -o State_test
