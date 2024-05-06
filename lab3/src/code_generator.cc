#include "code_generator.h"

#include "node.h"

namespace frontend {

CodeGenerator::CodeGenerator()
    : context_(std::make_unique<llvm::LLVMContext>()),
      module_(std::make_unique<llvm::Module>("a module", *context_)),
      builder_(std::make_unique<llvm::IRBuilder<>>(*context_)) {}

llvm::Value* CodeGenerator::AcceptAndReturn(INode& node) {
  node.Accept(*this);
  return return_;
}

llvm::AllocaInst* CodeGenerator::CreateEntryBlockAlloca(
    llvm::Function& function, const std::string& name) {
  auto& entry_block = function.getEntryBlock();
  auto builder = llvm::IRBuilder<>(&entry_block, entry_block.begin());
  return builder.CreateAlloca(llvm::Type::getInt64Ty(*context_), nullptr, name);
}

void CodeGenerator::Visit(Program& program) {
  // TODO
}

void CodeGenerator::Visit(FuncProto& proto) {
  // TODO
}

void CodeGenerator::Visit(FuncDef& def) {
  // TODO
}

void CodeGenerator::Visit(Scope& scope) {
  // TODO
}

void CodeGenerator::Visit(AssignStmt& stmt) {
  // TODO
}

void CodeGenerator::Visit(ReturnStmt& stmt) {
  // TODO
}

void CodeGenerator::Visit(IfStmt& stmt) {
  // TODO
}

void CodeGenerator::Visit(WhileStmt& stmt) {
  // TODO
}

void CodeGenerator::Visit(BinaryExpr& expr) {
  auto* const lhs = AcceptAndReturn(expr.get_lhs());
  auto* const rhs = AcceptAndReturn(expr.get_rhs());

  switch (expr.get_op()) {
    case ExprOp::kAdd: {
      return_ = builder_->CreateAdd(lhs, rhs, "addtmp");
      break;
    }
    case ExprOp::kSub: {
      return_ = builder_->CreateSub(lhs, rhs, "subtmp");
      break;
    }
    case ExprOp::kMul: {
      return_ = builder_->CreateMul(lhs, rhs, "multmp");
      break;
    }
    case ExprOp::kDiv: {
      return_ = builder_->CreateSDiv(lhs, rhs, "divtmp");
      break;
    }
    case ExprOp::kMod: {
      return_ = builder_->CreateSRem(lhs, rhs, "modtmp");
      break;
    }
    case ExprOp::kEq: {
      return_ = builder_->CreateICmpEQ(lhs, rhs, "eqtmp");
      break;
    }
    case ExprOp::kNe: {
      return_ = builder_->CreateICmpNE(lhs, rhs, "netmp");
      break;
    }
    case ExprOp::kLt: {
      return_ = builder_->CreateICmpSLT(lhs, rhs, "lttmp");
      break;
    }
    case ExprOp::kGt: {
      return_ = builder_->CreateICmpSGT(lhs, rhs, "gttmp");
      break;
    }
    case ExprOp::kLe: {
      return_ = builder_->CreateICmpSLE(lhs, rhs, "letmp");
      break;
    }
    case ExprOp::kGe: {
      return_ = builder_->CreateICmpSGE(lhs, rhs, "getmp");
      break;
    }
    case ExprOp::kAnd: {
      return_ = builder_->CreateAnd(lhs, rhs, "andtmp");
      break;
    }
    case ExprOp::kOr: {
      return_ = builder_->CreateOr(lhs, rhs, "ortmp");
      break;
    }
    default: {
      throw std::runtime_error("Invalid binary operator");
    }
  }
}

void CodeGenerator::Visit(UnaryExpr& expr) {
  auto* const subject = AcceptAndReturn(expr);

  switch (expr.get_op()) {
    case ExprOp::kNeg: {
      return_ = builder_->CreateNeg(subject, "negtmp");
      break;
    }
    case ExprOp::kNot: {
      return_ = builder_->CreateNot(subject, "nottmp");
      break;
    }
    default: {
      throw std::runtime_error("Invalid unary operator");
    }
  }
}

void CodeGenerator::Visit(VarExpr& expr) {
  const auto& name = expr.get_name();

  const auto it = named_allocas_.find(name);
  if (it == named_allocas_.cend()) {
    throw std::runtime_error("Unknown variable " + name);
  }

  auto* alloca = it->second;
  return_ =
      builder_->CreateLoad(alloca->getAllocatedType(), alloca, name.c_str());
}

void CodeGenerator::Visit(NumberExpr& expr) {
  return_ = llvm::ConstantInt::get(*context_,
                                   llvm::APInt(64, expr.get_value(), true));
}

}  // namespace frontend
