// #include <iostream>
// #include <string>
// #include "string_queue.hpp"

// using namespace std;

// enum TokenKind
// {
//   T_FUNCTION,
//   T_RETURN,
//   T_IF,
//   T_ELSE,
//   T_WHILE,
//   T_FOR,
//   T_INT,
//   T_FLOAT,
//   T_STRING,
//   T_BOOL,
//   T_IDENTIFIER,
//   T_INTLIT,
//   T_FLOATLIT,
//   T_STRINGLIT,
//   T_PARENL,
//   T_PARENR,
//   T_BRACEL,
//   T_BRACER,
//   T_SEMICOLON,
//   T_COMMA,
//   T_DOT,
//   T_QUOTES,
//   T_ASSIGNOP,
//   T_EQUALSOP,
//   T_GREATER,
//   T_LESSER,
//   T_GREATEQ,
//   T_LESSEQ,
//   T_NOTEQUAL,
//   T_AND,
//   T_OR,
//   T_NOT,
//   T_PLUS,
//   T_MINUS,
//   T_MUL,
//   T_DIV,
//   T_MOD,
//   T_UNKNOWN
// };

// class SimpleLexer
// {
//   string input;
//   int pos;
//   stringQueue tq;

// public:
//   SimpleLexer(string input) : input(input), pos(0) {}

//   bool isSpace(char c)
//   {
//     return c == ' ' || c == '\t' || c == '\n' || c == '\r';
//   }

//   bool isDigit(char c)
//   {
//     return c >= '0' && c <= '9';
//   }

//   bool isAlpha(char c)
//   {
//     return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
//   }

//   bool isAlphaNum(char c)
//   {
//     return isAlpha(c) || isDigit(c);
//   }

//   TokenKind getIdentifierKind(string str)
//   {
//     if (str == "fn")
//       return T_FUNCTION;
//     else if (str == "return")
//       return T_RETURN;
//     else if (str == "if")
//       return T_IF;
//     else if (str == "else")
//       return T_ELSE;
//     else if (str == "while")
//       return T_WHILE;
//     else if (str == "for")
//       return T_FOR;
//     else if (str == "int")
//       return T_INT;
//     else if (str == "float")
//       return T_FLOAT;
//     else if (str == "string")
//       return T_STRING;
//     else if (str == "bool")
//       return T_BOOL;
//     else if (str == "(")
//       return T_PARENL;
//     else if (str == ")")
//       return T_PARENR;
//     else if (str == "{")
//       return T_BRACEL;
//     else if (str == "}")
//       return T_BRACER;

//     else if (str == "=")
//       return T_ASSIGNOP;
//     else if (str == "==")
//       return T_EQUALSOP;
//     else if (str == ">")
//       return T_GREATER;
//     else if (str == "<")
//       return T_LESSER;
//     else if (str == ">=")
//       return T_GREATEQ;
//     else if (str == "<=")
//       return T_LESSEQ;
//     else if (str == "!=")
//       return T_NOTEQUAL;
//     else if (str == "+")
//       return T_PLUS;
//     else if (str == "-")
//       return T_MINUS;
//     else if (str == "*")
//       return T_MUL;
//     else if (str == "/")
//       return T_DIV;
//     else if (str == "%")
//       return T_MOD;

//     else if (str == ";")
//       return T_SEMICOLON;
//     else if (str == ",")
//       return T_COMMA;
//     else if (str == ".")
//       return T_DOT;
//     else if (str == "&&")
//       return T_AND;
//     else if (str == "||")
//       return T_OR;
//     else if (str == "!")
//       return T_NOT;
//     else if (str == "\"")
//       return T_QUOTES;

//     else
//       return T_UNKNOWN;
//   }

