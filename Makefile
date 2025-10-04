CC=gcc
CFLAGS=-std=c17 -Wall -Werror -Wpedantic -O0 -g -I/opt/homebrew/include
LDFLAGS=-L/opt/homebrew/lib -lsdl3

all:
	mkdir -p bin/ && $(CC) $(CFLAGS) bild.c $(LDFLAGS) -o bin/bild

clean:
	rm -rf bin/
