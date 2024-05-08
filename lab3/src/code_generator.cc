#include "code_generator.h"

// clang-format off
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
// clang-format on

#include "node.h"

namespace frontend {

namespace {

class Scope final {
 public:
  Scope(Scope* const parent) noexcept : parent_(parent) {}

  Scope* get_parent() noexcept { return parent_; }
  const Scope* get_parent() const noexcept { return parent_; }

  void Add(const std::string& name, llvm::AllocaInst* const alloc);
  llvm::AllocaInst* Visible(const std::string& name) const;
  llvm::AllocaInst* Find(const std::string& name) const;

 private:
  Scope* parent_;
  std::unordered_map<std::string, llvm::AllocaInst*> named_allocs_;
};

void Scope::Add(const std::string& name, llvm::AllocaInst* const alloc) {
  named_allocs_[name] = alloc;
}

llvm::AllocaInst* Scope::Visible(const std::string& name) const {
  if (auto* const alloc = Find(name)) {
    return alloc;
  }

  return parent_ ? parent_->Find(name) : nullptr;
}

llvm::AllocaInst* Scope::Find(const std::string& name) const {
  if (const auto it = named_allocs_.find(name); it != named_allocs_.cend()) {
    return it->second;
  }

  return nullptr;
}

}  // namespace

class CodeGenerator::Impl final {
 public:
  Impl();

  void Visit(CodeGenerator& visitor, Program& program);
  void Visit(CodeGenerator& visitor, AssignStmt& stmt);
  void Visit(CodeGenerator& visitor, IfStmt& stmt);
  void Visit(CodeGenerator& visitor, WhileStmt& stmt);
  void Visit(CodeGenerator& visitor, ReturnStmt& stmt);
  void Visit(CodeGenerator& visitor, BinaryExpr& expr);
  void Visit(CodeGenerator& visitor, UnaryExpr& expr);
  void Visit(CodeGenerator& visitor, VarExpr& expr);
  void Visit(CodeGenerator& visitor, NumberExpr& expr);

  void Dump();

 private:
  llvm::AllocaInst* CreateEntryBlockAlloca(const std::string& name);
  llvm::Value* AcceptAndReturn(CodeGenerator& visitor, INode& node);

 private:
  std::unique_ptr<llvm::LLVMContext> context_;
  std::unique_ptr<llvm::Module> module_;
  std::unique_ptr<llvm::IRBuilder<>> builder_;

  llvm::Function* main_;
  llvm::Value* return_;

