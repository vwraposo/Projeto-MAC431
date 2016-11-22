CC=g++-6
CXXFLAGS=-std=c++14 -Wall -pedantic -Ofast -march=native -fopenmp 

all: ep

clear:
	rm -f *.o

ep: ep.cpp
	$(CC) $(CXXFLAGS) $^ -o $@
