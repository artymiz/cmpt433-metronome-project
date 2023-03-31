#!/bin/sh
TARGETDIR = $HOME/cmpt433/public/cmpt433-metronome-project

echo "\e[40m\e[92m [Making and binning o files from button-prototype] \e[0m"
make -f button-prototype/Makefile

echo "\e[40m\e[92m [Making and binning o files from ticker-prototype] \e[0m"
make -f ticker-prototype/Makefile

echo "\e[40m\e[92m [Making and binning main.o] \e[0m"
make -f Makefile

echo "\e[40m\e[92m [Linking libraries in bin and sending executable to target] \e[0m"
make -f obj/Makefile

echo "\e[40m\e[92m [Sending out (.sh, .wav, etc) files to target] \e[0m"
	sudo chmod +x out/config-pin-script.sh
	sudo cp out/config-pin-script.sh $TARGETDIR/config-pin-script.sh
	sudo cp out/metronome-tick.wav $TARGETDIR/metronome-tick.wav
	sudo cp out/state.bin $TARGETDIR/state.bin

