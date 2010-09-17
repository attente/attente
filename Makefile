all:
	gcc -lm `pkg-config --cflags --libs clutter-1.0` -o clutter clutter.c

clean:
	rm clutter
