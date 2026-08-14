#!/usr/bin/env bash

set -euo pipefail

repo_root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)
if [[ -n ${BUILD_ROOT:-} ]]; then
  build_root=$BUILD_ROOT
  mkdir -p -- "$build_root"
else
  build_root=$(mktemp -d "${TMPDIR:-/tmp}/iu9-codegen-check.XXXXXXXX")
  trap 'rm -rf -- "$build_root"' EXIT
fi

find_command() {
  local candidate
  for candidate in "$@"; do
    if command -v "$candidate" >/dev/null 2>&1; then
      command -v "$candidate"
      return 0
    fi
  done
  return 1
}

llvm_config=${LLVM_CONFIG:-}
if [[ -z $llvm_config ]]; then
  llvm_config=$(find_command llvm-config llvm-config-19 llvm-config-18 || true)
fi

if [[ -n $llvm_config ]]; then
  llvm_dir=$($llvm_config --cmakedir)
  echo "LLVM: $($llvm_config --version)"

  cmake -S "$repo_root/lab2/src" -B "$build_root/lab2" \
    -DCMAKE_BUILD_TYPE=Release -DLLVM_DIR="$llvm_dir"
  cmake --build "$build_root/lab2" --parallel
  ctest --test-dir "$build_root/lab2" --output-on-failure

  if command -v flex >/dev/null 2>&1 && command -v bison >/dev/null 2>&1; then
    cmake -S "$repo_root/lab3/src" -B "$build_root/lab3" \
      -DCMAKE_BUILD_TYPE=Release -DLLVM_DIR="$llvm_dir"
    cmake --build "$build_root/lab3" --parallel
    ctest --test-dir "$build_root/lab3" --output-on-failure
  else
    echo "SKIP lab3: Flex and Bison are required"
  fi
else
  echo "SKIP lab2 and lab3: llvm-config was not found"
fi

cxx=${CXX:-c++}
gcc_plugin_path=${GCC_PLUGIN_PATH:-}
if [[ -z $gcc_plugin_path ]] && command -v "$cxx" >/dev/null 2>&1; then
  gcc_plugin_path=$($cxx -print-file-name=plugin)
fi
if command -v "$cxx" >/dev/null 2>&1 && \
    [[ $($cxx -dumpversion 2>/dev/null || true) =~ ^[0-9]+ ]] && \
    [[ -f $gcc_plugin_path/include/gcc-plugin.h ]]; then
  echo "GCC: $($cxx --version | head -n 1)"
  cmake -S "$repo_root/lab1/src" -B "$build_root/lab1" \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER="$cxx" \
    -DGCC_PLUGIN_PATH="$gcc_plugin_path"
  cmake --build "$build_root/lab1" --parallel
  ctest --test-dir "$build_root/lab1" --output-on-failure
else
  echo "SKIP lab1: matching GNU compiler plugin headers were not found"
fi
