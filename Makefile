CC=g++
CXXFLAGS=-std=c++14 -Ofast -msse2 -march=native -fopenmp
PROFFLAGS=-pg -std=c++14 -msse2 -march=native -fopenmp

all: ep

clear:
	rm -f *.o

ep: ep.cpp
	$(CC) $(CXXFLAGS) $^ -o $@

prof: ep.cpp
	$(CC) $(PROFFLAGS) $^ -o $@
