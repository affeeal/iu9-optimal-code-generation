#pragma once

#include <experimental/propagate_const>
#include <memory>

#include "visitor.h"

namespace frontend {

class INode;

class CodeGenerator final : public IVisitor {
 public:
  CodeGenerator();
  ~CodeGenerator();

  void Visit(Program& program) override;
  void Visit(AssignStmt& stmt) override;
  void Visit(IfStmt& stmt) override;
  void Visit(WhileStmt& stmt) override;
  void Visit(ReturnStmt& stmt) override;
  void Visit(BinaryExpr& expr) override;
  void Visit(UnaryExpr& expr) override;
  void Visit(VarExpr& expr) override;
  void Visit(NumberExpr& expr) override;

  void Print();

 private:
  class Impl;

  std::experimental::propagate_const<std::unique_ptr<Impl>> impl_;
};

}  // namespace frontend
