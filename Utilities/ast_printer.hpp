#ifndef AST_PRINTER_HPP
#define AST_PRINTER_HPP

#include <iostream>
#include <sstream>
#include "../ast.hpp"
#include "token_types.hpp"

using namespace std;

class ASTPrinter
{
private:
  static string indent(int level)
  {
    return string(level * 4, ' ');
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

  static void printExpr(const ExprPtr &expr, int level, ostringstream &out)
  {
    if (!expr)
    {
      out << "None";
      return;
    }

    std::visit([&](const auto &e)
               {
            using T = std::decay_t<decltype(e)>;
            
            if constexpr (std::is_same_v<T, IntLiteral>) {
                out << e.value;
            }
            else if constexpr (std::is_same_v<T, FloatLiteral>) {
                out << e.value;
            }
            else if constexpr (std::is_same_v<T, StringLiteral>) {
                out << "\"" << e.value << "\"";
            }
            else if constexpr (std::is_same_v<T, BoolLiteral>) {
                out << (e.value ? "true" : "false");
            }
            else if constexpr (std::is_same_v<T, Identifier>) {
                out << "\"" << e.name << "\"";
            }
            else if constexpr (std::is_same_v<T, BinaryOp>) {
                out << "::" << tokenTypeToString(e.op) << "\n";
                out << indent(level + 1);
                printExpr(e.left, level + 1, out);
                out << "\n" << indent(level + 1);
                printExpr(e.right, level + 1, out);
            }
            else if constexpr (std::is_same_v<T, UnaryOp>) {
                out << "Unary(" << tokenTypeToString(e.op) << ")\n";
                out << indent(level + 1);
                printExpr(e.operand, level + 1, out);
            }
            else if constexpr (std::is_same_v<T, Assignment>) {
                out << "Assign(" << tokenTypeToString(T_ASSIGNOP_RL) << ")\n";
                out << indent(level + 1) << "\"" << e.ident << "\"\n";
                out << indent(level + 1);
                printExpr(e.value, level + 1, out);
            }
            else if constexpr (std::is_same_v<T, FunctionCall>) {
                out << "Call(FnCall {\n";
                out << indent(level + 1) << "ident: \"" << e.name << "\",\n";
                out << indent(level + 1) << "args: [\n";
                for (const auto& arg : e.args) {
                    out << indent(level + 2) << "Some(\n";
                    out << indent(level + 3);
                    printExpr(arg, level + 3, out);
                    out << ",\n" << indent(level + 2) << "),\n";
                }
                out << indent(level + 1) << "],\n";
                out << indent(level) << "})";
            } }, expr->value);
  }

  static void printStmt(const StmtPtr &stmt, int level, std::ostringstream &out)
  {
    if (!stmt)
      return;

    visit([&](const auto &s)
          {
            using T = std::decay_t<decltype(s)>;
            
            if constexpr (std::is_same_v<T, FunctionDecl>) {
                out << indent(level) << "Fn(\n";
                out << indent(level + 1) << "FnDecl {\n";
                out << indent(level + 2) << "type_tok: " << tokenTypeToString(s.returnType) << ",\n";
                out << indent(level + 2) << "ident: \"" << s.name << "\",\n";
                out << indent(level + 2) << "params: [\n";
                for (const auto& param : s.params) {
                    out << indent(level + 3) << "Param {\n";
                    out << indent(level + 4) << "type_tok: " << tokenTypeToString(param.type) << ",\n";
                    out << indent(level + 4) << "ident: \"" << param.name << "\",\n";
                    out << indent(level + 3) << "},\n";
                }
                out << indent(level + 2) << "],\n";
                out << indent(level + 2) << "block: [\n";
                printStmt(s.body, level + 3, out);
                out << indent(level + 2) << "],\n";
                out << indent(level + 1) << "},\n";
                out << indent(level) << "),\n";
            }
            else if constexpr (std::is_same_v<T, VarDecl>) {
                out << indent(level) << "Var(\n";
                out << indent(level + 1) << "VarDecl {\n";
                out << indent(level + 2) << "type_tok: " << tokenTypeToString(s.type) << ",\n";
                out << indent(level + 2) << "ident: \"" << s.ident << "\",\n";
                out << indent(level + 2) << "expr: ";
                if (s.expr) {
                    out << "Some(\n" << indent(level + 3);
                    printExpr(s.expr, level + 3, out);
                    out << ",\n" << indent(level + 2) << ")";
                } else {
                    out << "None";
                }
                out << ",\n";
                out << indent(level + 1) << "},\n";
                out << indent(level) << "),\n";
            }
            else if constexpr (std::is_same_v<T, Block>) {
                for (const auto& stmt : s.stmts) {
                    printStmt(stmt, level, out);
                }
            }
            else if constexpr (std::is_same_v<T, ReturnStmt>) {
                out << indent(level) << "Ret(\n";
                out << indent(level + 1) << "ExprStmt {\n";
                out << indent(level + 2) << "expr: ";
                if (s.expr) {
                    out << "Some(\n" << indent(level + 3);
                    printExpr(s.expr, level + 3, out);
                    out << "\n" << indent(level + 2) << "),\n";
                } else {
                    out << "None,\n";
                }
                out << indent(level + 1) << "},\n";
                out << indent(level) << "),\n";
            }
            else if constexpr (std::is_same_v<T, IfStmt>) {
                out << indent(level) << "If(\n";
                out << indent(level + 1) << "IfStmt {\n";
                out << indent(level + 2) << "cond: Some(\n";
                out << indent(level + 3);
                printExpr(s.condition, level + 3, out);
                out << "\n" << indent(level + 2) << "),\n";
                out << indent(level + 2) << "if_block: [\n";
                printStmt(s.thenBranch, level + 3, out);
                out << indent(level + 2) << "],\n";
                out << indent(level + 2) << "else_block: ";
                if (s.elseBranch) {
                    out << "[\n";
                    printStmt(s.elseBranch, level + 3, out);
                    out << indent(level + 2) << "]";
                } else {
                    out << "[]";
                }
                out << ",\n";
                out << indent(level + 1) << "},\n";
                out << indent(level) << "),\n";
            }
            else if constexpr (std::is_same_v<T, ForStmt>) {
                out << indent(level) << "For(\n";
                out << indent(level + 1) << "ForStmt {\n";
                out << indent(level + 2) << "init: ";
                if (s.init) {
                    out << "Some(\n";
                    printStmt(s.init, level + 3, out);
                    out << indent(level + 2) << "),\n";
                } else {
                    out << "None,\n";
                }
                out << indent(level + 2) << "cond: ExprStmt {\n";
                out << indent(level + 3) << "expr: ";
                if (s.condition) {
                    out << "Some(\n" << indent(level + 4);
                    printExpr(s.condition, level + 4, out);
                    out << "\n" << indent(level + 3) << "),\n";
                } else {
                    out << "None,\n";
                }
                out << indent(level + 2) << "},\n";
                out << indent(level + 2) << "updt: ";
                if (s.update) {
                    out << "Some(\n" << indent(level + 3);
                    printExpr(s.update, level + 3, out);
                    out << "\n" << indent(level + 2) << "),\n";
                } else {
                    out << "None,\n";
                }
                out << indent(level + 2) << "block: [\n";
                printStmt(s.body, level + 3, out);
                out << indent(level + 2) << "],\n";
                out << indent(level + 1) << "},\n";
                out << indent(level) << "),\n";
            }
            else if constexpr (std::is_same_v<T, BreakStmt>) {
                out << indent(level) << "Break,\n";
            }
            else if constexpr (std::is_same_v<T, ContinueStmt>) {
                out << indent(level) << "Continue,\n";
            }
            else if constexpr (std::is_same_v<T, ExprStmt>) {
                out << indent(level) << "ExprStmt {\n";
                out << indent(level + 1) << "expr: ";
                if (s.expr) {
                    out << "Some(\n" << indent(level + 2);
                    printExpr(s.expr, level + 2, out);
                    out << "\n" << indent(level + 1) << ")";
                } else {
                    out << "None";
                }
                out << ",\n" << indent(level) << "},\n";
            } }, stmt->value);
  }

public:
  static string printAST(const vector<StmtPtr> &ast)
  {
    ostringstream out;
    out << "[\n";
    for (const auto &stmt : ast)
    {
      printStmt(stmt, 1, out);
    }
    out << "]";
    return out.str();
  }
};

#endif
