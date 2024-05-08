cmake_minimum_required(VERSION 3.22.1)
project(lab2)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_executable(${PROJECT_NAME} main.cc)

llvm_map_components_to_libnames(llvm_libs support core codegen irreader)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
target_include_directories(${PROJECT_NAME} PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${llvm_libs})
