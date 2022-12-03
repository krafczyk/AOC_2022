CXX=g++
#CXX=clang++

CXXFLAGS=-std=c++17 -fconcepts -O3 -Wall -Werror -Wextra -I utilities

all: ./bin/day_3

./bin/% : ./src/%.cpp 
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf ./bin/*
