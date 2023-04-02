CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror -Wshadow -pthread
LFLAGS = -L$(HOME)/cmpt433/public/asound_lib_BBG -lpthread -lasound
#when the compile_program script runs this file, it treats it like it 
#is running from the root, so this folder name is added to each file name
OBJFILES = $(DIR)/button.o $(DIR)/GPIO.o $(DIR)/State.o $(DIR)/Ticker.o $(DIR)/Audio.o $(DIR)/button-history.o $(DIR)/main.o
#used if compiling "locally" (from this folder, not using script)
OBJFILES_MODULE = button.o GPIO.o State.o Ticker.o Audio.o button-history.o main.o 

TARGETDIR = $(HOME)/cmpt433/public/cmpt433-metronome-project

all: test-ticker
	$(CC_C) $(CFLAGS) $(OBJFILES_MODULE) -o $(TARGETDIR)/metronome $(LFLAGS)

OBJFILES_TICKER = Audio.o Ticker.o State.o Ticker_test.o

test-ticker:
	$(CC_C) $(CFLAGS) $(OBJFILES_TICKER) -o $(TARGETDIR)/Ticker_test $(LFLAGS)
