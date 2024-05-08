#include "code_generator.h"

#include "node.h"

namespace frontend {

void CodeGenerator::Scope::Add(const std::string& name,
                               llvm::AllocaInst* const alloc) {
  named_allocs_[name] = alloc;
}

llvm::AllocaInst* CodeGenerator::Scope::Visible(const std::string& name) const {
  if (auto* const alloc = Find(name)) {
    return alloc;
  }

  return parent_ ? parent_->Find(name) : nullptr;
}

llvm::AllocaInst* CodeGenerator::Scope::Find(const std::string& name) const {
  if (const auto it = named_allocs_.find(name); it != named_allocs_.cend()) {
    return it->second;
  }

  return nullptr;
}

CodeGenerator::CodeGenerator()
    : context_(std::make_unique<llvm::LLVMContext>()),
      module_(std::make_unique<llvm::Module>("ParaParaCL", *context_)),
      builder_(std::make_unique<llvm::IRBuilder<>>(*context_)) {
  auto* const func_type =
      llvm::FunctionType::get(llvm::Type::getInt64Ty(*context_), false);
  main_ = llvm::Function::Create(func_type, llvm::Function::ExternalLinkage,
                                 "main", module_.get());

  auto* const bb = llvm::BasicBlock::Create(*context_, "entry", main_);
  builder_->SetInsertPoint(bb);
}

llvm::AllocaInst* CodeGenerator::CreateEntryBlockAlloca(
    const std::string& name) {
  auto& entry_block = main_->getEntryBlock();
  auto builder = llvm::IRBuilder<>(&entry_block, entry_block.begin());
  return builder.CreateAlloca(llvm::Type::getInt64Ty(*context_), nullptr, name);
}

llvm::Value* CodeGenerator::AcceptAndReturn(INode& node) {
  node.Accept(*this);
  return return_;
}

void CodeGenerator::Visit(Program& program) {
  const auto scope = std::make_unique<Scope>(nullptr);
  scope_ = scope.get();

  for (auto it = program.get_stmts_cbegin(), end = program.get_stmts_cend();
       it != end; ++it) {
    it->get()->Accept(*this);
  }
}

void CodeGenerator::Visit(AssignStmt& stmt) {
  auto* const rhs = AcceptAndReturn(stmt.get_expr());

  const auto& name = stmt.get_name();
  auto* lhs = scope_->Visible(name);
  if (!lhs) {
    lhs = CreateEntryBlockAlloca(name);
    scope_->Add(name, lhs);
  }

  builder_->CreateStore(rhs, lhs);
}

void CodeGenerator::Visit(ReturnStmt& stmt) {
  auto* const expr = AcceptAndReturn(stmt.get_expr());
  builder_->CreateRet(expr);
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
    using enum BinaryExpr::Op;
    case kAdd: {
      return_ = builder_->CreateAdd(lhs, rhs, "addtmp");
      break;
    }
    case kSub: {
      return_ = builder_->CreateSub(lhs, rhs, "subtmp");
      break;
    }
    case kMul: {
      return_ = builder_->CreateMul(lhs, rhs, "multmp");
      break;
    }
    case kDiv: {
      return_ = builder_->CreateSDiv(lhs, rhs, "divtmp");
      break;
    }
    case kMod: {
      return_ = builder_->CreateSRem(lhs, rhs, "modtmp");
      break;
    }
    case kEq: {
      return_ = builder_->CreateICmpEQ(lhs, rhs, "eqtmp");
      break;
    }
    case kNe: {
      return_ = builder_->CreateICmpNE(lhs, rhs, "netmp");
      break;
    }
    case kLt: {
      return_ = builder_->CreateICmpSLT(lhs, rhs, "lttmp");
      break;
    }
    case kGt: {
      return_ = builder_->CreateICmpSGT(lhs, rhs, "gttmp");
      break;
    }
    case kLe: {
      return_ = builder_->CreateICmpSLE(lhs, rhs, "letmp");
      break;
    }
    case kGe: {
      return_ = builder_->CreateICmpSGE(lhs, rhs, "getmp");
      break;
    }
    case kAnd: {
      return_ = builder_->CreateAnd(lhs, rhs, "andtmp");
      break;
    }
    case kOr: {
      return_ = builder_->CreateOr(lhs, rhs, "ortmp");
      break;
    }
  }
}

void CodeGenerator::Visit(UnaryExpr& expr) {
  auto* const value = AcceptAndReturn(expr);

  switch (expr.get_op()) {
    using enum UnaryExpr::Op;
    case kNeg: {
      return_ = builder_->CreateNeg(value, "negtmp");
      break;
    }
    case kNot: {
      return_ = builder_->CreateNot(value, "nottmp");
      break;
    }
  }
}

void CodeGenerator::Visit(VarExpr& expr) {
  const auto& name = expr.get_name();

  auto* const alloc = scope_->Visible(name);
  if (!alloc) {
    throw std::runtime_error("Unknown variable " + name);
  }

  return_ =
      builder_->CreateLoad(alloc->getAllocatedType(), alloc, name.c_str());
}

void CodeGenerator::Visit(NumberExpr& expr) {
  return_ = llvm::ConstantInt::get(*context_,
                                   llvm::APInt(64, expr.get_value(), true));
}

void CodeGenerator::Dump() const { module_->dump(); }

}  // namespace frontend
