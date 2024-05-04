#pragma once

#include <memory>
#include <string>
#include <vector>

namespace frontend {

// TODO: store symbols in the table.

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

class Program final {
  std::vector<std::unique_ptr<FuncDef>> func_defs_;

 public:
  Program(std::vector<std::unique_ptr<FuncDef>>&& func_defs)
      : func_defs_(std::move(func_defs)) {}

  Program(const Program&) = delete;
  Program& operator=(const Program&) = delete;
};

class FuncDef final {
  std::string name_;
  std::vector<std::string> params_;
  std::unique_ptr<Scope> body_;

 public:
  FuncDef(std::string&& name, std::vector<std::string>&& params,
          std::unique_ptr<Scope>&& body)
      : name_(std::move(name)),
        params_(std::move(params)),
        body_(std::move(body)) {}

  FuncDef(const FuncDef&) = delete;
  FuncDef& operator==(const FuncDef&) = delete;
};

class Scope final {
  std::vector<std::unique_ptr<IStmt>> stmts_;

 public:
  Scope(std::vector<std::unique_ptr<IStmt>>&& stmts)
      : stmts_(std::move(stmts)) {}

  Scope(const Scope&) = delete;
  Scope& operator==(const Scope&) = delete;
};

// TODO: delete copy ctor and assignment.

class IStmt {
 public:
  virtual ~IStmt() = default;
};

class AssignStmt final : public IStmt {
  std::string var_name_;
  std::unique_ptr<IExpr> expr_;

 public:
  AssignStmt(std::string&& var_name, std::unique_ptr<IExpr>&& expr)
      : var_name_(std::move(var_name)), expr_(std::move(expr)) {}
};

class ReturnStmt final : public IStmt {
  std::unique_ptr<IExpr> expr_;

 public:
  ReturnStmt(std::unique_ptr<IExpr>&& expr) : expr_(std::move(expr)) {}
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
};

class WhileStmt final : public IStmt {
  std::unique_ptr<IExpr> cond_;
  std::unique_ptr<Scope> body_;

 public:
  WhileStmt(std::unique_ptr<IExpr>&& cond, std::unique_ptr<Scope>&& body)
      : cond_(std::move(cond)), body_(std::move(body)) {}
};

class IExpr {
 public:
  virtual ~IExpr() = default;
};

class BinaryExpr final : public IExpr {
  std::unique_ptr<IExpr> lhs_, rhs_;
  ExprOp op_;

 public:
  BinaryExpr(std::unique_ptr<IExpr>&& lhs, std::unique_ptr<IExpr>&& rhs,
             const ExprOp op)
      : lhs_(std::move(lhs)), rhs_(std::move(rhs)), op_(op) {}
};

class UnaryExpr final : public IExpr {
  std::unique_ptr<IExpr> expr_;
  ExprOp op_;

 public:
  UnaryExpr(std::unique_ptr<IExpr>&& expr, const ExprOp op)
      : expr_(std::move(expr)), op_(op) {}
};

class VarExpr final : public IExpr {
  std::string name_;

 public:
  VarExpr(std::string&& name) : name_(std::move(name)) {}
};

class NumberExpr final : public IExpr {
  std::int64_t value_;

 public:
  NumberExpr(const std::int64_t value) : value_(value) {}
};

}  // namespace frontend
