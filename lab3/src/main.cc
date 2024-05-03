#include <fstream>
#include <iostream>

#include "driver.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <filename>\n";
    return 1;
  }

  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    std::cerr << "Failed to open file " << argv[1] << "\n";
    return 1;
  }

  const auto filename = std::string{argv[1]};

  auto driver = frontend::Driver{false, true};
  [[maybe_unused]] const auto is_success =
      driver.Parse(file, std::cout, &filename);
}
