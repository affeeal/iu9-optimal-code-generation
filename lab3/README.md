# Lab 3: Small Integer Language to LLVM IR

This C++20 educational frontend parses a compact imperative language and emits
LLVM IR for one implicit `main` function.

```text
scanner -> parser -> AST -> Visitor code generator -> verified LLVM IR
```

The concrete syntax supports assignment, `if`/`else`, `while`, `return`,
parentheses, variables, decimal integer literals, comparisons, arithmetic
operators, `%`, eager `&&`/`||`, unary minus, and logical negation. See the
[abstract grammar](specs/abstract_grammar.txt).

## Semantics

- Every value has signed LLVM type `i64`.
- Comparisons and logical operators return normalized `0` or `1` values.
- Conditions treat zero as false and every nonzero integer as true.
- `!x` is logical negation.
- `&&` and `||` are eager: both operands are evaluated.
- Assigning a new name declares it in the current lexical scope; assignments to
  visible outer names update the existing variable.
- A reachable path that falls through without `return` is rejected.
- Statements after a terminator are not emitted.

## Build, run, and test

Prerequisites are CMake 3.22+, a C++20 compiler, LLVM development files and
tools, Flex 2.6+, Bison 3.8+, and Python 3.

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

For example:

```text
x = 10 % 3;
if (x) {
  return !0;
} else {
  return 0;
}
```

The generated module is verified before it is printed; parse, semantic, and IR
verification failures return a nonzero process status.

## Limitations

The language has a single function, a single integer type, no function calls,
no user-defined types, and no optimization pipeline of its own. It is a course
frontend rather than a complete or standards-compliant compiler.

Verified locally with LLVM 19.1.7, Flex 2.6.4, Bison 3.8.2, GCC 14.2.0, and
CMake 3.31.6 on Debian 13.
