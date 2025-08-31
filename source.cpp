#include<iostream>
#include<conio.h>
#include<string>

#include "lexer1.cpp"
#include "regex_lexer.cpp"


using namespace std;

int main() {
  string code = "add(int a, float b) { return a + b; }";
  SimpleLexer lexer(code);
  Regex_Lexer RL;
  lexer.lex();
  lexer.printTokens();
  RL.RegexLexer(code);
  return 0;
}
