#include "code_generator.h"

// clang-format off
#include "llvm/IR/IRBuilder.h"
// clang-format on

#include "node.h"

namespace frontend {

namespace {

class Scope final {
 public:
  Scope(const Scope* const parent) noexcept : parent_(parent) {}

  void Add(const std::string& name, llvm::AllocaInst* const alloc);
  llvm::AllocaInst* Visible(const std::string& name) const;
  llvm::AllocaInst* Find(const std::string& name) const;

 private:
  const Scope* parent_;
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

  llvm::LLVMContext* get_context() noexcept { return context_.get(); }
  llvm::Module* get_module() noexcept { return module_.get(); }
  llvm::IRBuilder<>* get_builder() noexcept { return builder_.get(); }

  void set_return(llvm::Value* value) noexcept { return_ = value; }
  llvm::Value* get_return() noexcept { return return_; }

  void set_scope(Scope* scope) noexcept { scope_ = scope; }
  Scope* get_scope() noexcept { return scope_; }

  llvm::AllocaInst* CreateEntryBlockAlloca(const std::string& name);

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

llvm::AllocaInst* CodeGenerator::Impl::CreateEntryBlockAlloca(
    const std::string& name) {
  auto& entry_block = main_->getEntryBlock();
  auto builder = llvm::IRBuilder<>(&entry_block, entry_block.begin());
  return builder.CreateAlloca(llvm::Type::getInt64Ty(*context_), nullptr, name);
}

CodeGenerator::CodeGenerator()
    : impl_(std::make_unique<CodeGenerator::Impl>()) {}

CodeGenerator::~CodeGenerator() = default;

llvm::Value* CodeGenerator::AcceptAndReturn(INode& node) {
  node.Accept(*this);
  return impl_->get_return();
}

void CodeGenerator::Visit(Program& program) {
  const auto scope = std::make_unique<Scope>(nullptr);
  impl_->set_scope(scope.get());

  for (auto it = program.get_stmts_cbegin(), end = program.get_stmts_cend();
       it != end; ++it) {
    it->get()->Accept(*this);
  }
}

void CodeGenerator::Visit(AssignStmt& stmt) {
  auto* const rhs = AcceptAndReturn(stmt.get_expr());

  const auto& name = stmt.get_name();

  auto* lhs = impl_->get_scope()->Visible(name);
  if (!lhs) {
    lhs = impl_->CreateEntryBlockAlloca(name);
    impl_->get_scope()->Add(name, lhs);
  }

  impl_->get_builder()->CreateStore(rhs, lhs);
}

void CodeGenerator::Visit(ReturnStmt& stmt) {
  auto* const expr = AcceptAndReturn(stmt.get_expr());
  impl_->get_builder()->CreateRet(expr);
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
      impl_->set_return(impl_->get_builder()->CreateAdd(lhs, rhs, "addtmp"));
      break;
    }
    case kSub: {
      impl_->set_return(impl_->get_builder()->CreateSub(lhs, rhs, "subtmp"));
      break;
    }
    case kMul: {
      impl_->set_return(impl_->get_builder()->CreateMul(lhs, rhs, "multmp"));
      break;
    }
    case kDiv: {
      impl_->set_return(impl_->get_builder()->CreateSDiv(lhs, rhs, "divtmp"));
      break;
    }
    case kMod: {
      impl_->set_return(impl_->get_builder()->CreateSRem(lhs, rhs, "modtmp"));
      break;
    }
    case kEq: {
      impl_->set_return(impl_->get_builder()->CreateICmpEQ(lhs, rhs, "eqtmp"));
      break;
    }
    case kNe: {
      impl_->set_return(impl_->get_builder()->CreateICmpNE(lhs, rhs, "netmp"));
      break;
    }
    case kLt: {
      impl_->set_return(impl_->get_builder()->CreateICmpSLT(lhs, rhs, "lttmp"));
      break;
    }
    case kGt: {
      impl_->set_return(impl_->get_builder()->CreateICmpSGT(lhs, rhs, "gttmp"));
      break;
    }
    case kLe: {
      impl_->set_return(impl_->get_builder()->CreateICmpSLE(lhs, rhs, "letmp"));
      break;
    }
    case kGe: {
      impl_->set_return(impl_->get_builder()->CreateICmpSGE(lhs, rhs, "getmp"));
      break;
    }
    case kAnd: {
      impl_->set_return(impl_->get_builder()->CreateAnd(lhs, rhs, "andtmp"));
      break;
    }
    case kOr: {
      impl_->set_return(impl_->get_builder()->CreateOr(lhs, rhs, "ortmp"));
      break;
    }
  }
}

void CodeGenerator::Visit(UnaryExpr& expr) {
  auto* const value = AcceptAndReturn(expr);

  switch (expr.get_op()) {
    using enum UnaryExpr::Op;
    case kNeg: {
      impl_->set_return(impl_->get_builder()->CreateNeg(value, "negtmp"));
      break;
    }
    case kNot: {
      impl_->set_return(impl_->get_builder()->CreateNot(value, "nottmp"));
      break;
    }
  }
}

void CodeGenerator::Visit(VarExpr& expr) {
  const auto& name = expr.get_name();

  auto* const alloc = impl_->get_scope()->Visible(name);
  if (!alloc) {
    throw std::runtime_error("Unknown variable " + name);
  }

  impl_->set_return(impl_->get_builder()->CreateLoad(alloc->getAllocatedType(),
                                                     alloc, name.c_str()));
}

void CodeGenerator::Visit(NumberExpr& expr) {
  impl_->set_return(llvm::ConstantInt::get(
      *impl_->get_context(), llvm::APInt(64, expr.get_value(), true)));
}

void CodeGenerator::Dump() { impl_->get_module()->dump(); }

}  // namespace frontend
