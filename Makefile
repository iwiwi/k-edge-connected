CXX = g++
CXXFLAGS = -g -O3 -Wall -Wextra



all: bin bin/mkecs

bin:
	mkdir -p bin

bin/mkecs: src/mkecs.cc src/mkecs_main.cc
	$(CXX) $(CXXFLAGS) -Isrc -o $@ $^

bin/mkecs_test: src/mkecs.cc src/mkecs_test.cc
	$(CXX) $(CXXFLAGS) -lgtest -lgtest_main -o $@ $^



.PHONY: test clean

test: bin/mkecs_test
	bin/mkecs_test

clean:
	rm -rf bin