#pragma once

#include <optional>

// clang-format off
#include "llvm/IR/IRBuilder.h"
// clang-format on

namespace frontend {

class CGContext final {
  std::unique_ptr<llvm::LLVMContext> context_;
  std::unique_ptr<llvm::Module> module_;
  std::unique_ptr<llvm::IRBuilder<>> builder_;
  std::unordered_map<std::string, llvm::Value*> named_values_;

 public:
  CGContext();
  CGContext(const CGContext&) = delete;
  CGContext& operator==(const CGContext&) = delete;

  const llvm::LLVMContext& get_context() const noexcept;
  llvm::LLVMContext& get_context() noexcept;

  const llvm::IRBuilder<>& get_builder() const noexcept;
  llvm::IRBuilder<>& get_builder() noexcept;

  const llvm::Module& get_module() const noexcept;
  llvm::Module& get_module() noexcept;

  std::optional<llvm::Value*> FindNamedValue(const std::string& name) const;
  void AddNamedValue(const std::string& name, llvm::Value* const value);
};

}  // namespace frontend
