#pragma once

#include <memory>
#include <string>
#include <vector>

#include "codegen.h"

namespace frontend {

// TODO: store symbols in the table.
// TODO: ExprOp to string.

class FuncDef;
class Scope;
class IStmt;
class IExpr;

enum class ExprOp {
  kEq,
  kNe,
  kLt,
  kGt,
  kLe,
  kGe,
  kAdd,
  kSub,
  kOr,
  kMul,
  kDiv,
  kMod,
  kAnd,
  kNeg,
  kNot,
};

class INode {
 public:
  virtual ~INode() = default;
  virtual llvm::Value* Codegen(CGContext& context) const = 0;

 protected:
  INode() = default;
  INode(const INode&) = delete;
  INode& operator=(const INode&) = delete;
};

class Program final : public INode {
  std::vector<std::unique_ptr<FuncDef>> func_defs_;

 public:
  Program(std::vector<std::unique_ptr<FuncDef>>&& func_defs);
  llvm::Value* Codegen(CGContext& context) const override;
};

class FuncProto final : public INode {
  std::string name_;
  std::vector<std::string> params_;

 public:
  FuncProto(std::string&& name, std::vector<std::string>&& params);
  llvm::Value* Codegen(CGContext& context) const override;
};

class FuncDef final : public INode {
  std::unique_ptr<FuncProto> proto_;
  std::unique_ptr<Scope> body_;

 public:
  FuncDef(std::unique_ptr<FuncProto>&& proto, std::unique_ptr<Scope>&& body);
  llvm::Value* Codegen(CGContext& context) const override;
};

class Scope final : public INode {
  std::vector<std::unique_ptr<IStmt>> stmts_;

 public:
  Scope(std::vector<std::unique_ptr<IStmt>>&& stmts);
  llvm::Value* Codegen(CGContext& context) const override;
};

class IStmt : public INode {
 public:
  virtual ~IStmt() = default;
};

class AssignStmt final : public IStmt {
  std::string var_name_;
  std::unique_ptr<IExpr> expr_;

 public:
  AssignStmt(std::string&& var_name, std::unique_ptr<IExpr>&& expr);
  llvm::Value* Codegen(CGContext& context) const override;
};

class ReturnStmt final : public IStmt {
  std::unique_ptr<IExpr> expr_;

 public:
  ReturnStmt(std::unique_ptr<IExpr>&& expr);
  llvm::Value* Codegen(CGContext& context) const override;
};

class IfStmt final : public IStmt {
  std::unique_ptr<IExpr> cond_;
  std::unique_ptr<Scope> then_, otherwise_;

 public:
  IfStmt(std::unique_ptr<IExpr>&& cond, std::unique_ptr<Scope>&& then,
         std::unique_ptr<Scope>&& otherwise);
  llvm::Value* Codegen(CGContext& context) const override;
};

class WhileStmt final : public IStmt {
  std::unique_ptr<IExpr> cond_;
  std::unique_ptr<Scope> body_;

 public:
  WhileStmt(std::unique_ptr<IExpr>&& cond, std::unique_ptr<Scope>&& body);
  llvm::Value* Codegen(CGContext& context) const override;
};

class IExpr : public INode {
 public:
  virtual ~IExpr() = default;
};

class BinaryExpr final : public IExpr {
  std::unique_ptr<IExpr> lhs_, rhs_;
  ExprOp op_;

 public:
  BinaryExpr(std::unique_ptr<IExpr>&& lhs, std::unique_ptr<IExpr>&& rhs,
             const ExprOp op);
  llvm::Value* Codegen(CGContext& context) const override;
};

class UnaryExpr final : public IExpr {
  std::unique_ptr<IExpr> expr_;
  ExprOp op_;

 public:
  UnaryExpr(std::unique_ptr<IExpr>&& expr, const ExprOp op);
  llvm::Value* Codegen(CGContext& context) const override;
};

class VarExpr final : public IExpr {
  std::string name_;

 public:
  VarExpr(std::string&& name);
  llvm::Value* Codegen(CGContext& context) const override;
};

class NumberExpr final : public IExpr {
  std::int64_t value_;

 public:
  NumberExpr(const std::int64_t value);
  llvm::Value* Codegen(CGContext& context) const override;
};

}  // namespace frontend
