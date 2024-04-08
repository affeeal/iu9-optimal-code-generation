cmake_minimum_required(VERSION 3.22.1)
project(lab1 VERSION 1.0.0 DESCRIPTION "GIMPLE/IR print GCC plugin")

execute_process(
  COMMAND bash -c "echo -n `gcc -print-file-name=plugin`"
  OUTPUT_VARIABLE PluginPath
)

add_library(${PROJECT_NAME} SHARED lab1.cc)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_include_directories(${PROJECT_NAME} PRIVATE ${PluginPath}/include)
