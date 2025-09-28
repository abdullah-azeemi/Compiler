#ifndef AST_PRINTER_HPP
#define AST_PRINTER_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "../ast.hpp"
#include "token_types.hpp"

using namespace std;

class ASTPrinter
{
private:
  static string indent(int level)
  {
    string result = "";
    for (int i = 0; i < level * 4; i++)
    {
      result += " ";
    }
    return result;
  }

  static string tokenTypeToString(TokenType type)
  {
    switch (type)
    {
    case T_INT_RL:
    case T_GINTI_RL:
      return "Int";
    case T_FLOAT_RL:
      return "Float";
    case T_STRING_RL:
      return "String";
    case T_BOOL_RL:
      return "Bool";
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
    case T_EQUALSOP_RL:
      return "EqualsOp";
    case T_LESS_THAN_RL:
      return "LessThan";
    case T_GREATER_THAN_RL:
      return "GreaterThan";
    case T_LESS_EQUAL_RL:
      return "LessEqual";
    case T_GREATER_EQUAL_RL:
      return "GreaterEqual";
    case T_NOT_EQUALS_RL:
      return "NotEquals";
    case T_AND_LOGICAL_RL:
      return "AndOp";
    case T_OR_LOGICAL_RL:
      return "OrOp";
    case T_NOT_RL:
      return "NotOp";
    case T_ASSIGNOP_RL:
      return "AssignOp";
    case T_AND_BIT_RL:
      return "BitAndOp";
    case T_OR_BIT_RL:
      return "BitOrOp";
    case T_XOR_BIT_RL:
      return "BitXorOp";
    default:
      return "Unknown";
    }
  }

  static void printExpr(Expr *expr, int level, ostringstream &out)
  {
    if (!expr)
    {
      out << "None";
      return;
    }

    switch (expr->nodeType)
    {
    case NODE_INT_LIT:
    {
      IntLiteral *lit = (IntLiteral *)expr;
      out << lit->value;
      break;
    }
    case NODE_FLOAT_LIT:
    {
      FloatLiteral *lit = (FloatLiteral *)expr;
      out << lit->value;
      break;
    }
    case NODE_STRING_LIT:
    {
      StringLiteral *lit = (StringLiteral *)expr;
      out << "\"" << lit->value << "\"";
      break;
    }
    case NODE_BOOL_LIT:
    {
      BoolLiteral *lit = (BoolLiteral *)expr;
      out << (lit->value ? "true" : "false");
      break;
    }
    case NODE_IDENTIFIER:
    {
      Identifier *id = (Identifier *)expr;
      out << "\"" << id->name << "\"";
      break;
    }
    case NODE_BINARY_OP:
    {
      BinaryOp *op = (BinaryOp *)expr;
      out << "::" << tokenTypeToString(op->op) << "\n";
      out << indent(level + 1);
      printExpr(op->left, level + 1, out);
      out << "\n"
          << indent(level + 1);
      printExpr(op->right, level + 1, out);
      break;
    }
    case NODE_UNARY_OP:
    {
      UnaryOp *op = (UnaryOp *)expr;
      out << "Unary(" << tokenTypeToString(op->op) << ")\n";
      out << indent(level + 1);
      printExpr(op->operand, level + 1, out);
      break;
    }
    case NODE_ASSIGNMENT:
    {
      Assignment *assign = (Assignment *)expr;
      out << "Assign(" << tokenTypeToString(T_ASSIGNOP_RL) << ")\n";
      out << indent(level + 1) << "\"" << assign->ident << "\"\n";
      out << indent(level + 1);
      printExpr(assign->value, level + 1, out);
      break;
    }
    case NODE_FUNC_CALL:
    {
      FunctionCall *call = (FunctionCall *)expr;
      out << "Call(FnCall {\n";
      out << indent(level + 1) << "ident: \"" << call->name << "\",\n";
      out << indent(level + 1) << "args: [\n";
      for (auto arg : call->args)
      {
        out << indent(level + 2) << "Some(\n";
        out << indent(level + 3);
        printExpr(arg, level + 3, out);
        out << ",\n"
            << indent(level + 2) << "),\n";
      }
      out << indent(level + 1) << "],\n";
      out << indent(level) << "})";
      break;
    }
    }
  }

