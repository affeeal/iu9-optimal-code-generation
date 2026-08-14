#!/usr/bin/env python3

import pathlib
import subprocess
import sys
import tempfile


def main() -> int:
    executable, llvm_as = sys.argv[1:]
    result = subprocess.run(
        [executable], check=True, capture_output=True, text=True
    )
    if "ret i32 401" not in result.stdout:
        raise RuntimeError("generated IR does not return i32 401")

    with tempfile.TemporaryDirectory() as directory:
        ir_path = pathlib.Path(directory) / "module.ll"
        bitcode_path = pathlib.Path(directory) / "module.bc"
        ir_path.write_text(result.stdout, encoding="utf-8")
        subprocess.run(
            [llvm_as, str(ir_path), "-o", str(bitcode_path)], check=True
        )

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
