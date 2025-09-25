#include <iostream>
#include <conio.h>
#include <string>
#include <locale>

#include "lexer1.cpp"
#include "regex_lexer.cpp"

using namespace std;

int main()
{
  std::setlocale(LC_ALL, "en_US.UTF-8");

  string code = "\nfn void tricky() {\n    // single-line comment\n    string s = \"hello\\nworld\\t\\\"quote\\\"\";\n    comment */\n \n    int x = 5 & 3 | 2 ^ 1;\n    int π = 3;\n    string привет = \"hi\";\n \n    int 2bad = 5;       // invalid\n    float @oops = 1.2;  // invalid\n}";
  // SimpleLexer lexer(code);
  Regex_Lexer RL;
  cout << "INPUT:" << code << "\n";
  // lexer.lex();
  //  cout << "OUTPUT";
  //  lexer.printTokens();
  cout << "WITH REGEX";
  RL.RegexLexer(code);
  return 0;
}
