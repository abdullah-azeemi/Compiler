# Simple Lexer Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Files
SRC = source.cpp
EXE = lexer_test

# Build the lexer
all: $(EXE)

$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(EXE)

# Run the lexer
run: $(EXE)
	./$(EXE)

# Clean up
clean:
	rm -f $(EXE)

# Show help
help:
	@echo "make      - Build the lexer"
	@echo "make run  - Build and run"
	@echo "make clean - Remove executable"

