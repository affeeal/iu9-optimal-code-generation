#include "driver.h"

#include <fstream>

namespace frontend {

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
  auto parser = Parser{scanner, *this};
  parser.parse();
}

}  // namespace frontend
