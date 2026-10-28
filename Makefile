CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -O2 -Iinclude -MMD -MP
TARGET := build/potencia
SOURCES := $(wildcard src/*.cc)
OBJECTS := $(patsubst src/%.cc,build/%.o,$(SOURCES))
DEPFILES := $(OBJECTS:.o=.d)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS) | build
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@

build:
	mkdir -p $@

build/%.o: src/%.cc | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPFILES)

run: $(TARGET)
	./$(TARGET) -x 2 -y 5

clean:
	rm -rf build
