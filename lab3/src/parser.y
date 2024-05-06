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

%nterm <std::vector<std::unique_ptr<frontend::FuncDef>>> func_defs
%nterm <std::unique_ptr<frontend::FuncDef>> func_def
%nterm <std::unique_ptr<frontend::FuncProto>> func_proto
%nterm <std::vector<std::string>> func_params_opt
%nterm <std::vector<std::string>> func_params
%nterm <std::unique_ptr<frontend::Scope>> scope
%nterm <std::vector<std::unique_ptr<frontend::IStmt>>> stmts
%nterm <std::unique_ptr<frontend::IStmt>> stmt
%nterm <std::unique_ptr<frontend::AssignStmt>> assign_stmt
%nterm <std::unique_ptr<frontend::ReturnStmt>> return_stmt
%nterm <std::unique_ptr<frontend::IfStmt>> if_stmt
%nterm <std::unique_ptr<frontend::WhileStmt>> while_stmt
%nterm <std::unique_ptr<frontend::IExpr>> expr
%nterm <frontend::ExprOp> cmp_op
%nterm <frontend::ExprOp> add_op
%nterm <frontend::ExprOp> mul_op
%nterm <frontend::ExprOp> un_op

%%

program:
  func_defs
  {
    driver.program_ = std::make_unique<frontend::Program>($1);
  }


func_defs:
  func_def
  {
    $$.push_back($1);
  }
| func_defs func_def
  {
    $$ = $1;
    $$.push_back($2);
  }


func_def:
  func_proto scope
  {
    $$ = std::make_unique<frontend::FuncDef>($1, $2);
  }


func_proto:
  IDENT "(" func_params_opt ")"
  {
    $$ = std::make_unique<frontend::FuncProto>($1, $3);
  }


func_params_opt:
  func_params
| %empty { }


func_params:
  IDENT
  {
    $$.push_back($1);
  }
| func_params "," IDENT
  {
    $$ = $1;
    $$.push_back($3);
  }


scope:
  "{" stmts "}"
  {
    $$ = std::make_unique<frontend::Scope>($2);
  }


stmts:
  stmt
  {
    $$.push_back($1);
  }
| stmts stmt
  {
    $$ = $1;
    $$.push_back($2);
  }


stmt:
  assign_stmt  { $$ = $1; }
| return_stmt  { $$ = $1; }
| if_stmt      { $$ = $1; }
| while_stmt   { $$ = $1; }


assign_stmt:
  IDENT "=" expr ";"
  {
    $$ = std::make_unique<frontend::AssignStmt>($1, $3);
  }


return_stmt:
  RETURN expr ";"
  {
    $$ = std::make_unique<frontend::ReturnStmt>($2);
  }


if_stmt:
  IF "(" expr ")" scope ELSE scope
  {
    $$ = std::make_unique<frontend::IfStmt>($3, $5, $7);
  }


while_stmt:
  WHILE "(" expr ")" scope
  {
    $$ = std::make_unique<frontend::WhileStmt>($3, $5);
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
  EQUAL          { $$ = frontend::ExprOp::kEq; }
| NOT_EQUAL      { $$ = frontend::ExprOp::kNe; }
| LESS           { $$ = frontend::ExprOp::kLt; }
| GREATER        { $$ = frontend::ExprOp::kGt; }
| GREATER_EQUAL  { $$ = frontend::ExprOp::kGe; }
| LESS_EQUAL     { $$ = frontend::ExprOp::kLe; }


add_op:
  PLUS   { $$ = frontend::ExprOp::kAdd; }
| MINUS  { $$ = frontend::ExprOp::kSub; }
| OR     { $$ = frontend::ExprOp::kOr; }


mul_op:
  STAR     { $$ = frontend::ExprOp::kMul; }
| SLASH    { $$ = frontend::ExprOp::kDiv; }
| PERCENT  { $$ = frontend::ExprOp::kMod; }
| AND      { $$ = frontend::ExprOp::kAnd; }


un_op:
  MINUS        { $$ = frontend::ExprOp::kNeg; }
| EXCLAMATORY  { $$ = frontend::ExprOp::kNot; }

%%

void frontend::Parser::error(const location_type& loc, const std::string& msg) {
  // TODO: handle the error.
  std::cerr << loc << ':' << msg << '\n';
}