  static void printStmt(Stmt *stmt, int level, ostringstream &out)
  {
    if (!stmt)
      return;

    switch (stmt->nodeType)
    {
    case NODE_VAR_DECL:
    {
      VarDecl *var = (VarDecl *)stmt;
      out << indent(level) << "Var(\n";
      out << indent(level + 1) << "VarDecl {\n";
      out << indent(level + 2) << "type_tok: " << tokenTypeToString(var->type) << ",\n";
      out << indent(level + 2) << "ident: \"" << var->ident << "\",\n";
      out << indent(level + 2) << "expr: ";
      if (var->expr)
      {
        out << "Some(\n"
            << indent(level + 3);
        printExpr(var->expr, level + 3, out);
        out << ",\n"
            << indent(level + 2) << ")";
      }
      else
      {
        out << "None";
      }
      out << ",\n";
      out << indent(level + 1) << "},\n";
      out << indent(level) << "),\n";
      break;
    }
    case NODE_FUNC_DECL:
    {
      FunctionDecl *fn = (FunctionDecl *)stmt;
      out << indent(level) << "Fn(\n";
      out << indent(level + 1) << "FnDecl {\n";
      out << indent(level + 2) << "type_tok: " << tokenTypeToString(fn->returnType) << ",\n";
      out << indent(level + 2) << "ident: \"" << fn->name << "\",\n";
      out << indent(level + 2) << "params: [\n";
      for (auto &p : fn->params)
      {
        out << indent(level + 3) << "Param {\n";
        out << indent(level + 4) << "type_tok: " << tokenTypeToString(p.type) << ",\n";
        out << indent(level + 4) << "ident: \"" << p.name << "\",\n";
        out << indent(level + 3) << "},\n";
      }
      out << indent(level + 2) << "],\n";
      out << indent(level + 2) << "block: [\n";
      printStmt(fn->body, level + 3, out);
      out << indent(level + 2) << "],\n";
      out << indent(level + 1) << "},\n";
      out << indent(level) << "),\n";
      break;
    }
    case NODE_BLOCK:
    {
      Block *block = (Block *)stmt;
      for (auto s : block->stmts)
      {
        printStmt(s, level, out);
      }
      break;
    }
    case NODE_RETURN:
    {
      ReturnStmt *ret = (ReturnStmt *)stmt;
      out << indent(level) << "Ret(\n";
      out << indent(level + 1) << "ExprStmt {\n";
      out << indent(level + 2) << "expr: ";
      if (ret->expr)
      {
        out << "Some(\n"
            << indent(level + 3);
        printExpr(ret->expr, level + 3, out);
        out << "\n"
            << indent(level + 2) << "),\n";
      }
      else
      {
        out << "None,\n";
      }
      out << indent(level + 1) << "},\n";
      out << indent(level) << "),\n";
      break;
    }
    case NODE_IF:
    {
      IfStmt *ifStmt = (IfStmt *)stmt;
      out << indent(level) << "If(\n";
      out << indent(level + 1) << "IfStmt {\n";
      out << indent(level + 2) << "cond: Some(\n";
      out << indent(level + 3);
      printExpr(ifStmt->condition, level + 3, out);
      out << "\n"
          << indent(level + 2) << "),\n";
      out << indent(level + 2) << "if_block: [\n";
      printStmt(ifStmt->thenBranch, level + 3, out);
      out << indent(level + 2) << "],\n";
      out << indent(level + 2) << "else_block: ";
      if (ifStmt->elseBranch)
      {
        out << "[\n";
        printStmt(ifStmt->elseBranch, level + 3, out);
        out << indent(level + 2) << "]";
      }
      else
      {
        out << "[]";
      }
      out << ",\n";
      out << indent(level + 1) << "},\n";
      out << indent(level) << "),\n";
      break;
    }
    case NODE_FOR:
    {
      ForStmt *forStmt = (ForStmt *)stmt;
      out << indent(level) << "For(\n";
      out << indent(level + 1) << "ForStmt {\n";
      out << indent(level + 2) << "init: ";
      if (forStmt->init)
      {
        out << "Some(\n";
        printStmt(forStmt->init, level + 3, out);
        out << indent(level + 2) << "),\n";
      }
      else
      {
        out << "None,\n";
      }
      out << indent(level + 2) << "cond: ExprStmt {\n";
      out << indent(level + 3) << "expr: ";
      if (forStmt->condition)
      {
        out << "Some(\n"
            << indent(level + 4);
        printExpr(forStmt->condition, level + 4, out);
        out << "\n"
            << indent(level + 3) << "),\n";
      }
      else
      {
        out << "None,\n";
      }
      out << indent(level + 2) << "},\n";
      out << indent(level + 2) << "updt: ";
      if (forStmt->update)
      {
        out << "Some(\n"
            << indent(level + 3);
        printExpr(forStmt->update, level + 3, out);
        out << "\n"
            << indent(level + 2) << "),\n";
      }
      else
      {
        out << "None,\n";
      }
      out << indent(level + 2) << "block: [\n";
      printStmt(forStmt->body, level + 3, out);
      out << indent(level + 2) << "],\n";
      out << indent(level + 1) << "},\n";
      out << indent(level) << "),\n";
      break;
    }
    case NODE_WHILE:
    {
      WhileStmt *whileStmt = (WhileStmt *)stmt;
      out << indent(level) << "While(\n";
      out << indent(level + 1) << "WhileStmt {\n";
      out << indent(level + 2) << "cond: ";
      if (whileStmt->condition)
      {
        out << "Some(\n"
            << indent(level + 3);
        printExpr(whileStmt->condition, level + 3, out);
        out << "\n"
            << indent(level + 2) << "),\n";
      }
      else
      {
        out << "None,\n";
      }
      out << indent(level + 2) << "body: [\n";
      printStmt(whileStmt->body, level + 3, out);
      out << indent(level + 2) << "],\n";
      out << indent(level + 1) << "},\n";
      out << indent(level) << "),\n";
      break;
    }
    case NODE_BREAK:
    {
      out << indent(level) << "Break,\n";
      break;
    }
    case NODE_CONTINUE:
    {
      out << indent(level) << "Continue,\n";
      break;
    }
    case NODE_EXPR_STMT:
    {
      ExprStmt *exprStmt = (ExprStmt *)stmt;
      if (exprStmt->expr)
      {
        printExpr(exprStmt->expr, level, out);
      }
      break;
    }
    }
  }

public:
  static string printAST(const vector<Stmt *> &ast)
  {
    ostringstream out;
    out << "[\n";
    for (auto stmt : ast)
    {
      printStmt(stmt, 1, out);
    }
    out << "]";
    return out.str();
  }

  static void cleanup(vector<Stmt *> &ast)
  {
    for (auto stmt : ast)
    {
      delete stmt;
    }
  }
};

class ParseException : public runtime_error
{
public:
  ParseException(const string &msg) : runtime_error(msg) {}
};

#endif
