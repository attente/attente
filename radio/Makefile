all:
	gcc -lm -framework CoreAudio -ljack -o signal signal.c
	gcc -lm `pkg-config --cflags --libs fftw3` -framework CoreAudio -ljack -o record record.c
	gcc -lm `pkg-config --cflags --libs fftw3` -framework CoreAudio -ljack -o simple simple.c

clean:
	-rm signal record simple
