cmake_minimum_required(VERSION 3.22.1)
project(ParaParaCL LANGUAGES CXX)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
find_package(zstd QUIET CONFIG)
find_package(LLVM REQUIRED CONFIG)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

flex_target(scanner
  scanner.l
  ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc
)

bison_target(parser
  parser.y
  ${CMAKE_CURRENT_BINARY_DIR}/parser.cc
  COMPILE_FLAGS "--defines=${CMAKE_CURRENT_BINARY_DIR}/parser.h")

add_flex_bison_dependency(scanner parser)

add_executable(ParaParaCL
  code_generator.cc
  driver.cc
  main.cc
  ${BISON_parser_OUTPUTS}
  ${FLEX_scanner_OUTPUTS}
)

llvm_map_components_to_libnames(llvm_libs core support)

target_compile_features(ParaParaCL PRIVATE cxx_std_20)
target_compile_options(ParaParaCL PRIVATE -Wall -Wextra -Wpedantic)
target_include_directories(
  ParaParaCL PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
)
target_include_directories(ParaParaCL SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(ParaParaCL PRIVATE ${llvm_libs})

find_program(
  LLVM_AS_EXECUTABLE
  NAMES llvm-as llvm-as-${LLVM_VERSION_MAJOR}
  REQUIRED
)
find_program(
  LLI_EXECUTABLE
  NAMES lli lli-${LLVM_VERSION_MAJOR}
  REQUIRED
)

enable_testing()
add_test(
  NAME lab3_cli
  COMMAND
    ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../tests/check_cli.py
    $<TARGET_FILE:ParaParaCL> ${LLVM_AS_EXECUTABLE} ${LLI_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/../examples/001.dat
    ${CMAKE_CURRENT_SOURCE_DIR}/../tests/cases
)
