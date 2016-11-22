CC=g++
CXXFLAGS=-std=c++14 -Ofast -march=native -fopenmp 

all: ep

clear:
	rm -f *.o

ep: ep.cpp
	$(CC) $(CXXFLAGS) $^ -o $@
