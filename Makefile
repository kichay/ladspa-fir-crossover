CYAML ?= example/3band.yaml
PP = ./j2render.py
CFLAGS = -c -Wall -fPIC -ffast-math -O2 -D PIC

all:
	$(PP) $(CYAML) < crossover.c.j2 > crossover.c
	$(CC) $(CFLAGS) -o crossover.o crossover.c
	$(LD) -shared -o crossover.so crossover.o
