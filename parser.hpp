#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <vector>
#include <string>
#include "Utilities/token_types.hpp"
#include "ast.hpp"

using namespace std;

// My parsing approach: Look ahead and decide what block to build!
// Like looking at LEGO instructions - see what piece comes next

class Parser
{
private:
  vector<Token> tokens;
  int pos;

  // Helper to check if we're done
  bool isEnd()
  {
    return pos >= tokens.size() || tokens[pos].type == T_EOF_RL;
  }

  // Look at current token without moving
  Token currentToken()
  {
    if (pos < tokens.size())
      return tokens[pos];
    return tokens.back();
  }

  // Look ahead at next token
  Token peekNext()
  {
    if (pos + 1 < tokens.size())
      return tokens[pos + 1];
    return tokens.back();
  }

  // Move to next token
  void nextToken()
  {
    if (!isEnd())
      pos++;
  }

  // Check if current token is what we expect
  bool isToken(TokenType type)
  {
    return currentToken().type == type;
  }

  // Eat a token if it matches
  bool eatToken(TokenType type)
  {
    if (isToken(type))
    {
      nextToken();
      return true;
    }
    return false;
  }

  // Get token and move forward
  Token getToken()
  {
    Token t = currentToken();
    nextToken();
    return t;
  }

  // Check if token is a type keyword
  bool isTypeKeyword()
  {
    TokenType t = currentToken().type;
    return t == T_INT_RL || t == T_FLOAT_RL ||
           t == T_STRING_RL || t == T_BOOL_RL || t == T_GINTI_RL;
  }

  // Error handling - simple!
  void error(string msg)
  {
    cerr << "Parse Error at line " << currentToken().line
         << ": " << msg << endl;
    cerr << "Got token: " << currentToken().value << endl;
    exit(1);
  }

  // ===== EXPRESSION PARSING =====
  // Build expressions from bottom up, like stacking blocks

  Expr *parsePrimary()
  {
    Token tok = currentToken();

    // Integer literal
    if (tok.type == T_INT_RLLIT)
    {
      nextToken();
      return new IntLiteral(stoi(tok.value));
    }

    // Float literal
    if (tok.type == T_FLOAT_RLLIT)
    {
      nextToken();
      return new FloatLiteral(stod(tok.value));
    }

    // String literal
    if (tok.type == T_STRING_RLLIT)
    {
      nextToken();
      return new StringLiteral(tok.value);
    }

    // Boolean literal
    if (tok.type == T_BOOL_RLLIT)
    {
      nextToken();
      return new BoolLiteral(tok.value == "true");
    }

    // Identifier or function call
    if (tok.type == T_IDENTIFIER_RL)
    {
      string name = tok.value;
      nextToken();

      // Function call?
      if (isToken(T_PARENL_RL))
      {
        nextToken(); // eat (
        FunctionCall *call = new FunctionCall(name);

        // Parse arguments
        while (!isToken(T_PARENR_RL) && !isEnd())
        {
          call->args.push_back(parseExpression());
          if (!eatToken(T_COMMA_RL) && !isToken(T_PARENR_RL))
          {
            error("Expected ',' or ')' in function call");
          }
        }

        if (!eatToken(T_PARENR_RL))
        {
          error("Expected ')' after function arguments");
        }
        return call;
      }

      return new Identifier(name);
    }

    // Parenthesized expression
    if (tok.type == T_PARENL_RL)
    {
      nextToken();
      Expr *expr = parseExpression();
      if (!eatToken(T_PARENR_RL))
      {
        error("Expected ')' after expression");
      }
      return expr;
    }

    error("Expected expression");
    return nullptr;
  }

  Expr *parseUnary()
  {
    // Handle unary operators (-, !)
    if (isToken(T_MINUS_RL) || isToken(T_NOT_RL))
    {
      Token op = getToken();
      return new UnaryOp(op.type, parseUnary());
    }
    return parsePrimary();
  }

  // Binary operations - using precedence climbing!
  // Think of it like building layers of blocks

  Expr *parseMultiply()
  {
    Expr *left = parseUnary();

    while (isToken(T_MUL_RL) || isToken(T_DIV_RL) || isToken(T_MOD_RL))
    {
      Token op = getToken();
      Expr *right = parseUnary();
      left = new BinaryOp(op.type, left, right);
    }

    return left;
  }

