CXX = g++
CXXFLAGS = -O3 -Wall -Wextra

all: benchmark

benchmark: sample/benchmark.cpp src/PrunedLabeling.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

.PHONY: clean

clean:
	rm -f benchmark