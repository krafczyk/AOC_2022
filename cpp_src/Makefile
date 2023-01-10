CXX=g++
#CXX=clang++

CXXFLAGS=-std=c++20 -O3 -Wall -Werror -Wextra -I utilities

all: ./bin/day_3 ./bin/day_4 ./bin/day_7 ./bin/day_8 ./bin/day_11 ./bin/day_14 ./bin/day_15 ./bin/day_16

./obj/utilities.o: ./utilities/utilities.cxx ./utilities/utilities.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

./bin/%: ./src/%.cpp ./obj/utilities.o
	$(CXX) $(CXXFLAGS) $< ./obj/utilities.o -o $@

clean:
	rm -rf ./bin/* ./obj/*
