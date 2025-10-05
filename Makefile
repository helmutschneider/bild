CC=gcc
CFLAGS=-std=c17 -Wall -Werror -Wpedantic -O0 -g -I/opt/homebrew/include
LDFLAGS=-L/opt/homebrew/lib -lSDL3 -lm

all:
	mkdir -p bin/ && $(CC) $(CFLAGS) pic.c bild.c $(LDFLAGS) -o bin/bild

test:
	mkdir -p bin/ && $(CC) $(CFLAGS) pic.c test.c $(LDFLAGS) -o bin/test && ./bin/test

clean:
	rm -rf bin/
