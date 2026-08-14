#include <memory>

// clang-format off
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"
// clang-format on

int main() {
  const auto context = std::make_unique<llvm::LLVMContext>();
  const auto module = std::make_unique<llvm::Module>("a module", *context);
  const auto builder = std::make_unique<llvm::IRBuilder<>>(*context);

  auto* const func_type =
      llvm::FunctionType::get(llvm::Type::getInt32Ty(*context), false);
  auto* const func = llvm::Function::Create(
      func_type, llvm::Function::ExternalLinkage, "main", module.get());

  auto* const bb = llvm::BasicBlock::Create(*context, "entry", func);
  builder->SetInsertPoint(bb);

  auto* const a = llvm::ConstantInt::get(*context, llvm::APInt(32, 353, true));
  auto* const b = llvm::ConstantInt::get(*context, llvm::APInt(32, 48, true));
  auto* const add = builder->CreateAdd(a, b, "addtmp");

  builder->CreateRet(add);

  if (llvm::verifyFunction(*func, &llvm::errs())) {
    return 1;
  }

  module->print(llvm::outs(), nullptr);
  return 0;
}
