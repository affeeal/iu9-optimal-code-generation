cmake_minimum_required(VERSION 3.22.1)
project(lab1 LANGUAGES CXX)

find_package(Boost 1.82.0 COMPONENTS json REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  message(FATAL_ERROR "lab1 must be built with GNU C++, not ${CMAKE_CXX_COMPILER_ID}")
endif()

set(GCC_PLUGIN_PATH "" CACHE PATH "Override the matching GCC plugin directory")
if(NOT GCC_PLUGIN_PATH)
  execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=plugin
    OUTPUT_VARIABLE GCC_PLUGIN_PATH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GCC_PLUGIN_PATH_RESULT
  )
  if(NOT GCC_PLUGIN_PATH_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to query the GCC plugin directory")
  endif()
endif()
if(NOT EXISTS "${GCC_PLUGIN_PATH}/include/gcc-plugin.h")
  message(
    FATAL_ERROR
    "GCC plugin headers are missing for ${CMAKE_CXX_COMPILER}. "
    "Install the matching gcc-${CMAKE_CXX_COMPILER_VERSION}-plugin-dev package."
  )
endif()

add_library(gimple_json_plugin SHARED lab1.cc)
set_target_properties(gimple_json_plugin PROPERTIES PREFIX "")

target_link_libraries(gimple_json_plugin PRIVATE Boost::json)

target_compile_features(gimple_json_plugin PRIVATE cxx_std_20)
target_compile_options(
  gimple_json_plugin PRIVATE -Wall -Wextra -Wpedantic -fno-rtti
)
target_include_directories(
  gimple_json_plugin PRIVATE "${GCC_PLUGIN_PATH}/include"
)

enable_testing()
add_test(
  NAME lab1_smoke
  COMMAND
    ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../tests/check_plugin.py
    ${CMAKE_CXX_COMPILER} $<TARGET_FILE:gimple_json_plugin>
    ${CMAKE_CURRENT_SOURCE_DIR}/../tests/test.cc
)
