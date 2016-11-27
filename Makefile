CC=g++
CXXFLAGS=-pg -std=c++14 -msse2 -march=native -fopenmp

all: ep

clear:
	rm -f *.o

ep: ep.cpp
	$(CC) $(CXXFLAGS) $^ -o $@
