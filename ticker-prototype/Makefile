# Makefile for building embedded application.
# by Brian Fraser

# Edit this file to compile extra C files into their own programs.

OUTDIR = $(HOME)/cmpt433/public/cmpt433-metronome-project
CROSS_TOOL = arm-linux-gnueabihf-
CC_C = $(CROSS_TOOL)gcc

CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror


# Asound process:
# get alibsound2 lib on target:
# 	# apt-get install libasound2
# Copy target's /usr/lib/arm-linux-gnueabihf/libasound.so.2.0.0 
#      to host  ~/public/asound_lib_BBG/libasound.so
# Copy to just base library:
LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBG

# -pg for supporting gprof profiling.
#CFLAGS += -pg

all: wav ticker

clean:
	rm $(OUTDIR)/*

ticker:
	$(CC_C) $(CFLAGS) Timing.c Audio.c ticker.c -o $(OUTDIR)/ticker  $(LFLAGS) -lpthread -lasound

# Copy wav file
wav:
	cp metronome-tick.wav $(OUTDIR)/metronome-tick.wav
