CXX ?= g++
CXXFLAGS ?= -O3 -DNDEBUG -std=c++17 -Wall -Wextra -Wpedantic
CPPFLAGS = -Iinclude
COMMON = src/assignment.cpp src/tracking.cpp
TARGETS = build/lap_main build/lap_benchmark build/lap_tracking_experiment build/test_assignment build/test_tracking
.PHONY: all test clean
all: $(TARGETS)
build:
	mkdir -p build
build/lap_%: src/%.cpp $(COMMON) include/assignment.hpp include/tracking.hpp | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(COMMON) $< -o $@
build/test_%: tests/test_%.cpp $(COMMON) include/assignment.hpp include/tracking.hpp | build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(COMMON) $< -o $@
test: all
	./build/test_assignment
	./build/test_tracking
clean:
	rm -rf build