  Expr *parseAdd()
  {
    Expr *left = parseMultiply();

    while (isToken(T_PLUS_RL) || isToken(T_MINUS_RL))
    {
      Token op = getToken();
      Expr *right = parseMultiply();
      left = new BinaryOp(op.type, left, right);
    }

    return left;
  }

  Expr *parseCompare()
  {
    Expr *left = parseAdd();

    while (isToken(T_LESS_THAN_RL) || isToken(T_GREATER_THAN_RL) ||
           isToken(T_LESS_EQUAL_RL) || isToken(T_GREATER_EQUAL_RL))
    {
      Token op = getToken();
      Expr *right = parseAdd();
      left = new BinaryOp(op.type, left, right);
    }

    return left;
  }

  Expr *parseEquality()
  {
    Expr *left = parseCompare();

    while (isToken(T_EQUALSOP_RL) || isToken(T_NOT_EQUALS_RL))
    {
      Token op = getToken();
      Expr *right = parseCompare();
      left = new BinaryOp(op.type, left, right);
    }

    return left;
  }

  Expr *parseLogicalAnd()
  {
    Expr *left = parseEquality();

    while (isToken(T_AND_LOGICAL_RL))
    {
      Token op = getToken();
      Expr *right = parseEquality();
      left = new BinaryOp(op.type, left, right);
    }

    return left;
  }

  Expr *parseLogicalOr()
  {
    Expr *left = parseLogicalAnd();

    while (isToken(T_OR_LOGICAL_RL))
    {
      Token op = getToken();
      Expr *right = parseLogicalAnd();
      left = new BinaryOp(op.type, left, right);
    }

    return left;
  }

  Expr *parseAssign()
  {
    Expr *left = parseLogicalOr();

    if (isToken(T_ASSIGNOP_RL))
    {
      // Check if left is an identifier
      if (left->nodeType != NODE_IDENTIFIER)
      {
        error("Can only assign to variables");
      }
      Identifier *id = (Identifier *)left;
      string name = id->name;
      delete left; // Clean up

      nextToken();                 // eat =
      Expr *value = parseAssign(); // Right associative
      return new Assignment(name, value);
    }

    return left;
  }

  Expr *parseExpression()
  {
    return parseAssign();
  }

  // ===== STATEMENT PARSING =====

  Stmt *parseVarDecl()
  {
    Token typeToken = getToken();

    if (!isToken(T_IDENTIFIER_RL))
    {
      error("Expected variable name");
    }
    string name = getToken().value;

    Expr *init = nullptr;
    if (eatToken(T_ASSIGNOP_RL))
    {
      init = parseExpression();
    }

    if (!eatToken(T_DOT_RL))
    {
      error("Expected '.' after variable declaration");
    }

    return new VarDecl(typeToken.type, name, init);
  }

  Stmt *parseBlock()
  {
    Block *block = new Block();

    while (!isToken(T_BRACER_RL) && !isEnd())
    {
      block->stmts.push_back(parseStatement());
    }

    if (!eatToken(T_BRACER_RL))
    {
      error("Expected '}' after block");
    }

    return block;
  }

  Stmt *parseIfStatement()
  {
    if (!eatToken(T_PARENL_RL))
    {
      error("Expected '(' after if");
    }

    Expr *condition = parseExpression();

    if (!eatToken(T_PARENR_RL))
    {
      error("Expected ')' after if condition");
    }

    if (!eatToken(T_BRACEL_RL))
    {
      error("Expected '{' before if body");
    }

    Stmt *thenBranch = parseBlock();
    Stmt *elseBranch = nullptr;

    if (eatToken(T_ELSE_RL) || eatToken(T_WARNA_RL))
    {
      if (!eatToken(T_BRACEL_RL))
      {
        error("Expected '{' before else body");
      }
      elseBranch = parseBlock();
    }

    return new IfStmt(condition, thenBranch, elseBranch);
  }

