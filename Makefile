CC=g++-6
CXXFLAGS=-Ofast -fopenmp -std=c++14 -msse2 -march=native
PROFFLAGS=-pg -fopenmp -std=c++14 -msse2 -march=native

all: ep

clear:
	rm -f *.o

ep: ep.cpp ep.hpp
	$(CC) $(CXXFLAGS) -c ep.cpp

prof: ep.cpp ep.hpp
	$(CC) $(PROFFLAGS) -c ep.cpp
