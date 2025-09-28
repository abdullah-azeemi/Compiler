# Simple Lexer

A basic lexical analyzer that converts code into tokens.

## What it does
- Takes code like: `fn add(int a) { return a + 1; }`
- Splits it into tokens: `fn`, `add`, `(`, `int`, `a`, `)`, `{`, `return`, `a`, `+`, `1`, `;`, `}`
- Identifies what each token is: function, identifier, parenthesis, etc.

## How to use
1. Compile: `g++ -o lexer source.cpp`
2. Run: `./lexer`

## Files
- `lexer1.cpp` - The main lexer code
- `string_queue.hpp` - Stores the tokens
- `source.cpp` - Test program
- `Makefile` - Easy build commands

## What it recognizes
- Keywords: fn, return, if, while, int, float
- Operators: +, -, *, /, =, ==, >, <
- Symbols: (, ), {, }, ;, ,
- Numbers: 42, 3.14
- Names: variable names

![alt text](Results/images/image.png)
![alt text](Results/images/image-1.png)

## PARSER RESULT
![alt text](<output 1.2.jpeg>) ![alt text](<output 2.0.jpeg>) ![alt text](output1.2.jpeg) ![alt text](<input 1.jpeg>) ![alt text](<input 2.jpeg>) ![alt text](<out 1.0.jpeg>)