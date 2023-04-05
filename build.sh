#!/bin/sh
TARGETDIR=$HOME/cmpt433/public/cmpt433-metronome-project

echo "\e[40m\e[92m [Making and binning o files from button-prototype] \e[0m"
	cd button-prototype
	if make -f Makefile
	then
		echo "\e[40m\e[92m [Finished] \e[0m"
	else
		echo "\e[40m\e[31m [Build failed to compile button-prototype] \e[0m"
	fi
	cd ..

echo "\e[40m\e[92m [Making and binning o files from ticker-prototype] \e[0m"
	cd ticker-prototype
	if make -f Makefile
	then
		echo "\e[40m\e[92m [Finished] \e[0m"
	fi
	cd ..

echo "\e[40m\e[92m [Making and binning o files from utility] \e[0m"
	cd utility
	if make -f Makefile
	then
		echo "\e[40m\e[92m [Finished] \e[0m"
	fi
	cd ..

echo "\e[40m\e[92m [Making and binning main.o + Metronome.o] \e[0m"
	if make -f Makefile
	then
		echo "\e[40m\e[92m [Finished] \e[0m"
	else
		echo "\e[40m\e[31m [Build failed to compile ticker-prototype] \e[0m"
	fi

echo "\e[40m\e[92m [Linking libraries in bin and sending executable to target] \e[0m"
	cd obj
	if	make -f Makefile
	then
		echo "\e[40m\e[92m [Finished] \e[0m"
	else
		echo "\e[40m\e[31m [Build failed at linking stage] \e[0m"
	fi
	cd ..

echo "\e[40m\e[92m [Sending out (.sh, .wav, etc) files to target] \e[0m"
	echo "chmod on config-pin-script.sh"
	sudo chmod +x out/config-pin-script.sh
	echo "sending config-pin-script.sh to target"
	cp out/config-pin-script.sh $TARGETDIR/config-pin-script.sh
	echo "sending ticks (0.wav, 1.wav ...) to target"
	cp -r out/ticks/ $TARGETDIR
	echo "sending server files to target"
	cp -r server $TARGETDIR
	echo "sending State.txt to target"
	if cp out/State.txt $TARGETDIR/State.txt
	then 
		echo "\e[40m\e[92m [Finished] \e[0m"
	else
		echo "\e[40m\e[31m [Build failed at copying files out to target] \e[0m"
	fi

echo "\e[40m\e[92m [Build finished] \e[0m"
