CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -pthread
LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBG -lpthread -lasound
#when the compile_program script runs this file, it treats it like it 
#is running from the root, so this folder name is added to each file name
#used if compiling "locally" (from this folder, not using script)
OBJFILES= KillSignal.o Timing.o Button.o GPIO.o State.o Ticker.o Audio.o ButtonHistory.o Metronome.o main.o 

TARGETDIR = $(HOME)/cmpt433/public/cmpt433-metronome-project

all:
	$(CC_C) $(CFLAGS) $(OBJFILES) -o $(TARGETDIR)/metronome $(LFLAGS)
