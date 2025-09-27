#ifndef PARSER_HPP
#define PARSER_HPP

#include "Utilities/token_types.hpp"
#include "ast.hpp"
#include <stdexcept>
#include <sstream>
#include <iostream>

using namespace std;

enum class ParseError
{
  UnexpectedEOF,
  FailedToFindToken,
  ExpectedTypeToken,
  ExpectedIdentifier,
  UnexpectedToken,
  ExpectedFloatLit,
  ExpectedIntLit,
  ExpectedStringLit,
  ExpectedBoolLit,
  ExpectedExpr
};

class ParseException : public std::runtime_error
{
public:
  ParseError error;
  Token token;

  ParseException(ParseError e, const Token &t, const std::string &msg)
      : std::runtime_error(msg), error(e), token(t) {}
};

class Parser
{
private:
  vector<Token> tokens;
  size_t current = 0;

  bool isAtEnd()
  {
    return peek().type == T_EOF_RL;
  }

  Token peek(int offset = 0)
  {
    size_t index = current + offset;
    if (index >= tokens.size())
      return tokens.back();
    return tokens[index];
  }

  Token advance()
  {
    if (!isAtEnd())
      current++;
    return previous();
  }

  Token previous()
  {
    if (current == 0)
      return tokens[0];
    return tokens[current - 1];
  }

  bool check(TokenType type)
  {
    if (isAtEnd())
      return false;
    return peek().type == type;
  }

  bool match(std::initializer_list<TokenType> types)
  {
    for (auto type : types)
    {
      if (check(type))
      {
        advance();
        return true;
      }
    }
    return false;
  }

  Token consume(TokenType type, ParseError error, const std::string &message)
  {
    if (check(type))
      return advance();
    throw ParseException(error, peek(), message + " at line " + std::to_string(peek().line) + ", col " + std::to_string(peek().column));
  }

  bool isType(TokenType type)
  {
    return type == T_INT_RL || type == T_FLOAT_RL ||
           type == T_STRING_RL || type == T_BOOL_RL ||
           type == T_GINTI_RL;
  }

  ExprPtr parsePrimary()
  {
    if (match({T_INT_RLLIT}))
    {
      return make_shared<Expr>(IntLiteral(stoi(previous().value)));
    }
    if (match({T_FLOAT_RLLIT}))
    {
      return make_shared<Expr>(FloatLiteral(stod(previous().value)));
    }
    if (match({T_STRING_RLLIT}))
    {
      return make_shared<Expr>(StringLiteral(previous().value));
    }
    if (match({T_BOOL_RLLIT}))
    {
      return make_shared<Expr>(BoolLiteral(previous().value == "true"));
    }
    if (match({T_IDENTIFIER_RL}))
    {
      string ident = previous().value;

      if (match({T_PARENL_RL}))
      {
        vector<ExprPtr> args;
        if (!check(T_PARENR_RL))
        {
          do
          {
            args.push_back(parseExpression());
          } while (match({T_COMMA_RL}));
        }
        consume(T_PARENR_RL, ParseError::FailedToFindToken, "Expected ')' after arguments");
        return std::make_shared<Expr>(FunctionCall(ident, args));
      }

      if (match({T_BRACKETL_RL}))
      {
        ExprPtr index = parseExpression();
        consume(T_BRACKETR_RL, ParseError::FailedToFindToken, "Expected ']' after array index");
        return std::make_shared<Expr>(
            ArrayAccess(std::make_shared<Expr>(Identifier(ident)), index));
      }

      return make_shared<Expr>(Identifier(ident));
    }
    if (match({T_PARENL_RL}))
    {
      ExprPtr expr = parseExpression();
      consume(T_PARENR_RL, ParseError::FailedToFindToken, "Expected ')' after expression");
      return expr;
    }

    throw ParseException(ParseError::ExpectedExpr, peek(), "Expected expression");
  }

  ExprPtr parseUnary()
  {
    if (match({T_NOT_RL, T_MINUS_RL}))
    {
      Token op = previous();
      ExprPtr right = parseUnary();
      return make_shared<Expr>(UnaryOp(op.type, right));
    }
    return parsePrimary();
  }

  ExprPtr parseMultiplicative()
  {
    ExprPtr expr = parseUnary();

    while (match({T_MUL_RL, T_DIV_RL, T_MOD_RL}))
    {
      Token op = previous();
      ExprPtr right = parseUnary();
      expr = make_shared<Expr>(BinaryOp(op.type, expr, right));
    }

    return expr;
  }

  ExprPtr parseAdditive()
  {
    ExprPtr expr = parseMultiplicative();

    while (match({T_PLUS_RL, T_MINUS_RL}))
    {
      Token op = previous();
      ExprPtr right = parseMultiplicative();
      expr = make_shared<Expr>(BinaryOp(op.type, expr, right));
    }

    return expr;
  }

