# GCC and LLVM Code Generation Labs

Three C++20 compiler-construction assignments completed at BMSTU IU9 in 2024:
a GCC pass that inspects GIMPLE/SSA, direct construction of LLVM IR, and a small
Flex/Bison frontend that lowers an integer language to LLVM IR.

## Highlights

- traversal of GCC functions, control-flow graphs, SSA values, and selected
  GIMPLE/tree nodes;
- structured JSON output from a GCC plugin;
- a Flex/Bison frontend with an AST and Visitor-based LLVM code generation;
- lexically scoped variables, integer control flow, and LLVM verification;
- CLI-level regression tests that validate and execute generated IR.

## Projects

| Project | What it does | Main technologies | Status |
|---|---|---|---|
| [lab1](lab1/README.md) | Attaches a plugin pass after SSA and emits functions, basic blocks, edges, and selected GIMPLE/tree nodes as JSON | GCC plugin API, Boost.JSON, C++20 | GCC-major-specific smoke test |
| [lab2](lab2/README.md) | Constructs a minimal `main` function with `llvm::IRBuilder` | LLVM C++ API, C++20 | Verifier and `llvm-as` smoke test |
| [lab3](lab3/README.md) | Parses a small integer language and emits executable LLVM IR | Flex, Bison, LLVM, C++20 | Positive and negative CLI tests |

## Quick start: lab3

The commands below were verified on Debian 13 with CMake 3.31.6, GCC 14.2.0,
LLVM 19.1.7, Flex 2.6.4, and Bison 3.8.2:

```sh
cmake -S lab3/src -B build/lab3 \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_DIR=/usr/lib/llvm-19/cmake
cmake --build build/lab3 --parallel
ctest --test-dir build/lab3 --output-on-failure

./build/lab3/ParaParaCL lab3/examples/001.dat > /tmp/fibonacci.ll
llvm-as-19 /tmp/fibonacci.ll -o /tmp/fibonacci.bc
lli-19 /tmp/fibonacci.bc
test "$?" -eq 55
```

`lli` reports the program's result as its process status; the Fibonacci example
returns `55`.

## Building each lab

### lab1: GCC GIMPLE inspection

Install Boost.JSON and the plugin headers that exactly match the selected GCC
major version. A plugin built for one GCC major version must not be loaded by a
different one.

```sh
cmake -S lab1/src -B build/lab1 \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-14
cmake --build build/lab1 --parallel
ctest --test-dir build/lab1 --output-on-failure

g++-14 -O1 -fplugin=build/lab1/gimple_json_plugin.so \
  -c lab1/tests/test.cc -o /tmp/lab1-test.o > /tmp/gimple.json
python3 -m json.tool /tmp/gimple.json > /dev/null
```

The original assignment used GCC 11.4.0 and Boost.JSON 1.82.0. The current
smoke test was also verified with GCC 14.2.0 and Boost.JSON 1.83.0.

### lab2: direct LLVM construction

```sh
cmake -S lab2/src -B build/lab2 \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_DIR=/usr/lib/llvm-19/cmake
cmake --build build/lab2 --parallel
ctest --test-dir build/lab2 --output-on-failure
./build/lab2/lab2 > /tmp/lab2.ll
llvm-as-19 /tmp/lab2.ll -o /tmp/lab2.bc
```

The generated `main` returns `401` as an `i32`. Unix process statuses retain
only the low eight bits, so the test checks the IR rather than treating an
`lli` process status as the full value.

### lab3: source-to-LLVM frontend

Lab3 additionally requires Flex 2.6+ and Bison 3.8+. See the [language grammar](
lab3/specs/abstract_grammar.txt) and [lab README](lab3/README.md).

## Example

```text
x = 10 % 3;
return x;
```

The frontend emits verifier-clean LLVM IR containing an `srem i64` operation;
executing it returns `1`.

## Repository layout

```text
lab1/  GCC GIMPLE/SSA inspection plugin
lab2/  direct LLVM IRBuilder exercise
lab3/  scanner -> parser -> AST -> LLVM IR frontend
notes/ original Russian course notes
scripts/check.sh  repository-level build and test entry point
```

Run every locally available check with:

```sh
scripts/check.sh
```

The script uses out-of-source temporary builds and explains any dependency-based
skip.

## Limitations

- These are educational assignments, not a production compiler toolchain.
- Lab3 has one signed 64-bit integer type and no optimizer of its own.
- Lab3 `&&` and `||` evaluate both operands eagerly; boolean results are
  normalized to `0` or `1`.
- Lab1 serializes a useful subset of GIMPLE/tree nodes and reports unsupported
  nodes without attempting a complete GCC IR schema.
- Verified versions describe tested environments, not a broad compatibility
  guarantee.

## License

The project is distributed under [GPL-3.0](LICENSE). GCC, LLVM, Boost, Flex,
and Bison are independent third-party projects under their own licenses.
