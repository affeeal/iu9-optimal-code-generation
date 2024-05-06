#pragma once

#include <memory>
#include <string>
#include <vector>

#include "code_generator.h"

namespace frontend {

// TODO: store symbols in the table. TODO: convert ExprOp to string.

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

 public:
  virtual void Accept(IVisitor& visitor) = 0;
};

class Program final : public INode {
  std::vector<std::unique_ptr<FuncDef>> func_defs_;

 public:
  Program(std::vector<std::unique_ptr<FuncDef>>&& func_defs)
      : func_defs_(std::move(func_defs)) {}

 public:
  void Accept(IVisitor& visitor) override;
};

class FuncProto final : public INode {
  std::string name_;
  std::vector<std::string> params_;

 public:
  FuncProto(std::string&& name, std::vector<std::string>&& params)
      : name_(std::move(name)), params_(std::move(params)) {}

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }
};

class FuncDef final : public INode {
  std::unique_ptr<FuncProto> proto_;
  std::unique_ptr<Scope> body_;

 public:
  FuncDef(std::unique_ptr<FuncProto>&& proto, std::unique_ptr<Scope>&& body)
      : proto_(std::move(proto)), body_(std::move(body)) {}

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }
};

class Scope final : public INode {
  std::vector<std::unique_ptr<IStmt>> stmts_;

 public:
  Scope(std::vector<std::unique_ptr<IStmt>>&& stmts)
      : stmts_(std::move(stmts)) {}

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }
};

class IStmt : public INode {
 public:
  virtual ~IStmt() = default;
};

class AssignStmt final : public IStmt {
  std::string var_name_;
  std::unique_ptr<IExpr> expr_;

 public:
  AssignStmt(std::string&& var_name, std::unique_ptr<IExpr>&& expr)
      : var_name_(std::move(var_name)), expr_(std::move(expr)) {}

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }
};

class ReturnStmt final : public IStmt {
  std::unique_ptr<IExpr> expr_;

 public:
  ReturnStmt(std::unique_ptr<IExpr>&& expr) : expr_(std::move(expr)) {}

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }
};

class IfStmt final : public IStmt {
  std::unique_ptr<IExpr> cond_;
  std::unique_ptr<Scope> then_, otherwise_;

 public:
  IfStmt(std::unique_ptr<IExpr>&& cond, std::unique_ptr<Scope>&& then,
         std::unique_ptr<Scope>&& otherwise)
      : cond_(std::move(cond)),
        then_(std::move(then)),
        otherwise_(std::move(otherwise)) {}

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }
};

class WhileStmt final : public IStmt {
  std::unique_ptr<IExpr> cond_;
  std::unique_ptr<Scope> body_;

 public:
  WhileStmt(std::unique_ptr<IExpr>&& cond, std::unique_ptr<Scope>&& body)
      : cond_(std::move(cond)), body_(std::move(body)) {}

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }
};

class IExpr : public INode {
 public:
  virtual ~IExpr() = default;
};

class BinaryExpr final : public IExpr {
  std::unique_ptr<IExpr> lhs_, rhs_;
  ExprOp op_;

 public:
  // TODO: check lhs, rhs for non-null.
  BinaryExpr(std::unique_ptr<IExpr>&& lhs, std::unique_ptr<IExpr>&& rhs,
             const ExprOp op)
      : lhs_(std::move(lhs)), rhs_(std::move(rhs)), op_(op) {}

  const IExpr& get_lhs() const noexcept { return *lhs_; }
  IExpr& get_lhs() noexcept { return *lhs_; }

  const IExpr& get_rhs() const noexcept { return *rhs_; }
  IExpr& get_rhs() noexcept { return *rhs_; }

  ExprOp get_op() const noexcept { return op_; }

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }
};

class UnaryExpr final : public IExpr {
  std::unique_ptr<IExpr> subject_;
  ExprOp op_;

 public:
  // TODO: check subject for non-null.
  UnaryExpr(std::unique_ptr<IExpr>&& subject, const ExprOp op)
      : subject_(std::move(subject)), op_(op) {}

  const IExpr& get_subject() const noexcept { return *subject_; }
  IExpr& get_subject() noexcept { return *subject_; }

  ExprOp get_op() const noexcept { return op_; }

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }
};

class VarExpr final : public IExpr {
  std::string name_;

 public:
  VarExpr(std::string&& name) : name_(std::move(name)) {}

  const std::string& get_name() const noexcept { return name_; }

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }
};

class NumberExpr final : public IExpr {
  std::int64_t value_;

 public:
  NumberExpr(const std::int64_t value) : value_(value) {}

  std::int64_t get_value() const noexcept { return value_; }

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }
};

}  // namespace frontend
