CROSSOVER_HEADER ?= example/3band.h
LADSPA_TARGET ?= crossover.so
CFLAGS=-c -Wall -fPIC -ffast-math -O2 -D PIC

all:
	$(CC) $(CFLAGS) -D CROSSOVER_HEADER="$(CROSSOVER_HEADER)" -o crossover.o crossover.c
	$(LD) -shared -o $(LADSPA_TARGET) crossover.o
