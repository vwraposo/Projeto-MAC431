CC=gcc
CFLAGS= -ansi --std=gnu99 -Wall -pedantic -pthread

all: ep clear

clear:
	rm -f *.o

ep: ep_barrier.o
	$(CC) $(CFLAGS) $^ -o $@

ep_barrier.o: ep_barrier.c

