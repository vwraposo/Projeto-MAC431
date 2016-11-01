CC=gcc
CFLAGS=--std=c99 -Wall -pedantic
LIBS=-lpthread

all: ep clear

clear:
	rm -f *.o

ep: ep.o
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) 

ep.o: ep.c

