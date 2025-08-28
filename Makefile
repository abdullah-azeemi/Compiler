# Compiler and flags
CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall

# Sources and binary name
SRC := source.cpp
BIN := lexer

# Detect Windows executable suffix
ifeq ($(OS),Windows_NT)
  EXE := $(BIN).exe
else
  EXE := $(BIN)
endif

.PHONY: all run clean

all: $(EXE)

$(EXE): $(SRC) lexer1.cpp string_queue.hpp
	$(CXX) $(CXXFLAGS) $(SRC) -o $(EXE)

run: $(EXE)
	./$(EXE)

clean:
	$(RM) $(EXE)

