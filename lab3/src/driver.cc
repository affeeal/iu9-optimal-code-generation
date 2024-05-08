#include "driver.h"

#include <fstream>

namespace frontend {

void Driver::set_trace_scanning(const bool is_active) noexcept {
  trace_scanning_ = is_active;
}

void Driver::set_trace_parsing(const bool is_active) noexcept {
  trace_parsing_ = is_active;
}

void Driver::set_program(std::unique_ptr<Program>&& program) noexcept {
  program_ = std::move(program);
}

Program* Driver::get_program() noexcept { return program_.get(); }
const Program* Driver::get_program() const noexcept { return program_.get(); }

void Driver::Parse(const std::string& filename) {
  auto file = std::ifstream{filename};
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file " + filename);
  }

  auto scanner = Scanner{file, std::cout, &filename};
  scanner.set_debug(trace_scanning_);

  auto parser = Parser{scanner, *this};
  parser.set_debug_level(trace_parsing_);

  parser.parse();
}

}  // namespace frontend
