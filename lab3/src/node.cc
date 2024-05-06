#include "node.h"

namespace frontend {

void Program::Accept(IVisitor& visitor) { visitor.Visit(*this); }

}  // namespace frontend
