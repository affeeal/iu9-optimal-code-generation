cmake_minimum_required(VERSION 3.22.1)
project(lab1)

find_package(Boost 1.82.0 COMPONENTS json REQUIRED)

execute_process(
  COMMAND bash -c "echo -n `gcc -print-file-name=plugin`"
  OUTPUT_VARIABLE PluginPath
)

add_library(${PROJECT_NAME} SHARED lab1.cc)
set_property(TARGET ${PROJECT_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(${PROJECT_NAME} PRIVATE Boost::json)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -fno-rtti)

target_include_directories(${PROJECT_NAME} PRIVATE ${PluginPath}/include)
