CC=gcc
CFLAGS= -ansi --std=gnu99 -Wall -pedantic -pthread

all: ep_barrier ep_mutex clear

clear:
	rm -f *.o

ep_barrier: ep_barrier.o
	$(CC) $(CFLAGS) $^ -o $@

ep_barrier.o: ep_barrier.c

ep_mutex: ep_mutex.o
	$(CC) $(CFLAGS) $^ -o $@

ep_mutex.o: ep_mutex.c
