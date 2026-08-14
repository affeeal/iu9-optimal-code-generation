#!/usr/bin/env python3

import pathlib
import subprocess
import sys
import tempfile


def compile_and_run(
    compiler: str,
    llvm_as: str,
    lli: str,
    source: pathlib.Path,
    expected: int,
) -> None:
    result = subprocess.run(
        [compiler, str(source)], check=True, capture_output=True, text=True
    )
    with tempfile.TemporaryDirectory() as directory:
        ir_path = pathlib.Path(directory) / "module.ll"
        bitcode_path = pathlib.Path(directory) / "module.bc"
        ir_path.write_text(result.stdout, encoding="utf-8")
        subprocess.run(
            [llvm_as, str(ir_path), "-o", str(bitcode_path)], check=True
        )
        execution = subprocess.run([lli, str(bitcode_path)], check=False)
        if execution.returncode != expected:
            raise RuntimeError(
                f"{source.name}: expected exit {expected}, got "
                f"{execution.returncode}"
            )


def expect_failure(compiler: str, source: pathlib.Path) -> None:
    result = subprocess.run(
        [compiler, str(source)], check=False, capture_output=True, text=True
    )
    if result.returncode == 0:
        raise RuntimeError(f"{source.name}: compilation unexpectedly succeeded")


def main() -> int:
    compiler, llvm_as, lli, fibonacci_path, cases_path = sys.argv[1:]
    cases = pathlib.Path(cases_path)

    compile_and_run(
        compiler, llvm_as, lli, pathlib.Path(fibonacci_path), expected=55
    )
    for name, expected in (
        ("modulo.dat", 1),
        ("unary.dat", 6),
        ("comparison-value.dat", 1),
        ("nested-scope.dat", 5),
        ("return-in-branch.dat", 7),
    ):
        compile_and_run(compiler, llvm_as, lli, cases / name, expected)

    for name in (
        "unknown-variable.dat",
        "invalid-character.dat",
        "fallthrough.dat",
    ):
        expect_failure(compiler, cases / name)
    expect_failure(compiler, cases / "does-not-exist.dat")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