  ExprPtr parseBitwise()
  {
    ExprPtr expr = parseAdditive();

    while (match({T_AND_BIT_RL, T_OR_BIT_RL, T_XOR_BIT_RL}))
    {
      Token op = previous();
      ExprPtr right = parseAdditive();
      expr = make_shared<Expr>(BinaryOp(op.type, expr, right));
    }

    return expr;
  }

  ExprPtr parseComparison()
  {
    ExprPtr expr = parseBitwise();

    while (match({T_LESS_THAN_RL, T_GREATER_THAN_RL, T_LESS_EQUAL_RL, T_GREATER_EQUAL_RL}))
    {
      Token op = previous();
      ExprPtr right = parseBitwise();
      expr = make_shared<Expr>(BinaryOp(op.type, expr, right));
    }

    return expr;
  }
  ExprPtr parseEquality()
  {
    ExprPtr expr = parseComparison();

    while (match({T_EQUALSOP_RL, T_NOT_EQUALS_RL}))
    {
      Token op = previous();
      ExprPtr right = parseComparison();
      expr = make_shared<Expr>(BinaryOp(op.type, expr, right));
    }

    return expr;
  }

  ExprPtr parseLogicalAnd()
  {
    ExprPtr expr = parseEquality();

    while (match({T_AND_LOGICAL_RL}))
    {
      Token op = previous();
      ExprPtr right = parseEquality();
      expr = make_shared<Expr>(BinaryOp(op.type, expr, right));
    }

    return expr;
  }

  ExprPtr parseLogicalOr()
  {
    ExprPtr expr = parseLogicalAnd();

    while (match({T_OR_LOGICAL_RL}))
    {
      Token op = previous();
      ExprPtr right = parseLogicalAnd();
      expr = make_shared<Expr>(BinaryOp(op.type, expr, right));
    }

    return expr;
  }

  ExprPtr parseAssignment()
  {
    ExprPtr expr = parseLogicalOr();

    if (match({T_ASSIGNOP_RL}))
    {
      if (auto *ident = get_if<Identifier>(&expr->value))
      {
        ExprPtr value = parseAssignment();
        return make_shared<Expr>(Assignment(ident->name, value));
      }
      throw ParseException(ParseError::UnexpectedToken, previous(), "Invalid assignment target");
    }

    return expr;
  }

  ExprPtr parseExpression()
  {
    return parseAssignment();
  }

  StmtPtr parseVarDecl()
  {
    Token type = previous();
    Token name = consume(T_IDENTIFIER_RL, ParseError::ExpectedIdentifier, "Expected variable name");

    ExprPtr initializer = nullptr;
    if (match({T_ASSIGNOP_RL}))
    {
      initializer = parseExpression();
    }

    consume(T_DOT_RL, ParseError::FailedToFindToken, "Expected '.' after variable declaration");
    return make_shared<Stmt>(VarDecl(type.type, name.value, initializer));
  }

  StmtPtr parseBlock()
  {
    std::vector<StmtPtr> statements;

    while (!check(T_BRACER_RL) && !isAtEnd())
    {
      statements.push_back(parseStatement());
    }

    consume(T_BRACER_RL, ParseError::FailedToFindToken,
            "Expected '}' after block");
    return make_shared<Stmt>(Block(statements));
  }

  StmtPtr parseIfStatement()
  {
    consume(T_PARENL_RL, ParseError::FailedToFindToken, "Expected '(' after 'if'");
    ExprPtr condition = parseExpression();
    consume(T_PARENR_RL, ParseError::FailedToFindToken, "Expected ')' after if condition");

    consume(T_BRACEL_RL, ParseError::FailedToFindToken, "Expected '{' before if body");
    StmtPtr thenBranch = parseBlock();

    StmtPtr elseBranch = nullptr;
    if (match({T_ELSE_RL, T_WARNA_RL}))
    {
      consume(T_BRACEL_RL, ParseError::FailedToFindToken, "Expected '{' before else body");
      elseBranch = parseBlock();
    }

    return make_shared<Stmt>(IfStmt(condition, thenBranch, elseBranch));
  }

