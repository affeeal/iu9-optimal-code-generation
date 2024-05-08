#pragma once

// clang-format off
// TODO: PIMPL or move the include to .cc
#include "llvm/IR/IRBuilder.h"
// clang-format on

#include "visitor.h"

namespace frontend {

class INode;

class CodeGenerator final : public IVisitor {
 public:
  CodeGenerator();

  void Visit(Program& program) override;
  void Visit(AssignStmt& stmt) override;
  void Visit(IfStmt& stmt) override;
  void Visit(WhileStmt& stmt) override;
  void Visit(ReturnStmt& stmt) override;
  void Visit(BinaryExpr& expr) override;
  void Visit(UnaryExpr& expr) override;
  void Visit(VarExpr& expr) override;
  void Visit(NumberExpr& expr) override;

  void Dump() const;

 private:
  llvm::Value* AcceptAndReturn(INode& node);
  llvm::AllocaInst* CreateEntryBlockAlloca(const std::string& name);

 private:
  class Scope final {
   public:
    Scope(const Scope* const parent) noexcept : parent_(parent) {}

    void Add(const std::string& name, llvm::AllocaInst* const alloc);
    llvm::AllocaInst* Visible(const std::string& name) const;
    llvm::AllocaInst* Find(const std::string& name) const;

   private:
    const Scope* parent_;
    std::unordered_map<std::string, llvm::AllocaInst*> named_allocs_;
  } * scope_;

  std::unique_ptr<llvm::LLVMContext> context_;
  std::unique_ptr<llvm::Module> module_;
  std::unique_ptr<llvm::IRBuilder<>> builder_;

  llvm::Function* main_;
  llvm::Value* return_;
};

}  // namespace frontend
