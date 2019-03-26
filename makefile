CC = gcc
CFLAGS = -Wall

all: mb mc md

mb:
	$(CC) $(CFLAGS) mandelbrot-ms11.c -o mb

mc:
	$(CC) $(CFLAGS) mandelCalc-ms11.c -o mc

md:
	$(CC) $(CFLAGS) mandelDisplay-ms11.c -o md

clean:
	rm -f *mb *mc *md

run:
	./mb
