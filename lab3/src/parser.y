%require "3.8.2"
%language "c++"
%skeleton "lalr1.cc"
%header

%define api.namespace {frontend} 
%define api.parser.class {Parser}
%define api.token.prefix {TOK_}
%define api.token.raw
%define api.token.constructor
%define api.value.type variant

%define parse.assert
%define parse.error detailed
%define parse.trace
%define parse.lac full

%locations

%parse-param {frontend::Scanner& scanner}

%code requires {

namespace frontend {

class Parser;
class Scanner;

}  // namespace frontend

}

%code {

#include "driver.h"

#define yylex scanner.Get

}

%token <std::string>   IDENT   "identifier";
%token <std::int64_t>  NUMBER  "number";

%token
  IF      "if"
  ELSE    "else"
  WHILE   "while"
  RETURN  "return"

  ASSIGN     "="
  COMMA      ","
  SEMICOLON  ";"

  LEFT_PARENTHESIS     "("
  RIGHT_PARENTHESIS    ")"
  LEFT_CURLY_BRACKET   "{"
  RIGHT_CURLY_BRACKET  "}"
;

%nonassoc
  CMP_OP
  EQUAL          "=="
  NOT_EQUAL      "!="
  LESS           "<"
  GREATER        ">"
  LESS_EQUAL     "<="
  GREATER_EQUAL  ">="
;

%left
  ADD_OP
  PLUS   "+"
  MINUS  "-"
  OR     "||"
;
  
%left 
  MUL_OP
  STAR     "*"
  SLASH    "/"
  PERCENT  "%"
  AND      "&&"
;

%precedence
  UN_OP
  EXCLAMATORY  "!"
;

%%

program: func_def

func_def: IDENT "(" func_params ")" scope

func_params: func_params1
           | %empty

func_params1: IDENT
            | func_params1 "," IDENT

scope: "{" stmts "}"

stmts: stmt
     | stmts stmt

stmt: var_def
    | ret_stmt
    | if_stmt
    | while_stmt

var_def: IDENT "=" expr ";"

ret_stmt: RETURN expr ";"

if_stmt: IF "(" expr ")" scope ELSE scope

while_stmt: WHILE "(" expr ")" scope

expr: expr cmp_op expr %prec CMP_OP
    | expr add_op expr %prec ADD_OP
    | expr mul_op expr %prec MUL_OP
    | un_op expr %prec UN_OP
    | IDENT
    | NUMBER
    | "(" expr ")"

cmp_op: EQUAL | NOT_EQUAL | LESS | GREATER | GREATER_EQUAL | LESS_EQUAL

add_op: PLUS | MINUS | OR

mul_op: STAR | SLASH | PERCENT | AND

un_op: MINUS | EXCLAMATORY

%%

void frontend::Parser::error(const location_type& loc, const std::string& msg) {
  std::cerr << loc << ':' << msg << '\n';
}
