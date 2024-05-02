#pragma once

#include <iostream>
#include <ostream>

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL frontend::Parser::symbol_type frontend::Scanner::Get()

#include "location.hh"
#include "parser.tab.hh"

namespace frontend {

class Scanner final : public yyFlexLexer {
 public:
  Scanner(std::istream& is = std::cin, std::ostream& os = std::cout,
          const std::string* isname = nullptr);

  Parser::symbol_type Get();

 private:
  Parser::symbol_type MakeNumber(const std::string& str,
                                 const Parser::location_type& loc);

 private:
  location loc_;
};

}  // namespace frontend
