# Lab 1: GCC GIMPLE/SSA Inspection Plugin

This C++20 GCC plugin registers a `print` pass immediately after SSA. It walks
functions and basic blocks, records predecessor/successor edges, and serializes
selected GIMPLE statements and tree operands as JSON.

## Prerequisites

- CMake 3.22 or newer;
- a GNU C++ compiler and its matching plugin development headers;
- Boost.JSON 1.82 or newer;
- Python 3 for the smoke test.

GCC plugin ABIs are tied to the compiler major version. Configure and run the
plugin with the same compiler, for example GCC 14 throughout:

```sh
cmake -S lab1/src -B build/lab1 \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-14
cmake --build build/lab1 --parallel
g++-14 -O1 -fplugin=build/lab1/gimple_json_plugin.so \
  -c lab1/tests/test.cc -o /tmp/lab1-test.o > /tmp/gimple.json
python3 -m json.tool /tmp/gimple.json
```

Run the structural smoke test with:

```sh
ctest --test-dir build/lab1 --output-on-failure
```

The original submission was built with GCC 11.4.0 and Boost.JSON 1.82.0.
`tests/test.json` is a labeled sample from that environment; block identifiers
and SSA versions can differ across GCC releases. The automated test checks
schema invariants instead of comparing the full sample byte-for-byte. The
current implementation was also verified with GCC 14.2.0 and Boost.JSON 1.83.0.

## JSON overview

The root contains a `functions` array. Each function has a name and
`basic_blocks`; each block contains its numeric index, predecessor and successor
indices, and serialized statements. Supported statements include assignments,
calls, conditions, labels, and returns. Operands include common declarations,
SSA names, constants, array/component references, addresses, and memory
references. Indirect calls use `"callee_name": "<indirect>"` and include the
callee expression.

Large integer constants are serialized as exact decimal strings. String
constants use GCC's explicit byte length, so embedded NUL bytes are not silently
truncated.

## Limitations

This is an inspection aid for an educational assignment, not a stable exchange
format for all GCC IR. Unsupported GIMPLE and tree nodes are reported on stderr
and are not fully serialized. Output details can change with GCC's internal IR,
which is another reason to keep builds and tests compiler-major-specific.
