#include<iostream>
#include<conio.h>
#include<string>

#include "lexer1.cpp"

using namespace std;

int main() {
  string code = "fn add(int a, float b) { return a + b; }";
  SimpleLexer lexer(code);
  lexer.lex();
  lexer.printTokens();
  return 0;
}
