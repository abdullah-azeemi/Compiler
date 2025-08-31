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
  cout<<"INPUT:"<<code<<"\n";
  lexer.lex();
  cout<<"OUTPUT";
  lexer.printTokens();
  // cout<<"WITH REGEX";
  // RL.RegexLexer(code);
  return 0;
}
