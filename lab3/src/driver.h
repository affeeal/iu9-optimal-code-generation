#pragma once

#include <iostream>
#include <ostream>

#include "node.h"
#include "scanner.h"

namespace frontend {

class Driver final {
 public:
  bool Parse(std::istream& is = std::cin, std::ostream& os = std::cout,
             const std::string* isname = nullptr);

 public:
  bool trace_scanning_ = false;
  bool trace_parsing_ = false;
  std::unique_ptr<Program> program_ = nullptr;
};

}  // namespace frontend
