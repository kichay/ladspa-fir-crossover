PP = ./preprocessor.py
CROSSOVER_YAML ?= example/3band.yaml
CFLAGS=-c -Wall -fPIC -ffast-math -O2 -D PIC

all:
	$(PP) $(CROSSOVER_YAML) < crossover.h.j2 > crossover.h
	$(CC) $(CFLAGS) -o crossover.o crossover.c
	$(LD) -shared -o crossover.so crossover.o
