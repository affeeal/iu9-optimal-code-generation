%require "3.8.2"
%language "c++"
%skeleton "lalr1.cc"

%header
%locations

%define api.location.file "location.h"
%define api.namespace {frontend} 
%define api.parser.class {Parser}
%define api.token.prefix {TOK_}
%define api.token.raw
%define api.token.constructor
%define api.value.automove
%define api.value.type variant

%define parse.assert
%define parse.error detailed
%define parse.trace
%define parse.lac full

%parse-param {frontend::Scanner& scanner}
%parse-param {frontend::Driver& driver}

%code requires {

#include "node.h"

namespace frontend {

class Driver;
class Scanner;

}  // namespace frontend

}

%code top {

#include <sstream>
#include <memory>

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

%nonassoc
  CMP_OP
  EQUAL          "=="
  NOT_EQUAL      "!="
  LESS           "<"
  GREATER        ">"
  LESS_EQUAL     "<="
  GREATER_EQUAL  ">="

%left
  ADD_OP
  PLUS   "+"
  MINUS  "-"
  OR     "||"
  
%left 
  MUL_OP
  STAR     "*"
  SLASH    "/"
  PERCENT  "%"
  AND      "&&"

%precedence
  UN_OP
  EXCLAMATORY  "!"

%nterm <std::vector<std::unique_ptr<frontend::IStmt>>> stmts
%nterm <std::unique_ptr<frontend::IStmt>> stmt
%nterm <std::unique_ptr<frontend::AssignStmt>> assign_stmt
%nterm <std::unique_ptr<frontend::IfStmt>> if_stmt
%nterm <std::unique_ptr<frontend::WhileStmt>> while_stmt
%nterm <std::unique_ptr<frontend::ReturnStmt>> return_stmt
%nterm <std::unique_ptr<frontend::IExpr>> expr
%nterm <frontend::BinaryExpr::Op> cmp_op
%nterm <frontend::BinaryExpr::Op> add_op
%nterm <frontend::BinaryExpr::Op> mul_op
%nterm <frontend::UnaryExpr::Op> un_op

%%

program:
  stmts
  {
    driver.set_program(std::make_unique<frontend::Program>($1));
  }

stmts:
  stmts stmt
  {
    $$ = $1;
    $$.push_back($2);
  }
| %empty { }

stmt:
  assign_stmt
  {
    $$ = $1;
  }
| if_stmt
  {
    $$ = $1;
  }
| while_stmt
  {
    $$ = $1;
  }
| return_stmt
  {
    $$ = $1;
  }

assign_stmt:
  IDENT "=" expr ";"
  {
    $$ = std::make_unique<frontend::AssignStmt>($1, $3);
  }

if_stmt:
  IF "(" expr ")" "{" stmts "}" ELSE "{" stmts "}"
  {
    $$ = std::make_unique<frontend::IfStmt>($3, $6, $10);
  }

while_stmt:
  WHILE "(" expr ")" "{" stmts "}"
  {
    $$ = std::make_unique<frontend::WhileStmt>($3, $6);
  }

return_stmt:
  RETURN expr ";"
  {
    $$ = std::make_unique<frontend::ReturnStmt>($2);
  }

expr:
  expr cmp_op expr %prec CMP_OP
  {
    $$ = std::make_unique<frontend::BinaryExpr>($1, $3, $2);
  }
| expr add_op expr %prec ADD_OP
  {
    $$ = std::make_unique<frontend::BinaryExpr>($1, $3, $2);
  }
| expr mul_op expr %prec MUL_OP
  {
    $$ = std::make_unique<frontend::BinaryExpr>($1, $3, $2);
  }
| un_op expr %prec UN_OP
  {
    $$ = std::make_unique<frontend::UnaryExpr>($2, $1);
  }
| IDENT
  {
    $$ = std::make_unique<frontend::VarExpr>($1);
  }
| NUMBER
  {
    $$ = std::make_unique<frontend::NumberExpr>($1);
  }
| "(" expr ")"
  {
    $$ = $2;
  }

cmp_op:
  EQUAL
  {
    $$ = frontend::BinaryExpr::Op::kEq;
  }
| NOT_EQUAL
  {
    $$ = frontend::BinaryExpr::Op::kNe;
  }
| LESS
  {
    $$ = frontend::BinaryExpr::Op::kLt;
  }
| GREATER
  {
    $$ = frontend::BinaryExpr::Op::kGt;
  }
| GREATER_EQUAL
  {
    $$ = frontend::BinaryExpr::Op::kGe;
  }
| LESS_EQUAL
  {
    $$ = frontend::BinaryExpr::Op::kLe;
  }

add_op:
  PLUS
  {
    $$ = frontend::BinaryExpr::Op::kAdd;
  }
| MINUS
  {
    $$ = frontend::BinaryExpr::Op::kSub;
  }
| OR
  {
    $$ = frontend::BinaryExpr::Op::kOr;
  }

mul_op:
  STAR
  {
    $$ = frontend::BinaryExpr::Op::kMul;
  }
| SLASH
  {
    $$ = frontend::BinaryExpr::Op::kDiv;
  }
| PERCENT
  {
    $$ = frontend::BinaryExpr::Op::kMod;
  }
| AND
  {
    $$ = frontend::BinaryExpr::Op::kAnd;
  }

un_op:
  MINUS
  {
    $$ = frontend::UnaryExpr::Op::kNeg;
  }
| EXCLAMATORY
  {
    $$ = frontend::UnaryExpr::Op::kNot;
  }

%%

void frontend::Parser::error(const location_type& loc, const std::string& msg) {
  throw syntax_error(loc, msg);
}
