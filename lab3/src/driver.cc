#include "driver.h"

namespace frontend {

bool Driver::Parse(std::istream& is, std::ostream& os,
                   const std::string* isname) {
  auto scanner = Scanner{is, os, isname};
  scanner.set_debug(trace_scanning_);

  auto parser = Parser{scanner, *this};
  parser.set_debug_level(trace_parsing_);

  return parser.parse();
}

}  // namespace frontend
