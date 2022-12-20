CXX=g++
#CXX=clang++

CXXFLAGS=-std=c++20 -O3 -Wall -Werror -Wextra -I utilities

all: ./bin/day_3 ./bin/day_4 ./bin/day_7 ./bin/day_8 ./bin/day_11

./bin/% : ./src/%.cpp 
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -rf ./bin/*
