#include "node.h"

#include <stdexcept>

#include "codegen.h"

namespace frontend {

Program::Program(std::vector<std::unique_ptr<FuncDef>>&& func_defs)
    : func_defs_(std::move(func_defs)) {}

llvm::Value* Program::Codegen(CGContext& context) const {
  return nullptr;  // TODO
}

FuncProto::FuncProto(std::string&& name, std::vector<std::string>&& params)
    : name_(std::move(name)), params_(std::move(params)) {}

llvm::Value* FuncProto::Codegen(CGContext& context) const {
  return nullptr;  // TODO
}

FuncDef::FuncDef(std::unique_ptr<FuncProto>&& proto,
                 std::unique_ptr<Scope>&& body)
    : proto_(std::move(proto)), body_(std::move(body)) {}

llvm::Value* FuncDef::Codegen(CGContext& context) const {
  return nullptr;  // TODO
}

Scope::Scope(std::vector<std::unique_ptr<IStmt>>&& stmts)
    : stmts_(std::move(stmts)) {}

llvm::Value* Scope::Codegen(CGContext& context) const {
  return nullptr;  // TODO
}

AssignStmt::AssignStmt(std::string&& var_name, std::unique_ptr<IExpr>&& expr)
    : var_name_(std::move(var_name)), expr_(std::move(expr)) {}

llvm::Value* AssignStmt::Codegen(CGContext& context) const {
  return nullptr;  // TODO
}

ReturnStmt::ReturnStmt(std::unique_ptr<IExpr>&& expr)
    : expr_(std::move(expr)) {}

llvm::Value* ReturnStmt::Codegen(CGContext& context) const {
  return nullptr;  // TODO
}

IfStmt::IfStmt(std::unique_ptr<IExpr>&& cond, std::unique_ptr<Scope>&& then,
               std::unique_ptr<Scope>&& otherwise)
    : cond_(std::move(cond)),
      then_(std::move(then)),
      otherwise_(std::move(otherwise)) {}

llvm::Value* IfStmt::Codegen(CGContext& context) const {
  return nullptr;  // TODO
}

WhileStmt::WhileStmt(std::unique_ptr<IExpr>&& cond,
                     std::unique_ptr<Scope>&& body)
    : cond_(std::move(cond)), body_(std::move(body)) {}

llvm::Value* WhileStmt::Codegen(CGContext& context) const {
  return nullptr;  // TODO
}

BinaryExpr::BinaryExpr(std::unique_ptr<IExpr>&& lhs,
                       std::unique_ptr<IExpr>&& rhs, const ExprOp op)
    : lhs_(std::move(lhs)), rhs_(std::move(rhs)), op_(op) {}

llvm::Value* BinaryExpr::Codegen(CGContext& context) const {
  auto* lhs = lhs_->Codegen(context);
  auto* rhs = rhs_->Codegen(context);
  if (!lhs || !rhs) {
    throw std::runtime_error("Binary expression LHS or RHS is nullptr");
  }

  auto& builder = context.get_builder();
  switch (op_) {
    case ExprOp::kAdd: {
      return builder.CreateAdd(lhs, rhs, "addtmp");
    }
    case ExprOp::kSub: {
      return builder.CreateSub(lhs, rhs, "subtmp");
    }
    case ExprOp::kMul: {
      return builder.CreateMul(lhs, rhs, "multmp");
    }
    case ExprOp::kDiv: {
      return builder.CreateSDiv(lhs, rhs, "divtmp");
    }
    case ExprOp::kMod: {
      return builder.CreateSRem(lhs, rhs, "modtmp");
    }
    case ExprOp::kEq: {
      return builder.CreateICmpEQ(lhs, rhs, "eqtmp");
    }
    case ExprOp::kNe: {
      return builder.CreateICmpNE(lhs, rhs, "netmp");
    }
    case ExprOp::kLt: {
      return builder.CreateICmpSLT(lhs, rhs, "lttmp");
    }
    case ExprOp::kGt: {
      return builder.CreateICmpSGT(lhs, rhs, "gttmp");
    }
    case ExprOp::kLe: {
      return builder.CreateICmpSLE(lhs, rhs, "letmp");
    }
    case ExprOp::kGe: {
      return builder.CreateICmpSGE(lhs, rhs, "getmp");
    }
    case ExprOp::kAnd: {
      return builder.CreateAnd(lhs, rhs, "andtmp");
    }
    case ExprOp::kOr: {
      return builder.CreateOr(lhs, rhs, "ortmp");
    }
    default: {
      throw std::runtime_error("Invalid binary operator");
    }
  }
}

UnaryExpr::UnaryExpr(std::unique_ptr<IExpr>&& expr, const ExprOp op)
    : expr_(std::move(expr)), op_(op) {}

llvm::Value* UnaryExpr::Codegen(CGContext& context) const {
  auto* expr = expr_->Codegen(context);
  if (!expr) {
    throw std::runtime_error("Unary expression is nullptr");
  }

  auto& builder = context.get_builder();
  switch (op_) {
    case ExprOp::kNeg: {
      return builder.CreateNeg(expr, "negtmp");
    }
    case ExprOp::kNot: {
      return builder.CreateNot(expr, "nottmp");
    }
    default: {
      throw std::runtime_error("Invalid unary operator");
    }
  }
}

VarExpr::VarExpr(std::string&& name) : name_(std::move(name)) {}

llvm::Value* VarExpr::Codegen(CGContext& context) const {
  if (const auto found = context.FindNamedValue(name_); found.has_value()) {
    return found.value();
  }

  throw std::runtime_error("Unknown variable " + name_);
}

NumberExpr::NumberExpr(const std::int64_t value) : value_(value) {}

llvm::Value* NumberExpr::Codegen(CGContext& context) const {
  return llvm::ConstantInt::get(context.get_context(),
                                llvm::APInt(64, value_, true));
}

}  // namespace frontend
