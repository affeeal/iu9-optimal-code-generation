#pragma once

// clang-format off
#include "llvm/IR/IRBuilder.h"
// clang-format on

#include "visitor.h"

namespace frontend {

class CodeGenerator final : public IVisitor {
  llvm::Value* return_;

  std::unique_ptr<llvm::LLVMContext> context_;
  std::unique_ptr<llvm::Module> module_;
  std::unique_ptr<llvm::IRBuilder<>> builder_;
  std::unordered_map<std::string, llvm::AllocaInst*> named_allocas_;

 public:
  CodeGenerator();

 public:
  void Visit(Program& program) override;
  void Visit(FuncProto& proto) override;
  void Visit(FuncDef& def) override;
  void Visit(Scope& scope) override;
  void Visit(AssignStmt& stmt) override;
  void Visit(ReturnStmt& stmt) override;
  void Visit(IfStmt& stmt) override;
  void Visit(WhileStmt& stmt) override;
  void Visit(BinaryExpr& expr) override;
  void Visit(UnaryExpr& expr) override;
  void Visit(VarExpr& expr) override;
  void Visit(NumberExpr& expr) override;

 private:
  llvm::Value* AcceptAndReturn(INode& node);
  llvm::AllocaInst* CreateEntryBlockAlloca(llvm::Function& function,
                                           const std::string& name);
};

}  // namespace frontend
