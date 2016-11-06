CC=g++-6
CFLAGS= -ansi --std=gnu99 -Wall -pedantic -pthread

all: ep

clear:
	rm -f *.o

ep: ep.cpp
	$(CC) -fopenmp $^
	mv a.out ep

