#pragma once

#include "node.h"
#include "scanner.h"

namespace frontend {

class Driver final {
  bool trace_scanning_, trace_parsing_;
  std::unique_ptr<Program> program_;

 public:
  void Parse(const std::string& filename);

  void set_trace_scanning(const bool is_active) noexcept;
  void set_trace_parsing(const bool is_active) noexcept;

  void set_program(std::unique_ptr<Program>&& program) noexcept;
  Program* get_program() noexcept;
  const Program* get_program() const noexcept;
};

}  // namespace frontend
