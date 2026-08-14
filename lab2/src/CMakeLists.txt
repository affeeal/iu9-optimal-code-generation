cmake_minimum_required(VERSION 3.22.1)
project(lab2 LANGUAGES CXX)

find_package(zstd QUIET CONFIG)
find_package(LLVM REQUIRED CONFIG)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_executable(lab2 main.cc)

llvm_map_components_to_libnames(llvm_libs core support)

target_compile_features(lab2 PRIVATE cxx_std_20)
target_compile_options(lab2 PRIVATE -Wall -Wextra -Wpedantic)
target_include_directories(lab2 SYSTEM PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(lab2 PRIVATE ${llvm_libs})

find_program(
  LLVM_AS_EXECUTABLE
  NAMES llvm-as llvm-as-${LLVM_VERSION_MAJOR}
  REQUIRED
)

enable_testing()
add_test(
  NAME lab2_ir
  COMMAND
    ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../tests/check_ir.py
    $<TARGET_FILE:lab2> ${LLVM_AS_EXECUTABLE}
)
