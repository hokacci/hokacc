#! /usr/bin/env python

from pathlib import Path
import subprocess


test_dir = Path(__file__).resolve().parent
root_dir = test_dir.parent
build_dir = root_dir / "build"
exe = build_dir / "hokacc"


def test(input: str, expected: int) -> bool:
    result = subprocess.run([str(exe), input], stdout=subprocess.PIPE)
    stdout = result.stdout.decode("utf-8")
    with open("tmp.s", "w") as f:
        f.write(stdout)
    result = subprocess.run(["cc", "-o", "tmp", "tmp.s"])
    result = subprocess.run(["./tmp"])
    actual = result.returncode

    print(f"input: {input}, expected: {expected}, actual: {actual}")
    return actual == expected


def main():
    assert(test("1;", 1))
    assert(test("0;", 0))
    assert(test("42;", 42))
    assert(test("42+3;", 45))
    assert(test("13+192-2;", 203))
    assert(test(" 13 + 192 - 2  ;", 203))
    assert(test("(1 + 2) * 4 / (20 - 18);", 6))
    assert(test("+12;", 12))
    assert(test("- ((1 + 2) * 4 / (20 - 18)) + 10;", 4))
    assert(test("+ ((1 + 2) * 4 / (20 - 18)) + 10;", 16))
    assert(test("12 == 8 + 4;", 1))
    assert(test("12 < 8 + 4;", 0))
    assert(test("3 * 4 > 8 + 4;", 0))
    assert(test("3 * 4 >= 8 + 4;", 1))
    assert(test("3 + 4 <= 8 + 4 != 0;", 1))
    assert(test("a = 1;", 1))
    assert(test("a = 102; b = 2; a;", 102))
    assert(test("aiko = 1; becky = 2; aiko + becky == 3;", 1))
    print("******** All tests passed! ********")


if __name__ == "__main__":
    main()
