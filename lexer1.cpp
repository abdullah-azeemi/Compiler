#include<iostream>
#include<conio.h>
#include<string>
#include<cctype>

#include "string_queue.hpp"

using namespace std;

enum TokenKind {
  T_FUNCTION, T_RETURN, T_IF, T_ELSE, T_WHILE, T_FOR,
  T_INT, T_FLOAT, T_STRING, T_BOOL,
  T_IDENTIFIER,
  T_INTLIT, T_FLOATLIT, T_STRINGLIT,
  T_PARENL, T_PARENR, T_BRACEL, T_BRACER,
  T_SEMICOLON, T_COMMA, T_DOT, T_QUOTES,
  T_ASSIGNOP, T_EQUALSOP, T_GREATER, T_LESSER, T_GREATEQ, T_LESSEQ, T_NOTEQUAL,
  T_AND, T_OR, T_NOT, T_PLUS, T_MINUS, T_MUL, T_DIV, T_MOD,
  T_UNKNOWN
};

// Using Token from string_queue.hpp; avoid redefining here

class SimpleLexer{

  string input;
  int pos;

  stringQueue tq;

public:
  SimpleLexer(string input) : input(input), pos(0){

  }

  TokenKind getIdentifierKind(string str){
    if (str == "fn") return T_FUNCTION;
    else if (str == "return") return T_RETURN;
    else if (str == "if") return T_IF;
    else if (str == "else") return T_ELSE;
    else if (str == "while") return T_WHILE;
    else if (str == "for") return T_FOR;
    else if (str == "int") return T_INT;
    else if (str == "float") return T_FLOAT;
    else if (str == "string") return T_STRING;
    else if (str == "bool") return T_BOOL;
    else if (str == "(") return T_PARENL;
    else if (str == ")") return T_PARENR;
    else if (str == "{") return T_BRACEL;
    else if (str == "}") return T_BRACER;

    else if (str == "=") return T_ASSIGNOP;
    else if (str == "==") return T_EQUALSOP;
    else if (str == ">") return T_GREATER;
    else if (str == "<") return T_LESSER;
    else if (str == ">=") return T_GREATEQ;
    else if (str == "<=") return T_LESSEQ;
    else if (str == "!=") return T_NOTEQUAL;
    else if (str == "+") return T_PLUS;
    else if (str == "-") return T_MINUS;
    else if (str == "*") return T_MUL;
    else if (str == "/") return T_DIV;
    else if (str == "%") return T_MOD;

    else if (str == ";") return T_SEMICOLON;
    else if (str == ",") return T_COMMA;
    else if (str == ".") return T_DOT;
    else if (str == "&&") return T_AND;
    else if (str == "||") return T_OR;
    else if (str == "!") return T_NOT;
    else if (str == "\"") return T_QUOTES;

    else return T_UNKNOWN;
  }

  const char * kindToCString(TokenKind k){
    switch(k){
      case T_FUNCTION: return "FUNCTION";
      case T_RETURN: return "RETURN";
      case T_IF: return "IF";
      case T_ELSE: return "ELSE";
      case T_WHILE: return "WHILE";
      case T_FOR: return "FOR";
      case T_IDENTIFIER: return "IDENTIFIER";
      case T_INT: return "INT";
      case T_FLOAT: return "FLOAT";
      case T_STRING: return "STRING";
      case T_BOOL: return "BOOL";
      case T_INTLIT: return "INTLIT";
      case T_FLOATLIT: return "FLOATLIT";
      case T_STRINGLIT: return "STRINGLIT";
      case T_PARENL: return "PARENL";
      case T_PARENR: return "PARENR";
      case T_BRACEL: return "BRACEL";
      case T_BRACER: return "BRACER";
      case T_QUOTES: return "QUOTES";
      case T_ASSIGNOP: return "ASSIGNOP";
      case T_EQUALSOP: return "EQUALSOP";
      case T_GREATER: return "GREATER";
      case T_LESSER: return "LESSER";
      case T_GREATEQ: return "GREATEQ";
      case T_LESSEQ: return "LESSEQ";
      case T_NOTEQUAL: return "NOTEQUAL";
      case T_SEMICOLON: return "SEMICOLON";
      case T_COMMA: return "COMMA";
      case T_DOT: return "DOT";
      case T_AND: return "AND";
      case T_OR: return "OR";
      case T_NOT: return "NOT";
      case T_PLUS: return "PLUS";
      case T_MINUS: return "MINUS";
      case T_MUL: return "MUL";
      case T_DIV: return "DIV";
      case T_MOD: return "MOD";
      case T_UNKNOWN: default: return "UNKNOWN";
    }
  }

