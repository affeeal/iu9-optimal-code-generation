%{
#include "scanner.h"

#define yyterminate() return Parser::make_YYEOF(loc_)

#define YY_USER_ACTION loc_.columns(yyleng);

using frontend::Parser;
%}

%option c++
%option yyclass="frontend::Scanner"
%option noyywrap nounput noinput
%option batch
%option debug

BLANK   [ \t\r]
IDENT   [A-Za-z_][A-Za-z_0-9]*
NUMBER  [0-9]+

%%

%{
  loc_.step();
%}

{BLANK}+  { loc_.step(); }

\n+       { loc_.lines(yyleng); loc_.step(); }

"="       { return Parser::make_AS(loc_); } 
","       { return Parser::make_CM(loc_); }
";"       { return Parser::make_SC(loc_); }
"("       { return Parser::make_LP(loc_); }
")"       { return Parser::make_RP(loc_); }
"{"       { return Parser::make_LCB(loc_); }
"}"       { return Parser::make_RCB(loc_); }

"=="      { return Parser::make_EQ(loc_); }
"!="      { return Parser::make_NE(loc_); }
"<"       { return Parser::make_LT(loc_); }
"<="      { return Parser::make_LE(loc_); }
">"       { return Parser::make_GT(loc_); }
">="      { return Parser::make_GE(loc_); }

"||"      { return Parser::make_OR(loc_); }
"&&"      { return Parser::make_AND(loc_); }
"!"       { return Parser::make_NOT(loc_); }

"+"       { return Parser::make_PLUS(loc_); }
"-"       { return Parser::make_MINUS(loc_); }
"*"       { return Parser::make_STAR(loc_); }
"/"       { return Parser::make_SLASH(loc_); }
"%"       { return Parser::make_PERCENT(loc_); }

"if"      { return Parser::make_IF(loc_); }
"else"    { return Parser::make_ELSE(loc_); }
"while"   { return Parser::make_WHILE(loc_); }
"return"  { return Parser::make_RETURN(loc_); }

{IDENT}   { return Parser::make_IDENT(yytext, loc_); }

{NUMBER}  { return MakeNumber(yytext, loc_); }

.         { throw Parser::syntax_error(
                loc_, "unexpected character: " + std::string(yytext)); }

%%

namespace frontend {

Scanner::Scanner(std::istream& is, std::ostream& os, const std::string* isname)
    : yyFlexLexer(is, os), loc_(isname) {}

Parser::symbol_type Scanner::MakeNumber(const std::string& s,
                                        const Parser::location_type& loc) {
  try {
    return Parser::make_NUMBER(std::stoll(s), loc);
  } catch (const std::logic_error& e) {
    throw Parser::syntax_error(loc, e.what());
  }
}

}  // namespace frontend
