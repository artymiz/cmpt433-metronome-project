#!/bin/sh
echo "\e[40m\e[92m [Making and binning o files from button-prototype] \e[0m"
make -f button-prototype/Makefile

echo "\e[40m\e[92m [Making and binning o files from ticker-prototype] \e[0m"
make -f ticker-prototype/Makefile

echo "\e[40m\e[92m [Linking libraries in bin and sending to target] \e[0m"
make -f bin/Makefile

