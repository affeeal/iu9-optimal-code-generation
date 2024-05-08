#pragma once

#include "node.h"
#include "scanner.h"

namespace frontend {

class Driver final {
  std::unique_ptr<Program> program_ = nullptr;

 public:
  void Parse(const std::string& filename);

  void set_program(std::unique_ptr<Program>&& program) noexcept;
  Program* get_program() noexcept;
  const Program* get_program() const noexcept;
};

}  // namespace frontend
