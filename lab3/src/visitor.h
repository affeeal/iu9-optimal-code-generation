#pragma once

namespace frontend {

class INode;
class Program;
class FuncProto;
class FuncDef;
class Scope;
class AssignStmt;
class ReturnStmt;
class IfStmt;
class WhileStmt;
class BinaryExpr;
class UnaryExpr;
class VarExpr;
class NumberExpr;

class IVisitor {
 public:
  virtual ~IVisitor() = default;

 public:
  virtual void Visit(Program& program) = 0;
  virtual void Visit(FuncProto& proto) = 0;
  virtual void Visit(FuncDef& def) = 0;
  virtual void Visit(Scope& scope) = 0;
  virtual void Visit(AssignStmt& stmt) = 0;
  virtual void Visit(ReturnStmt& stmt) = 0;
  virtual void Visit(IfStmt& stmt) = 0;
  virtual void Visit(WhileStmt& stmt) = 0;
  virtual void Visit(BinaryExpr& expr) = 0;
  virtual void Visit(UnaryExpr& expr) = 0;
  virtual void Visit(VarExpr& expr) = 0;
  virtual void Visit(NumberExpr& expr) = 0;
};

}  // namespace frontend
