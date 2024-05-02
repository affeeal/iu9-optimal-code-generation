%require "3.8.2"
%language "c++"
%skeleton "lalr1.cc"
%header

%define api.namespace {frontend} 
%define api.parser.class {Parser}

%define api.token.raw

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {

namespace frontend {

class Parser;
class Scanner;

}  // namespace frontend

}

%parse-param {frontend::Scanner& scanner}

%locations

%define parse.trace
%define parse.error detailed
%define parse.lac full

%code {

#include "driver.h"

#define yylex scanner.Get

}

%define api.token.prefix {TOK_}

%token
  AS   "="
  CM   ","
  SC   ";"
  LP   "("
  RP   ")"
  LCB  "{"
  RCB  "}"

  EQ  "=="
  NE  "!="
  LT  "<"
  LE  "<="
  GT  ">"
  GE  ">="

  NOT  "!"
  OR   "||"
  AND  "&&"

  PLUS     "+"
  MINUS    "-"
  STAR     "*"
  SLASH    "/"
  PERCENT  "%"

  IF      "if"
  ELSE    "else"
  WHILE   "while"
  RETURN  "return"
; 

%token <std::string>   IDENT   "identifier";
%token <std::int64_t>  NUMBER  "number";

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

ret_stmt: "return" expr ";"

if_stmt: "if" "(" expr ")" scope opt_else

opt_else: "else" scope
        | %empty

while_stmt: "while" "(" expr ")" scope

expr: arithm_expr
    | expr cmp_op arithm_expr

cmp_op: "==" | "!=" | "<" | ">" | "<=" | ">="

arithm_expr: term
           | arithm_expr add_op term

add_op: "+" | "-" | "||"

term: factor
    | term mul_op factor

mul_op: "*" | "/" | "%" | "&&"

factor: atom |
        un_op atom

un_op: "-" | "!"

atom: IDENT
      | NUMBER
      | "(" expr ")"

%%

void frontend::Parser::error(const location_type& loc, const std::string& msg) {
  std::cerr << loc << ':' << msg << '\n';
}
