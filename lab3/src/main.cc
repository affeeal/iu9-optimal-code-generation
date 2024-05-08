#include "code_generator.h"
#include "driver.h"

int main(int argc, char* argv[]) try {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }

  auto driver = frontend::Driver{};
  driver.Parse(argv[1]);

  auto* program = driver.get_program();
  auto code_generator = frontend::CodeGenerator{};
  program->Accept(code_generator);

  code_generator.Dump();
} catch (const std::exception& e) {
  std::cerr << e.what() << std::endl;
}