  StmtPtr parseForStatement()
  {
    consume(T_PARENL_RL, ParseError::FailedToFindToken, "Expected '(' after 'for'");

    StmtPtr init = nullptr;
    if (isType(peek().type))
    {
      advance();
      init = parseVarDecl();
    }
    else if (!check(T_DOT_RL))
    {
      ExprPtr expr = parseExpression();
      consume(T_DOT_RL, ParseError::FailedToFindToken, "Expected '.' after for init");
      init = std::make_shared<Stmt>(ExprStmt(expr));
    }
    else
    {
      advance();
    }

    ExprPtr condition = nullptr;
    if (!check(T_DOT_RL))
    {
      condition = parseExpression();
    }
    consume(T_DOT_RL, ParseError::FailedToFindToken, "Expected '.' after for condition");

    ExprPtr update = nullptr;
    if (!check(T_PARENR_RL))
    {
      update = parseExpression();
    }
    consume(T_PARENR_RL, ParseError::FailedToFindToken, "Expected ')' after for clauses");

    consume(T_BRACEL_RL, ParseError::FailedToFindToken, "Expected '{' before for body");
    StmtPtr body = parseBlock();

    return make_shared<Stmt>(ForStmt(init, condition, update, body));
  }

  StmtPtr parseStatement()
  {
    if (match({T_RETURN_RL, T_WAPSI_RL}))
    {
      ExprPtr expr = nullptr;
      if (!check(T_DOT_RL))
      {
        expr = parseExpression();
      }
      consume(T_DOT_RL, ParseError::FailedToFindToken, "Expected '.' after return statement");
      return make_shared<Stmt>(ReturnStmt(expr));
    }

    if (match({T_BREAK_RL, T_TORO_RL}))
    {
      return make_shared<Stmt>(BreakStmt());
    }

    if (match({T_CONTINUE_RL, T_RAKHO_RL}))
    {
      return make_shared<Stmt>(ContinueStmt());
    }

    if (match({T_IF_RL, T_AGAR_RL}))
    {
      return parseIfStatement();
    }

    if (match({T_WHILE_RL, T_JAB_RL}))
    {
      consume(T_PARENL_RL, ParseError::FailedToFindToken, "Expected '(' after 'while'");
      ExprPtr condition = parseExpression();
      consume(T_PARENR_RL, ParseError::FailedToFindToken, "Expected ')' after condition");
      consume(T_BRACEL_RL, ParseError::FailedToFindToken, "Expected '{' before while body");
      StmtPtr body = parseBlock();
      return make_shared<Stmt>(WhileStmt(condition, body));
    }

    if (match({T_FOR_RL, T_DUHRAO_RL}))
    {
      return parseForStatement();
    }

    if (match({T_BRACEL_RL}))
    {
      return parseBlock();
    }

    if (isType(peek().type))
    {
      advance();
      return parseVarDecl();
    }

    ExprPtr expr = parseExpression();
    consume(T_DOT_RL, ParseError::FailedToFindToken, "Expected '.' after expression");
    return make_shared<Stmt>(ExprStmt(expr));
  }

  StmtPtr parseFunctionDecl()
  {
    TokenType returnType = T_INT_RL;
    if (isType(peek().type))
    {
      returnType = advance().type;
    }

    Token name = consume(T_IDENTIFIER_RL, ParseError::ExpectedIdentifier, "Expected function name");

    consume(T_PARENL_RL, ParseError::FailedToFindToken, "Expected '(' after function name");

    vector<Param> params;
    if (!check(T_PARENR_RL))
    {
      do
      {
        if (!isType(peek().type))
        {
          throw ParseException(ParseError::ExpectedTypeToken, peek(), "Expected parameter type");
        }
        TokenType paramType = advance().type;
        Token paramName = consume(T_IDENTIFIER_RL, ParseError::ExpectedIdentifier, "Expected parameter name");
        params.push_back(Param(paramType, paramName.value));
      } while (match({T_COMMA_RL}));
    }

    consume(T_PARENR_RL, ParseError::FailedToFindToken, "Expected ')' after parameters");
    consume(T_BRACEL_RL, ParseError::FailedToFindToken, "Expected '{' before function body");

    StmtPtr body = parseBlock();
    consume(T_DOT_RL, ParseError::FailedToFindToken, "Expected '.' after function declaration");

    return make_shared<Stmt>(FunctionDecl(returnType, name.value, params, body));
  }

public:
  Parser(const vector<Token> &tokenList) : tokens(tokenList) {}

  vector<StmtPtr> parse()
  {
    vector<StmtPtr> statements;

    while (!isAtEnd())
    {
      try
      {
        if (match({T_FUNCTION_RL, T_FN_RL}))
        {
          statements.push_back(parseFunctionDecl());
        }
        else
        {
          statements.push_back(parseStatement());
        }
      }
      catch (const ParseException &e)
      {
        std::cerr << "Parse error: " << e.what() << endl;

        while (!isAtEnd() && !check(T_DOT_RL))
        {
          advance();
        }
        if (check(T_DOT_RL))
          advance();
      }
    }

    return statements;
  }
};

#endif
