%{
#include <stdio.h>
#include <stdlib.h>
%}

/* ---------- TOKENS ---------- */
%token T_FUNCTION_RL T_FN_RL
%token T_RETURN_RL T_WAPSI_RL
%token T_IF_RL T_AGAR_RL
%token T_ELSE_RL T_WARNA_RL
%token T_WHILE_RL T_JAB_RL
%token T_FOR_RL T_DUHRAO_RL
%token T_BREAK_RL T_TORO_RL
%token T_CONTINUE_RL T_RAKHO_RL

%token T_INT_RL T_GINTI_RL
%token T_FLOAT_RL T_STRING_RL T_BOOL_RL

%token T_IDENTIFIER_RL
%token T_INT_RLLIT T_FLOAT_RLLIT T_STRING_RLLIT T_BOOL_RLLIT

%token T_ASSIGNOP_RL
%token T_EQUALSOP_RL T_NOT_EQUALS_RL
%token T_LESS_THAN_RL T_GREATER_THAN_RL T_LESS_EQUAL_RL T_GREATER_EQUAL_RL
%token T_PLUS_RL T_MINUS_RL T_MUL_RL T_DIV_RL T_MOD_RL
%token T_AND_LOGICAL_RL T_OR_LOGICAL_RL T_NOT_RL
%token T_AND_BIT_RL T_OR_BIT_RL T_XOR_BIT_RL

%token T_PARENL_RL T_PARENR_RL
%token T_BRACEL_RL T_BRACER_RL
%token T_BRACKETL_RL T_BRACKETR_RL
%token T_SEMICOLON_RL T_COMMA_RL T_DOT_RL

%start program

%%

program
    : /* empty */
    | program top_level
    ;

top_level
    : function_decl
    | statement
    ;


function_decl
    : (T_FUNCTION_RL | T_FN_RL) type_specifier_opt T_IDENTIFIER_RL
      T_PARENL_RL param_list_opt T_PARENR_RL
      T_BRACEL_RL stmt_list_opt T_BRACER_RL T_DOT_RL
    ;


param_list_opt
    : /* empty */
    | param_list
    ;

param_list
    : param_decl
    | param_list T_COMMA_RL param_decl
    ;

param_decl
    : type_specifier T_IDENTIFIER_RL
    ;


type_specifier
    : T_INT_RL
    | T_FLOAT_RL
    | T_STRING_RL
    | T_BOOL_RL
    | T_GINTI_RL
    ;

type_specifier_opt
    : 
    | type_specifier
    ;

statement
    : var_decl
    | if_stmt
    | while_stmt
    | for_stmt
    | return_stmt
    | break_stmt
    | continue_stmt
    | block
    | expr_stmt
    ;

stmt_list_opt
    : 
    | stmt_list
    ;

stmt_list
    : statement
    | stmt_list statement
    ;


var_decl
    : type_specifier T_IDENTIFIER_RL var_init_opt T_DOT_RL
    ;

var_init_opt
    : 
    | T_ASSIGNOP_RL expression
    ;


expr_stmt
    : expression T_DOT_RL
    ;


return_stmt
    : (T_RETURN_RL | T_WAPSI_RL) expression_opt T_DOT_RL
    ;

break_stmt
    : (T_BREAK_RL | T_TORO_RL)
    ;

continue_stmt
    : (T_CONTINUE_RL | T_RAKHO_RL)
    ;

expression_opt
    : 
    | expression
    ;


if_stmt
    : (T_IF_RL | T_AGAR_RL) T_PARENL_RL expression T_PARENR_RL
      T_BRACEL_RL stmt_list_opt T_BRACER_RL
      else_part_opt
    ;

else_part_opt
    :
    | (T_ELSE_RL | T_WARNA_RL) T_BRACEL_RL stmt_list_opt T_BRACER_RL
    ;


while_stmt
    : (T_WHILE_RL | T_JAB_RL) T_PARENL_RL expression T_PARENR_RL
      T_BRACEL_RL stmt_list_opt T_BRACER_RL
    ;


for_stmt
    : (T_FOR_RL | T_DUHRAO_RL) T_PARENL_RL for_init_opt T_DOT_RL for_cond_opt T_DOT_RL for_update_opt T_PARENR_RL
      T_BRACEL_RL stmt_list_opt T_BRACER_RL
    ;

for_init_opt
    : 
    | var_decl
    | expression
    ;

for_cond_opt
    : 
    | expression
    ;

for_update_opt
    : 
    | expression
    ;


block
    : T_BRACEL_RL stmt_list_opt T_BRACER_RL
    ;


expression
    : assignment
    ;

assignment
    : logical_or
    | T_IDENTIFIER_RL T_ASSIGNOP_RL assignment
    ;

logical_or
    : logical_and
    | logical_or T_OR_LOGICAL_RL logical_and
    ;

logical_and
    : equality
    | logical_and T_AND_LOGICAL_RL equality
    ;

equality
    : comparison
    | equality (T_EQUALSOP_RL | T_NOT_EQUALS_RL) comparison
    ;

comparison
    : add
    | comparison (T_LESS_THAN_RL | T_GREATER_THAN_RL | T_LESS_EQUAL_RL | T_GREATER_EQUAL_RL) add
    ;

add
    : multiply
    | add (T_PLUS_RL | T_MINUS_RL) multiply
    ;

multiply
    : unary
    | multiply (T_MUL_RL | T_DIV_RL | T_MOD_RL) unary
    ;

unary
    : primary
    | (T_MINUS_RL | T_NOT_RL) unary
    ;

primary
    : T_INT_RLLIT
    | T_FLOAT_RLLIT
    | T_STRING_RLLIT
    | T_BOOL_RLLIT
    | T_IDENTIFIER_RL
    | T_IDENTIFIER_RL T_PARENL_RL arg_list_opt T_PARENR_RL
    | T_PARENL_RL expression T_PARENR_RL
    ;

arg_list_opt
    : 
    | arg_list
    ;

arg_list
    : expression
    | arg_list T_COMMA_RL expression
    ;

%%

int main() {
    yyparse();
    printf("Parsing completed successfully.\n");
    return 0;
}
