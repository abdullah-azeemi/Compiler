#ifndef TOKEN_TYPES_HPP
#define TOKEN_TYPES_HPP

#include <string>

enum TokenType
{

  T_FUNCTION_RL,
  T_RETURN_RL,
  T_IF_RL,
  T_ELSE_RL,
  T_WHILE_RL,
  T_FOR_RL,
  T_BREAK_RL,
  T_CONTINUE_RL,
  T_INT_RL,
  T_FLOAT_RL,
  T_STRING_RL,
  T_BOOL_RL,

  T_FN_RL,
  T_GINTI_RL,
  T_WAPSI_RL,
  T_AGAR_RL,
  T_WARNA_RL,
  T_DUHRAO_RL,
  T_JAB_RL,
  T_TORO_RL,
  T_RAKHO_RL,

  T_IDENTIFIER_RL,
  T_INT_RLLIT,
  T_FLOAT_RLLIT,
  T_STRING_RLLIT,
  T_BOOL_RLLIT,

  T_ASSIGNOP_RL,
  T_EQUALSOP_RL,
  T_NOT_EQUALS_RL,
  T_LESS_THAN_RL,
  T_GREATER_THAN_RL,
  T_LESS_EQUAL_RL,
  T_GREATER_EQUAL_RL,
  T_PLUS_RL,
  T_MINUS_RL,
  T_MUL_RL,
  T_DIV_RL,
  T_MOD_RL,
  T_AND_LOGICAL_RL,
  T_OR_LOGICAL_RL,
  T_NOT_RL,
  T_AND_BIT_RL,
  T_OR_BIT_RL,
  T_XOR_BIT_RL,

  T_PARENL_RL,
  T_PARENR_RL,
  T_BRACEL_RL,
  T_BRACER_RL,
  T_BRACKETL_RL,
  T_BRACKETR_RL,
  T_SEMICOLON_RL,
  T_COMMA_RL,
  T_DOT_RL,

  T_EOF_RL,
  T_UNKNOWN_RL
};

struct Token
{
  TokenType type;
  std::string value;
  int line;
  int column;

  Token(TokenType t = T_UNKNOWN_RL, const std::string &v = "", int l = 1, int c = 1)
      : type(t), value(v), line(l), column(c) {}
};

#endif