  void lex(){
    while (pos < (int)input.size()){
      char c = input[pos];

      // Whitespace
      if (isspace(static_cast<unsigned char>(c))){ pos += 1; continue; }

      // Comments
      if (c == '/'){
        if (pos + 1 < (int)input.size() && input[pos+1] == '/'){
          pos += 2;
          while (pos < (int)input.size() && input[pos] != '\n') pos += 1;
          continue;
        } else if (pos + 1 < (int)input.size() && input[pos+1] == '*'){
          pos += 2;
          while (pos + 1 < (int)input.size() && !(input[pos] == '*' && input[pos+1] == '/')) pos += 1;
          if (pos + 1 < (int)input.size()) pos += 2; else tq.push("UNKNOWN", "Unterminated comment");
          continue;
        }
      }

      // Strings with escapes
      if (c == '"'){
        string quote(1, '"');
        tq.push(kindToCString(T_QUOTES), quote.c_str());
        pos += 1;
        string lit = "";
        bool terminated = false;
        while (pos < (int)input.size()){
          char ch = input[pos++];
          if (ch == '\\'){
            if (pos >= (int)input.size()) break;
            char esc = input[pos++];
            switch(esc){
              case 'n': lit.push_back('\n'); break;
              case 't': lit.push_back('\t'); break;
              case 'r': lit.push_back('\r'); break;
              case '\\': lit.push_back('\\'); break;
              case '"': lit.push_back('"'); break;
              default: lit.push_back(esc); break;
            }
          } else if (ch == '"'){
            terminated = true;
            break;
          } else {
            lit.push_back(ch);
          }
        }
        tq.push(kindToCString(T_STRINGLIT), lit.c_str());
        tq.push(kindToCString(T_QUOTES), quote.c_str());
        if (!terminated) tq.push("UNKNOWN", "Unterminated string");
        continue;
      }

      // Numbers (int/float)
      if (isdigit(static_cast<unsigned char>(c))){
        int start = pos;
        bool isFloat = false;
        while (pos < (int)input.size() && isdigit(static_cast<unsigned char>(input[pos]))) pos += 1;
        if (pos < (int)input.size() && input[pos] == '.'){
          isFloat = true;
          pos += 1;
          while (pos < (int)input.size() && isdigit(static_cast<unsigned char>(input[pos]))) pos += 1;
        }
        if (pos < (int)input.size() && (isalpha(static_cast<unsigned char>(input[pos])) || input[pos] == '_')){
          tq.push("UNKNOWN", "Invalid identifier starts with digit");
          while (pos < (int)input.size() && (isalnum(static_cast<unsigned char>(input[pos])) || input[pos] == '_')) pos += 1;
          continue;
        }
        string num = input.substr(start, pos - start);
        if (isFloat) tq.push(kindToCString(T_FLOATLIT), num.c_str());
        else tq.push(kindToCString(T_INTLIT), num.c_str());
        continue;
      }

      // Identifier or keyword
      if (isalpha(static_cast<unsigned char>(c)) || c == '_'){
        int start = pos;
        pos += 1;
        while (pos < (int)input.size() && (isalnum(static_cast<unsigned char>(input[pos])) || input[pos] == '_')) pos += 1;
        string word = input.substr(start, pos - start);
        TokenKind k = getIdentifierKind(word);
        if (k != T_UNKNOWN && k != T_IDENTIFIER){
          tq.push(kindToCString(k), word.c_str());
        } else {
          tq.push(kindToCString(T_IDENTIFIER), word.c_str());
        }
        continue;
      }

      // Two-character operators
      if (pos + 1 < (int)input.size()){
        string two = input.substr(pos, 2);
        TokenKind k2 = getIdentifierKind(two);
        if (k2 != T_UNKNOWN){
          tq.push(kindToCString(k2), two.c_str());
          pos += 2;
          continue;
        }
      }

      // Single-character operators and delimiters
      {
        string one(1, c);
        TokenKind k1 = getIdentifierKind(one);
        if (k1 != T_UNKNOWN){
          tq.push(kindToCString(k1), one.c_str());
          pos += 1;
          continue;
        }
      }

      // Unknown
      {
        string unk(1, c);
        tq.push(kindToCString(T_UNKNOWN), unk.c_str());
        pos += 1;
      }
    }
  }

  void printTokens(){
    // Custom print format: [T_KIND, T_KIND, T_IDENTIFIER("val"), T_INTLIT(123), ...]
    // We'll reconstruct from the queue by popping all tokens and printing in order
    // Note: stringQueue doesn't support iteration without popping; make a copy
    stringQueue copy;
    while (!tq.empty()){
      Token t = tq.pop();
      copy.push(t.type, t.value);
    }
    cout << "[";
    bool first = true;
    while (!copy.empty()){
      Token t = copy.pop();
      // restore into original queue to preserve state
      tq.push(t.type, t.value);

      if (!first) cout << ", ";
      first = false;

      string kind = string(t.type);
      if (kind == "IDENTIFIER"){
        cout << "T_IDENTIFIER(\"" << t.value << "\")";
      } else if (kind == "INTLIT"){
        cout << "T_INTLIT(" << t.value << ")";
      } else if (kind == "FLOATLIT"){
        cout << "T_FLOATLIT(" << t.value << ")";
      } else if (kind == "STRINGLIT"){
        cout << "T_STRINGLIT(\"" << t.value << "\")";
      } else {
        cout << "T_" << kind;
      }
    }
    cout << "]" << endl;
  }

  


};