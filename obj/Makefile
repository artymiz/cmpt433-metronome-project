CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -pthread
LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBG -lpthread -lasound
DIR = obj
#when the compile_program script runs this file, it treats it like it 
#is running from the root, so this folder name is added to each file name
OBJFILES = $(DIR)/button.o $(DIR)/GPIO.o $(DIR)/button-test.o $(DIR)/State.o $(DIR)/Ticker.o $(DIR)/Audio.o 
#used if compiling "locally" (from this folder, not using script)
OBJFILES_MODULE = button.o GPIO.o button-test.o State.o Ticker.o Audio.o 

OUTDIR = $(HOME)/cmpt433/public/cmpt433-metronome-project

all:
	$(CC_C) $(CFLAGS) $(OBJFILES) -o $(OUTDIR)/metronome $(LFLAGS)
	cp $(DIR)/metronome-tick.wav $(OUTDIR)/metronome-tick.wav
module:
	$(CC_C) $(CFLAGS) $(OBJFILES_MODULE) -o $(OUTDIR)/metronome $(LFLAGS)
	cp metronome-tick.wav $(OUTDIR)/metronome-tick.wav