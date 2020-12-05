FIR_HEADER ?= example.h
LADSPA_TARGET ?= fir_example.so
CFLAGS=-fPIC -DPIC -Wall -O2 -c

all:
	$(CC) $(CFLAGS) -D FIR_HEADER="$(FIR_HEADER)" -o fir.o fir.c
	$(LD) -shared -o $(LADSPA_TARGET) fir.o