  Scope* scope_;
};

CodeGenerator::Impl::Impl()
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

void CodeGenerator::Impl::Visit(CodeGenerator& visitor, Program& program) {
  const auto scope = std::make_unique<Scope>(nullptr);
  scope_ = scope.get();

  for (auto it = program.get_stmts_cbegin(), end = program.get_stmts_cend();
       it != end; ++it) {
    it->get()->Accept(visitor);
  }
}

void CodeGenerator::Impl::Visit(CodeGenerator& visitor, AssignStmt& stmt) {
  auto* const rhs = AcceptAndReturn(visitor, stmt.get_expr());

  const auto& name = stmt.get_name();
  auto* lhs = scope_->Visible(name);
  if (!lhs) {
    lhs = CreateEntryBlockAlloca(name);
    scope_->Add(name, lhs);
  }

  builder_->CreateStore(rhs, lhs);
}

void CodeGenerator::Impl::Visit(CodeGenerator& visitor, ReturnStmt& stmt) {
  auto* const expr = AcceptAndReturn(visitor, stmt.get_expr());
  builder_->CreateRet(expr);
}

void CodeGenerator::Impl::Visit(CodeGenerator& visitor, IfStmt& stmt) {
  auto* const cond = AcceptAndReturn(visitor, stmt.get_cond());
  auto* const then_bb = llvm::BasicBlock::Create(*context_, "then", main_);
  auto* const else_bb = llvm::BasicBlock::Create(*context_, "else", main_);
  auto* const merge_bb = llvm::BasicBlock::Create(*context_, "ifcont", main_);

  builder_->CreateCondBr(cond, then_bb, else_bb);

  {
    const auto then_scope = std::make_unique<Scope>(scope_);
    scope_ = then_scope.get();
    builder_->SetInsertPoint(then_bb);
    for (auto it = stmt.get_then_cbegin(), end = stmt.get_then_cend();
         it != end; ++it) {
      it->get()->Accept(visitor);
    }
    builder_->CreateBr(merge_bb);
    scope_ = scope_->get_parent();
  }

  {
    const auto else_scope = std::make_unique<Scope>(scope_);
    scope_ = else_scope.get();
    builder_->SetInsertPoint(else_bb);
    for (auto it = stmt.get_else_cbegin(), end = stmt.get_else_cend();
         it != end; ++it) {
      it->get()->Accept(visitor);
    }
    builder_->CreateBr(merge_bb);
    scope_ = scope_->get_parent();
  }

  builder_->SetInsertPoint(merge_bb);
}

void CodeGenerator::Impl::Visit(CodeGenerator& visitor, WhileStmt& stmt) {
  // TODO
}

void CodeGenerator::Impl::Visit(CodeGenerator& visitor, BinaryExpr& expr) {
  auto* const lhs = AcceptAndReturn(visitor, expr.get_lhs());
  auto* const rhs = AcceptAndReturn(visitor, expr.get_rhs());

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

void CodeGenerator::Impl::Visit(CodeGenerator& visitor, UnaryExpr& expr) {
  auto* const value = AcceptAndReturn(visitor, expr);

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

void CodeGenerator::Impl::Visit(CodeGenerator& visitor, VarExpr& expr) {
  const auto& name = expr.get_name();

  auto* const alloc = scope_->Visible(name);
  if (!alloc) {
    throw std::runtime_error("Unknown variable " + name);
  }

  return_ =
      builder_->CreateLoad(alloc->getAllocatedType(), alloc, name.c_str());
}

void CodeGenerator::Impl::Visit(CodeGenerator& visitor, NumberExpr& expr) {
  return_ = llvm::ConstantInt::get(*context_,
                                   llvm::APInt(64, expr.get_value(), true));
}

void CodeGenerator::Impl::Dump() { module_->dump(); }

llvm::AllocaInst* CodeGenerator::Impl::CreateEntryBlockAlloca(
    const std::string& name) {
  auto& entry_block = main_->getEntryBlock();
  auto builder = llvm::IRBuilder<>(&entry_block, entry_block.begin());
  return builder.CreateAlloca(llvm::Type::getInt64Ty(*context_), nullptr, name);
}

llvm::Value* CodeGenerator::Impl::AcceptAndReturn(CodeGenerator& code_generator,
                                                  INode& node) {
  node.Accept(code_generator);
  return return_;
}

CodeGenerator::CodeGenerator()
    : impl_(std::make_unique<CodeGenerator::Impl>()) {}

CodeGenerator::~CodeGenerator() = default;

void CodeGenerator::Visit(Program& program) { impl_->Visit(*this, program); }
void CodeGenerator::Visit(AssignStmt& stmt) { impl_->Visit(*this, stmt); }
void CodeGenerator::Visit(IfStmt& stmt) { impl_->Visit(*this, stmt); }
void CodeGenerator::Visit(WhileStmt& stmt) { impl_->Visit(*this, stmt); }
void CodeGenerator::Visit(ReturnStmt& stmt) { impl_->Visit(*this, stmt); }
void CodeGenerator::Visit(BinaryExpr& expr) { impl_->Visit(*this, expr); }
void CodeGenerator::Visit(UnaryExpr& expr) { impl_->Visit(*this, expr); }
void CodeGenerator::Visit(VarExpr& expr) { impl_->Visit(*this, expr); }
void CodeGenerator::Visit(NumberExpr& expr) { impl_->Visit(*this, expr); }

void CodeGenerator::Dump() { impl_->Dump(); }

}  // namespace frontend
