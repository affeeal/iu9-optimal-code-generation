# Lab 2: Direct LLVM IR Construction

This small C++20 exercise constructs the following function with LLVM's C++ API
and prints the resulting module:

```c
int main() {
  return 353 + 48;
}
```

## Build and verify

Prerequisites are CMake 3.22+, LLVM development files, `llvm-as`, a C++20
compiler, and Python 3 for CTest.

```sh
cmake -S lab2/src -B build/lab2 \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_DIR=/usr/lib/llvm-19/cmake
cmake --build build/lab2 --parallel
ctest --test-dir build/lab2 --output-on-failure

./build/lab2/lab2 > /tmp/lab2.ll
llvm-as-19 /tmp/lab2.ll -o /tmp/lab2.bc
```

The program verifies the generated function before printing it. The expected IR
return value is `i32 401`. Because Unix process statuses are eight bits, the
test validates the IR directly instead of interpreting an `lli` status as 401.

Verified locally with LLVM 19.1.7, GCC 14.2.0, and CMake 3.31.6 on Debian 13.
