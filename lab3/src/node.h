#pragma once

#include <memory>
#include <string>
#include <vector>

#include "code_generator.h"

namespace frontend {

class IStmt;
class IExpr;

class INode {
 public:
  virtual ~INode() = default;

 public:
  virtual void Accept(IVisitor& visitor) = 0;
};

class Program final : public INode {
  std::vector<std::unique_ptr<IStmt>> stmts_;

 public:
  Program(std::vector<std::unique_ptr<IStmt>>&& stmts)
      : stmts_(std::move(stmts)) {}

  auto get_stmts_cbegin() const noexcept { return stmts_.cbegin(); }
  auto get_stmts_begin() noexcept { return stmts_.begin(); }
  auto get_stmts_cend() const noexcept { return stmts_.cend(); }
  auto get_stmts_end() noexcept { return stmts_.end(); }

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }
};

class IStmt : public INode {
 public:
  virtual ~IStmt() = default;
};

class AssignStmt final : public IStmt {
  std::string name_;
  std::unique_ptr<IExpr> expr_;

 public:
  AssignStmt(std::string&& name, std::unique_ptr<IExpr>&& expr)
      : name_(std::move(name)), expr_(std::move(expr)) {}

  const std::string& get_name() const noexcept { return name_; }

  const IExpr& get_expr() const noexcept { return *expr_; }
  IExpr& get_expr() noexcept { return *expr_; }

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }
};

class IfStmt final : public IStmt {
  std::unique_ptr<IExpr> cond_;
  std::vector<std::unique_ptr<IStmt>> then_stmts_, else_stmts_;

 public:
  IfStmt(std::unique_ptr<IExpr>&& cond,
         std::vector<std::unique_ptr<IStmt>>&& then_stmts,
         std::vector<std::unique_ptr<IStmt>>&& else_stmts)
      : cond_(std::move(cond)),
        then_stmts_(std::move(then_stmts)),
        else_stmts_(std::move(else_stmts)) {}

  const IExpr& get_cond() const noexcept { return *cond_; }
  IExpr& get_cond() noexcept { return *cond_; }

  auto get_then_cbegin() const noexcept { return then_stmts_.cbegin(); }
  auto get_then_begin() noexcept { return then_stmts_.begin(); }
  auto get_then_cend() const noexcept { return then_stmts_.cend(); }
  auto get_then_end() noexcept { return then_stmts_.end(); }

  auto get_else_cbegin() const noexcept { return else_stmts_.cbegin(); }
  auto get_else_begin() noexcept { return else_stmts_.begin(); }
  auto get_else_cend() const noexcept { return else_stmts_.cend(); }
  auto get_else_end() noexcept { return else_stmts_.end(); }

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }
};

class WhileStmt final : public IStmt {
  std::unique_ptr<IExpr> cond_;
  std::vector<std::unique_ptr<IStmt>> stmts_;

 public:
  WhileStmt(std::unique_ptr<IExpr>&& cond,
            std::vector<std::unique_ptr<IStmt>>&& stmts)
      : cond_(std::move(cond)), stmts_(std::move(stmts)) {}

  const IExpr& get_cond() const noexcept { return *cond_; }
  IExpr& get_cond() noexcept { return *cond_; }

  auto get_stmts_cbegin() const noexcept { return stmts_.cbegin(); }
  auto get_stmts_begin() noexcept { return stmts_.begin(); }
  auto get_stmts_cend() const noexcept { return stmts_.cend(); }
  auto get_stmts_end() noexcept { return stmts_.end(); }

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }
};

class ReturnStmt final : public IStmt {
  std::unique_ptr<IExpr> expr_;

 public:
  ReturnStmt(std::unique_ptr<IExpr>&& expr) : expr_(std::move(expr)) {}

  const IExpr& get_expr() const noexcept { return *expr_; }
  IExpr& get_expr() noexcept { return *expr_; }

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }
};

class IExpr : public INode {
 public:
  virtual ~IExpr() = default;
};

class BinaryExpr final : public IExpr {
 public:
  enum class Op {
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
  };

  BinaryExpr(std::unique_ptr<IExpr>&& lhs, std::unique_ptr<IExpr>&& rhs,
             const Op op)
      : lhs_(std::move(lhs)), rhs_(std::move(rhs)), op_(op) {}

  const IExpr& get_lhs() const noexcept { return *lhs_; }
  IExpr& get_lhs() noexcept { return *lhs_; }

  const IExpr& get_rhs() const noexcept { return *rhs_; }
  IExpr& get_rhs() noexcept { return *rhs_; }

  Op get_op() const noexcept { return op_; }

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }

 private:
  std::unique_ptr<IExpr> lhs_, rhs_;
  Op op_;
};

class UnaryExpr final : public IExpr {
 public:
  enum class Op {
    kNeg,
    kNot,
  };

 public:
  UnaryExpr(std::unique_ptr<IExpr>&& expr, const Op op)
      : expr_(std::move(expr)), op_(op) {}

  const IExpr& get_expr() const noexcept { return *expr_; }
  IExpr& get_expr() noexcept { return *expr_; }

  Op get_op() const noexcept { return op_; }

 public:
  void Accept(IVisitor& visitor) override { visitor.Visit(*this); }

 private:
  std::unique_ptr<IExpr> expr_;
  Op op_;
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
