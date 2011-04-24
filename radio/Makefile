all:
	gcc -lm `pkg-config --cflags --libs fftw3` -framework CoreAudio -ljack -o simple simple.c

clean:
	-rm simple
