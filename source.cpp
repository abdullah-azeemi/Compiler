#include <iostream>
#include <string>
#include <sstream>
#include "regex_lexer.hpp"
#include "parser.hpp"
#include "Utilities/ast_printer.hpp"

using namespace std;

string tokenTypeToName(TokenType type)
{
  switch (type)
  {
  case T_FUNCTION_RL:
  case T_FN_RL:
    return "Function";
  case T_RETURN_RL:
  case T_WAPSI_RL:
    return "Return";
  case T_IF_RL:
  case T_AGAR_RL:
    return "If";
  case T_ELSE_RL:
  case T_WARNA_RL:
    return "Else";
  case T_WHILE_RL:
  case T_JAB_RL:
    return "While";
  case T_FOR_RL:
  case T_DUHRAO_RL:
    return "For";
  case T_BREAK_RL:
  case T_TORO_RL:
    return "Break";
  case T_CONTINUE_RL:
  case T_RAKHO_RL:
    return "Continue";
  case T_INT_RL:
  case T_GINTI_RL:
    return "Int";
  case T_FLOAT_RL:
    return "Float";
  case T_STRING_RL:
    return "String";
  case T_BOOL_RL:
    return "Bool";
  case T_IDENTIFIER_RL:
    return "Identifier";
  case T_INT_RLLIT:
    return "IntLit";
  case T_FLOAT_RLLIT:
    return "FloatLit";
  case T_STRING_RLLIT:
    return "StringLit";
  case T_BOOL_RLLIT:
    return "BoolLit";
  case T_ASSIGNOP_RL:
    return "AssignOp";
  case T_EQUALSOP_RL:
    return "EqualsOp";
  case T_NOT_EQUALS_RL:
    return "NotEquals";
  case T_LESS_THAN_RL:
    return "LessThan";
  case T_GREATER_THAN_RL:
    return "GreaterThan";
  case T_LESS_EQUAL_RL:
    return "LessEqual";
  case T_GREATER_EQUAL_RL:
    return "GreaterEqual";
  case T_PLUS_RL:
    return "AddOp";
  case T_MINUS_RL:
    return "SubOp";
  case T_MUL_RL:
    return "MulOp";
  case T_DIV_RL:
    return "DivOp";
  case T_MOD_RL:
    return "ModOp";
  case T_AND_LOGICAL_RL:
    return "AndOp";
  case T_OR_LOGICAL_RL:
    return "OrOp";
  case T_NOT_RL:
    return "NotOp";
  case T_AND_BIT_RL:
    return "BitAndOp";
  case T_OR_BIT_RL:
    return "BitOrOp";
  case T_XOR_BIT_RL:
    return "BitXorOp";
  case T_PARENL_RL:
    return "ParenL";
  case T_PARENR_RL:
    return "ParenR";
  case T_BRACEL_RL:
    return "BraceL";
  case T_BRACER_RL:
    return "BraceR";
  case T_BRACKETL_RL:
    return "BracketL";
  case T_BRACKETR_RL:
    return "BracketR";
  case T_SEMICOLON_RL:
    return "Semicolon";
  case T_COMMA_RL:
    return "Comma";
  case T_DOT_RL:
    return "Dot";
  default:
    return "Unknown";
  }
}

string formatToken(const Token &token)
{
  string typeName = tokenTypeToName(token.type);

  if (token.type == T_IDENTIFIER_RL)
  {
    return typeName + "(\"" + token.value + "\")";
  }
  else if (token.type == T_INT_RLLIT)
  {
    return typeName + "(" + token.value + ")";
  }
  else if (token.type == T_FLOAT_RLLIT)
  {
    return typeName + "(" + token.value + ")";
  }
  else if (token.type == T_STRING_RLLIT)
  {
    return typeName + "(\"" + token.value + "\")";
  }
  else if (token.type == T_BOOL_RLLIT)
  {
    return typeName + "(" + token.value + ")";
  }

  return typeName;
}

void printTokenStream(const vector<Token> &tokens)
{
  cout << "[";
  bool first = true;
  for (const auto &token : tokens)
  {
    if (token.type == T_EOF_RL)
      continue;

    if (!first)
    {
      cout << ", ";
    }
    cout << formatToken(token);
    first = false;
  }
  cout << "]" << endl;
}

int main()
{
  // Example no 1
  string example1 = R"(
        int a = b +  c * 245345 - b / -6245 + g .

        parse_var_decl
        


        )";

  // Example
  string example2 = R"(
        fn ginti some_fn() {
          duhrao (ginti a = 0 . a < 10 . a = a + 1) {
            agar (a == 5) { toro } warna {}
          }

          wapsi 5 .
        } .
        )";

  cout << "# Example 01 - Expressions\n"
       << endl;
  cout << "# Source code\n"
       << endl;
  cout << "```" << example1 << "```\n"
       << endl;

  try
  { // 1st example
    // Tokenize the input
    Lexer lexer;
    auto tokens = lexer.tokenize(example1);
    cout << "## Token Stream\n"
         << endl;
    cout << "```" << endl;
    printTokenStream(tokens);
    cout << "```\n"
         << endl;

    // parsing
    Parser parser(tokens);
    auto ast = parser.parse();
    cout << "# Abstract Syntax Tree\n"
         << endl;
    cout << "```" << endl;
    cout << ASTPrinter::printAST(ast) << endl;
    cout << "```\n"
         << endl;
  }
  catch (const ParseException &e)
  {
    std::cerr << "Parse error in Example 1: " << e.what() << endl;
  }

  cout << "# Example 02 - Nested Blocks\n"
       << endl;
  cout << "## Source code\n"
       << endl;
  cout << "```" << example2 << "```\n"
       << endl;

  try
  { // 2nd example
    // Tokenize
    Lexer lexer;
    auto tokens = lexer.tokenize(example2);
    cout << "## Token Stream\n"
         << endl;
    cout << "```" << endl;
    printTokenStream(tokens);
    cout << "```\n"
         << endl;

    // parsing
    Parser parser(tokens);
    auto ast = parser.parse();
    cout << "## Abstract Syntax Tree\n"
         << endl;
    cout << "```" << endl;
    cout << ASTPrinter::printAST(ast) << endl;
    cout << "```\n"
         << endl;
  }
  catch (const ParseException &e)
  {
    std::cerr << "Parse error in Example 2: " << e.what() << endl;
  }
  return 0;
}
