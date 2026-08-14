#!/usr/bin/env python3

import json
import pathlib
import subprocess
import sys
import tempfile


def statements(document: dict):
    for function in document["functions"]:
        for block in function["basic_blocks"]:
            yield from block["statements"]


def main() -> int:
    compiler, plugin, source = sys.argv[1:]
    with tempfile.TemporaryDirectory() as directory:
        output = pathlib.Path(directory) / "test.o"
        result = subprocess.run(
            [
                compiler,
                "-std=c++20",
                "-O1",
                f"-fplugin={plugin}",
                "-c",
                source,
                "-o",
                str(output),
            ],
            check=True,
            capture_output=True,
            text=True,
        )

    document = json.loads(result.stdout)
    functions = document.get("functions")
    if not isinstance(functions, list) or not functions:
        raise RuntimeError("plugin output has no functions array")

    names = {function.get("name") for function in functions}
    for expected in ("Foo", "Apply", "Twice", "WideInteger", "main"):
        if expected not in names:
            raise RuntimeError(f"function {expected!r} is missing")

    for function in functions:
        blocks = function.get("basic_blocks")
        if not isinstance(blocks, list):
            raise RuntimeError("function has no basic_blocks array")
        for block in blocks:
            for key in ("index", "predecessors", "successors", "statements"):
                if key not in block:
                    raise RuntimeError(f"basic block is missing {key!r}")

    all_statements = list(statements(document))
    statement_types = {statement.get("type") for statement in all_statements}
    for expected in ("gimple_assign", "gimple_call", "gimple_return"):
        if expected not in statement_types:
            raise RuntimeError(f"statement type {expected!r} is missing")

    if not any(
        statement.get("type") == "gimple_call"
        and statement.get("callee_name") == "<indirect>"
        for statement in all_statements
    ):
        raise RuntimeError("indirect call was not serialized")
    if not any(
        statement.get("type") == "gimple_return" and "value" in statement
        for statement in all_statements
    ):
        raise RuntimeError("non-void return value was not serialized")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
