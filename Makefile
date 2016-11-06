CC=g++-6
CFLAGS= -Wall -pedantic -Ofast -march=native -fopenmp 

all: ep

clear:
	rm -f *.o

ep: ep.cpp
	$(CC) $(CFLAGS) $^ -o $@
