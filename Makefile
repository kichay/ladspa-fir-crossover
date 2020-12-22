FIR_HEADER ?= example/3band.h
LADSPA_TARGET ?= crossover.so
CFLAGS=-c -Wall -fPIC -ffast-math -O2 -D PIC

all:
	$(CC) $(CFLAGS) -D FIR_HEADER="$(FIR_HEADER)" -o crossover.o crossover.c
	$(LD) -shared -o $(LADSPA_TARGET) crossover.o