  Stmt *parseForStatement()
  {
    if (!eatToken(T_PARENL_RL))
    {
      error("Expected '(' after for");
    }

    // Init part
    Stmt *init = nullptr;
    if (isTypeKeyword())
    {
      init = parseVarDecl();
    }
    else if (!isToken(T_DOT_RL))
    {
      Expr *expr = parseExpression();
      if (!eatToken(T_DOT_RL))
      {
        error("Expected '.' after for init");
      }
      init = new ExprStmt(expr);
    }
    else
    {
      nextToken(); // Skip the dot
    }

    // Condition part
    Expr *condition = nullptr;
    if (!isToken(T_DOT_RL))
    {
      condition = parseExpression();
    }
    if (!eatToken(T_DOT_RL))
    {
      error("Expected '.' after for condition");
    }

    // Update part
    Expr *update = nullptr;
    if (!isToken(T_PARENR_RL))
    {
      update = parseExpression();
    }

    if (!eatToken(T_PARENR_RL))
    {
      error("Expected ')' after for clauses");
    }

    if (!eatToken(T_BRACEL_RL))
    {
      error("Expected '{' before for body");
    }

    Stmt *body = parseBlock();

    return new ForStmt(init, condition, update, body);
  }

  Stmt *parseStatement()
  {
    // Return statement
    if (eatToken(T_RETURN_RL) || eatToken(T_WAPSI_RL))
    {
      Expr *expr = nullptr;
      if (!isToken(T_DOT_RL))
      {
        expr = parseExpression();
      }
      if (!eatToken(T_DOT_RL))
      {
        error("Expected '.' after return");
      }
      return new ReturnStmt(expr);
    }

    // Break
    if (eatToken(T_BREAK_RL) || eatToken(T_TORO_RL))
    {
      return new BreakStmt();
    }

    // Continue
    if (eatToken(T_CONTINUE_RL) || eatToken(T_RAKHO_RL))
    {
      return new ContinueStmt();
    }

    // If statement
    if (eatToken(T_IF_RL) || eatToken(T_AGAR_RL))
    {
      return parseIfStatement();
    }

    // While loop
    if (eatToken(T_WHILE_RL) || eatToken(T_JAB_RL))
    {
      if (!eatToken(T_PARENL_RL))
      {
        error("Expected '(' after while");
      }
      Expr *condition = parseExpression();
      if (!eatToken(T_PARENR_RL))
      {
        error("Expected ')' after condition");
      }
      if (!eatToken(T_BRACEL_RL))
      {
        error("Expected '{' before while body");
      }
      Stmt *body = parseBlock();
      return new WhileStmt(condition, body);
    }

    // For loop
    if (eatToken(T_FOR_RL) || eatToken(T_DUHRAO_RL))
    {
      return parseForStatement();
    }

    // Block
    if (eatToken(T_BRACEL_RL))
    {
      return parseBlock();
    }

    // Variable declaration
    if (isTypeKeyword())
    {
      return parseVarDecl();
    }

    // Expression statement
    Expr *expr = parseExpression();
    if (!eatToken(T_DOT_RL))
    {
      error("Expected '.' after expression");
    }
    return new ExprStmt(expr);
  }

  Stmt *parseFunctionDecl()
  {
    TokenType returnType = T_INT_RL;
    if (isTypeKeyword())
    {
      returnType = getToken().type;
    }

    if (!isToken(T_IDENTIFIER_RL))
    {
      error("Expected function name");
    }
    string name = getToken().value;

    if (!eatToken(T_PARENL_RL))
    {
      error("Expected '(' after function name");
    }

    vector<Param> params;
    while (!isToken(T_PARENR_RL) && !isEnd())
    {
      if (!isTypeKeyword())
      {
        error("Expected parameter type");
      }
      TokenType paramType = getToken().type;

      if (!isToken(T_IDENTIFIER_RL))
      {
        error("Expected parameter name");
      }
      string paramName = getToken().value;

      params.push_back(Param(paramType, paramName));

      if (!eatToken(T_COMMA_RL) && !isToken(T_PARENR_RL))
      {
        error("Expected ',' or ')' in parameters");
      }
    }

    if (!eatToken(T_PARENR_RL))
    {
      error("Expected ')' after parameters");
    }

    if (!eatToken(T_BRACEL_RL))
    {
      error("Expected '{' before function body");
    }

    Stmt *body = parseBlock();

    if (!eatToken(T_DOT_RL))
    {
      error("Expected '.' after function");
    }

    return new FunctionDecl(returnType, name, params, body);
  }

public:
  Parser(vector<Token> tokenList)
  {
    tokens = tokenList;
    pos = 0;
  }

  vector<Stmt *> parse()
  {
    vector<Stmt *> program;

    while (!isEnd())
    {
      if (eatToken(T_FUNCTION_RL) || eatToken(T_FN_RL))
      {
        program.push_back(parseFunctionDecl());
      }
      else
      {
        program.push_back(parseStatement());
      }
    }

    return program;
  }
};

#endif
