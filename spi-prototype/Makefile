CROSS_COMPILE = arm-linux-gnueabihf-
CC_C = $(CROSS_COMPILE)gcc
CFLAGS = -Wall -g -Werror -Wshadow

OUTDIR = $(HOME)/cmpt433/public/

all:
	$(CC_C) $(CFLAGS) spi.c -o $(OUTDIR)/spi
	$(CC_C) $(CFLAGS) stabie-soft.c -o $(OUTDIR)/stabie-soft
	$(CC_C) $(CFLAGS) spi-test.c -o $(OUTDIR)/spi-test
