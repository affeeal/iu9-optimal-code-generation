#include "codegen.h"

namespace frontend {

CGContext::CGContext()
    : context_(std::make_unique<llvm::LLVMContext>()),
      module_(std::make_unique<llvm::Module>("a module", *context_)),
      builder_(std::make_unique<llvm::IRBuilder<>>(*context_)) {}

const llvm::LLVMContext& CGContext::get_context() const noexcept {
  return *context_;
}

llvm::LLVMContext& CGContext::get_context() noexcept { return *context_; }

std::optional<llvm::Value*> CGContext::FindNamedValue(
    const std::string& name) const {
  if (const auto it = named_values_.find(name); it != named_values_.cend()) {
    return it->second;
  }

  return std::nullopt;
}

void CGContext::AddNamedValue(const std::string& name,
                              llvm::Value* const value) {
  named_values_[name] = value;
}

const llvm::IRBuilder<>& CGContext::get_builder() const noexcept {
  return *builder_;
}

llvm::IRBuilder<>& CGContext::get_builder() noexcept { return *builder_; }

const llvm::Module& CGContext::get_module() const noexcept { return *module_; }

llvm::Module& CGContext::get_module() noexcept { return *module_; }

}  // namespace frontend
