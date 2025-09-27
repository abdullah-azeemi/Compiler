#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <vector>
#include <string>
#include <variant>
#include "Utilities/token_types.hpp"
using namespace std;

struct Expr;
struct Stmt;

using ExprPtr = std::shared_ptr<Expr>;
using StmtPtr = std::shared_ptr<Stmt>;

// Expression types
struct IntLiteral
{
  int value;
  IntLiteral(int v) : value(v) {}
};

struct FloatLiteral
{
  double value;
  FloatLiteral(double v) : value(v) {}
};

struct StringLiteral
{
  std::string value;
  StringLiteral(const string &v) : value(v) {}
};

struct BoolLiteral
{
  bool value;
  BoolLiteral(bool v) : value(v) {}
};

struct Identifier
{
  std::string name;
  Identifier(const string &n) : name(n) {}
};

struct BinaryOp
{
  TokenType op;
  ExprPtr left;
  ExprPtr right;
  BinaryOp(TokenType o, ExprPtr l, ExprPtr r) : op(o), left(l), right(r) {}
};

struct UnaryOp
{
  TokenType op;
  ExprPtr operand;
  UnaryOp(TokenType o, ExprPtr e) : op(o), operand(e) {}
};

struct Assignment
{
  std::string ident;
  ExprPtr value;
  Assignment(const std::string &i, ExprPtr v) : ident(i), value(v) {}
};

struct FunctionCall
{
  string name;
  vector<ExprPtr> args;
  FunctionCall(const string &n, vector<ExprPtr> a) : name(n), args(a) {}
};

struct ArrayAccess
{
  ExprPtr array;
  ExprPtr index;
  ArrayAccess(ExprPtr a, ExprPtr i) : array(a), index(i) {}
};

struct Expr
{
  variant<
      IntLiteral,
      FloatLiteral,
      StringLiteral,
      BoolLiteral,
      Identifier,
      BinaryOp,
      UnaryOp,
      Assignment,
      FunctionCall,
      ArrayAccess>
      value;

  template <typename T>
  Expr(T &&val) : value(std::forward<T>(val)) {}
};

struct VarDecl
{
  TokenType type;
  string ident;
  ExprPtr expr;
  VarDecl(TokenType t, const string &i, ExprPtr e = nullptr)
      : type(t), ident(i), expr(e) {}
};

struct ExprStmt
{
  ExprPtr expr;
  ExprStmt(ExprPtr e = nullptr) : expr(e) {}
};

struct ReturnStmt
{
  ExprPtr expr;
  ReturnStmt(ExprPtr e = nullptr) : expr(e) {}
};

struct BreakStmt
{
};
struct ContinueStmt
{
};

struct Block
{
  vector<StmtPtr> stmts;
  Block(vector<StmtPtr> s = {}) : stmts(s) {}
};

struct IfStmt
{
  ExprPtr condition;
  StmtPtr thenBranch;
  StmtPtr elseBranch;
  IfStmt(ExprPtr c, StmtPtr t, StmtPtr e = nullptr)
      : condition(c), thenBranch(t), elseBranch(e) {}
};

struct WhileStmt
{
  ExprPtr condition;
  StmtPtr body;
  WhileStmt(ExprPtr c, StmtPtr b) : condition(c), body(b) {}
};

struct ForStmt
{
  StmtPtr init;
  ExprPtr condition;
  ExprPtr update;
  StmtPtr body;
  ForStmt(StmtPtr i, ExprPtr c, ExprPtr u, StmtPtr b)
      : init(i), condition(c), update(u), body(b) {}
};

struct Param
{
  TokenType type;
  string name;
  Param(TokenType t, const std::string &n) : type(t), name(n) {}
};

struct FunctionDecl
{
  TokenType returnType;
  string name;
  vector<Param> params;
  StmtPtr body;
  FunctionDecl(TokenType rt, const string &n, vector<Param> p, StmtPtr b)
      : returnType(rt), name(n), params(p), body(b) {}
};

struct Stmt
{
  variant<
      VarDecl,
      ExprStmt,
      ReturnStmt,
      BreakStmt,
      ContinueStmt,
      Block,
      IfStmt,
      WhileStmt,
      ForStmt,
      FunctionDecl>
      value;

  template <typename T>
  Stmt(T &&val) : value(std::forward<T>(val)) {}
};

#endif
