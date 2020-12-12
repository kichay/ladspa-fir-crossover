FIR_HEADER ?= example.h
LADSPA_TARGET ?= fir_example.so
CFLAGS=-c -Wall -fPIC -ffast-math -O2 -D PIC

all:
	$(CC) $(CFLAGS) -D FIR_HEADER="$(FIR_HEADER)" -o fir.o fir.c
	$(LD) -shared -o $(LADSPA_TARGET) fir.o
