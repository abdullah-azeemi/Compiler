#ifndef SIMPLE_AST_HPP
#define SIMPLE_AST_HPP

#include <string>
#include <vector>
#include "Utilities/token_types.hpp"

using namespace std;

enum NodeType
{
  NODE_INT_LIT,
  NODE_FLOAT_LIT,
  NODE_STRING_LIT,
  NODE_BOOL_LIT,
  NODE_IDENTIFIER,
  NODE_BINARY_OP,
  NODE_UNARY_OP,
  NODE_ASSIGNMENT,
  NODE_FUNC_CALL,
  NODE_VAR_DECL,
  NODE_EXPR_STMT,
  NODE_RETURN,
  NODE_BREAK,
  NODE_CONTINUE,
  NODE_BLOCK,
  NODE_IF,
  NODE_WHILE,
  NODE_FOR,
  NODE_FUNC_DECL
};

class ASTNode
{
public:
  NodeType nodeType;
  virtual ~ASTNode() {}
};

class Expr : public ASTNode
{
public:
  virtual ~Expr() {}
};

class IntLiteral : public Expr
{
public:
  int value;
  IntLiteral(int v) : value(v) { nodeType = NODE_INT_LIT; }
};

class FloatLiteral : public Expr
{
public:
  double value;
  FloatLiteral(double v) : value(v) { nodeType = NODE_FLOAT_LIT; }
};

class StringLiteral : public Expr
{
public:
  string value;
  StringLiteral(string v) : value(v) { nodeType = NODE_STRING_LIT; }
};

class BoolLiteral : public Expr
{
public:
  bool value;
  BoolLiteral(bool v) : value(v) { nodeType = NODE_BOOL_LIT; }
};

class Identifier : public Expr
{
public:
  string name;
  Identifier(string n) : name(n) { nodeType = NODE_IDENTIFIER; }
};

class BinaryOp : public Expr
{
public:
  TokenType op;
  Expr *left;
  Expr *right;
  BinaryOp(TokenType o, Expr *l, Expr *r) : op(o), left(l), right(r)
  {
    nodeType = NODE_BINARY_OP;
  }
  ~BinaryOp()
  {
    delete left;
    delete right;
  }
};

class UnaryOp : public Expr
{
public:
  TokenType op;
  Expr *operand;
  UnaryOp(TokenType o, Expr *e) : op(o), operand(e)
  {
    nodeType = NODE_UNARY_OP;
  }
  ~UnaryOp() { delete operand; }
};

class Assignment : public Expr
{
public:
  string ident;
  Expr *value;
  Assignment(string i, Expr *v) : ident(i), value(v)
  {
    nodeType = NODE_ASSIGNMENT;
  }
  ~Assignment() { delete value; }
};

class FunctionCall : public Expr
{
public:
  string name;
  vector<Expr *> args;
  FunctionCall(string n) : name(n) { nodeType = NODE_FUNC_CALL; }
  ~FunctionCall()
  {
    for (auto arg : args)
      delete arg;
  }
};

class Stmt : public ASTNode
{
public:
  virtual ~Stmt() {}
};

class VarDecl : public Stmt
{
public:
  TokenType type;
  string ident;
  Expr *expr;
  VarDecl(TokenType t, string i, Expr *e = nullptr)
      : type(t), ident(i), expr(e)
  {
    nodeType = NODE_VAR_DECL;
  }
  ~VarDecl() { delete expr; }
};

class ExprStmt : public Stmt
{
public:
  Expr *expr;
  ExprStmt(Expr *e) : expr(e) { nodeType = NODE_EXPR_STMT; }
  ~ExprStmt() { delete expr; }
};

class ReturnStmt : public Stmt
{
public:
  Expr *expr;
  ReturnStmt(Expr *e = nullptr) : expr(e) { nodeType = NODE_RETURN; }
  ~ReturnStmt() { delete expr; }
};

class BreakStmt : public Stmt
{
public:
  BreakStmt() { nodeType = NODE_BREAK; }
};

class ContinueStmt : public Stmt
{
public:
  ContinueStmt() { nodeType = NODE_CONTINUE; }
};

class Block : public Stmt
{
public:
  vector<Stmt *> stmts;
  Block() { nodeType = NODE_BLOCK; }
  ~Block()
  {
    for (auto stmt : stmts)
      delete stmt;
  }
};

class IfStmt : public Stmt
{
public:
  Expr *condition;
  Stmt *thenBranch;
  Stmt *elseBranch;
  IfStmt(Expr *c, Stmt *t, Stmt *e = nullptr)
      : condition(c), thenBranch(t), elseBranch(e)
  {
    nodeType = NODE_IF;
  }
  ~IfStmt()
  {
    delete condition;
    delete thenBranch;
    delete elseBranch;
  }
};

class WhileStmt : public Stmt
{
public:
  Expr *condition;
  Stmt *body;
  WhileStmt(Expr *c, Stmt *b) : condition(c), body(b)
  {
    nodeType = NODE_WHILE;
  }
  ~WhileStmt()
  {
    delete condition;
    delete body;
  }
};

class ForStmt : public Stmt
{
public:
  Stmt *init;
  Expr *condition;
  Expr *update;
  Stmt *body;
  ForStmt(Stmt *i, Expr *c, Expr *u, Stmt *b)
      : init(i), condition(c), update(u), body(b)
  {
    nodeType = NODE_FOR;
  }
  ~ForStmt()
  {
    delete init;
    delete condition;
    delete update;
    delete body;
  }
};

struct Param
{
  TokenType type;
  string name;
  Param(TokenType t, string n) : type(t), name(n) {}
};

class FunctionDecl : public Stmt
{
public:
  TokenType returnType;
  string name;
  vector<Param> params;
  Stmt *body;
  FunctionDecl(TokenType rt, string n, vector<Param> p, Stmt *b)
      : returnType(rt), name(n), params(p), body(b)
  {
    nodeType = NODE_FUNC_DECL;
  }
  ~FunctionDecl() { delete body; }
};

#endif