//   const char *kindToCString(TokenKind k)
//   {
//     switch (k)
//     {
//     case T_FUNCTION:
//       return "FUNCTION";
//     case T_RETURN:
//       return "RETURN";
//     case T_IF:
//       return "IF";
//     case T_ELSE:
//       return "ELSE";
//     case T_WHILE:
//       return "WHILE";
//     case T_FOR:
//       return "FOR";
//     case T_IDENTIFIER:
//       return "IDENTIFIER";
//     case T_INT:
//       return "INT";
//     case T_FLOAT:
//       return "FLOAT";
//     case T_STRING:
//       return "STRING";
//     case T_BOOL:
//       return "BOOL";
//     case T_INTLIT:
//       return "INTLIT";
//     case T_FLOATLIT:
//       return "FLOATLIT";
//     case T_STRINGLIT:
//       return "STRINGLIT";
//     case T_PARENL:
//       return "PARENL";
//     case T_PARENR:
//       return "PARENR";
//     case T_BRACEL:
//       return "BRACEL";
//     case T_BRACER:
//       return "BRACER";
//     case T_QUOTES:
//       return "QUOTES";
//     case T_ASSIGNOP:
//       return "ASSIGNOP";
//     case T_EQUALSOP:
//       return "EQUALSOP";
//     case T_GREATER:
//       return "GREATER";
//     case T_LESSER:
//       return "LESSER";
//     case T_GREATEQ:
//       return "GREATEQ";
//     case T_LESSEQ:
//       return "LESSEQ";
//     case T_NOTEQUAL:
//       return "NOTEQUAL";
//     case T_SEMICOLON:
//       return "SEMICOLON";
//     case T_COMMA:
//       return "COMMA";
//     case T_DOT:
//       return "DOT";
//     case T_AND:
//       return "AND";
//     case T_OR:
//       return "OR";
//     case T_NOT:
//       return "NOT";
//     case T_PLUS:
//       return "PLUS";
//     case T_MINUS:
//       return "MINUS";
//     case T_MUL:
//       return "MUL";
//     case T_DIV:
//       return "DIV";
//     case T_MOD:
//       return "MOD";
//     case T_UNKNOWN:
//     default:
//       return "UNKNOWN";
//     }
//   }

//   bool isNumber(string str)
//   {
//     for (int i = 0; i < str.length(); i++)
//     {
//       if (i == 0 && str[i] == '-')
//         continue;
//       if (str[i] == '.' && i > 0)
//         continue;
//       if (!isDigit(str[i]))
//         return false;
//     }
//     return true;
//   }

//   bool isFloat(string str)
//   {
//     return str.find('.') != string::npos && isNumber(str);
//   }

//   void lex()
//   {
//     string word = "";

//     for (int i = 0; i < input.length(); i++)
//     {
//       char c = input[i];

//       if (isSpace(c))
//       {
//         if (word != "")
//         {
//           processWord(word);
//           word = "";
//         }
//         continue;
//       }

//       string singleChar(1, c);
//       if (getIdentifierKind(singleChar) != T_UNKNOWN)
//       {
//         if (word != "")
//         {
//           processWord(word);
//           word = "";
//         }
//         processWord(singleChar);
//         continue;
//       }

//       word += c;
//     }

//     if (word != "")
//     {
//       processWord(word);
//     }
//   }

//   void processWord(string word)
//   {
//     TokenKind kind = getIdentifierKind(word);

//     if (kind != T_UNKNOWN)
//     {
//       tq.push(kindToCString(kind), word.c_str());
//       return;
//     }

//     if (isNumber(word))
//     {
//       if (isFloat(word))
//       {
//         tq.push("FLOATLIT", word.c_str());
//       }
//       else
//       {
//         tq.push("INTLIT", word.c_str());
//       }
//       return;
//     }

//     if (word.length() >= 2 && word[0] == '"' && word[word.length() - 1] == '"')
//     {
//       tq.push("STRINGLIT", word.c_str());
//       return;
//     }

//     tq.push("IDENTIFIER", word.c_str());
//   }

//   void printTokens()
//   {
//     stringQueue copy;
//     while (!tq.empty())
//     {
//       Token t = tq.pop();
//       copy.push(t.type, t.value);
//     }
//     cout << "[";
//     bool first = true;
//     while (!copy.empty())
//     {
//       Token t = copy.pop();
//       tq.push(t.type, t.value);

//       if (!first)
//         cout << ", ";
//       first = false;

//       string kind = string(t.type);
//       if (kind == "IDENTIFIER")
//       {
//         cout << "T_IDENTIFIER(\"" << t.value << "\")";
//       }
//       else if (kind == "INTLIT")
//       {
//         cout << "T_INTLIT(" << t.value << ")";
//       }
//       else if (kind == "FLOATLIT")
//       {
//         cout << "T_FLOATLIT(" << t.value << ")";
//       }
//       else if (kind == "STRINGLIT")
//       {
//         cout << "T_STRINGLIT(\"" << t.value << "\")";
//       }
//       else
//       {
//         cout << "T_" << kind;
//       }
//     }
//     cout << "]" << endl;
//   }
// };
