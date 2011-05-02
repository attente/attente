.PHONY: all clean

all: signal record slice fourier simple radio

clean:
	-rm signal record slice fourier simple radio *.o

signal: signal.c
	gcc -lm -framework CoreAudio -ljack -o signal signal.c

record: record.c
	gcc -framework CoreAudio -ljack -D AUTOSTOP -o record record.c

slice: slice.c
	gcc -o slice slice.c

fourier: fourier.c window.o
	gcc -lm `pkg-config --cflags --libs fftw3` -o fourier window.o fourier.c

simple: simple.c
	gcc -lm `pkg-config --cflags --libs fftw3` -framework CoreAudio -ljack -o simple simple.c

radio: radio.c
	gcc -lm `pkg-config --cflags --libs fftw3` -framework CoreAudio -ljack -o radio radio.c
