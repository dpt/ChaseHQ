#!/usr/bin/env python3
r"""Align the leading variable-declaration block of each function to the
house style in docs/function_comment_template_example.c (type, name, and
trailing "(was X)" comment each in their own column).

Only the block of plain "type name;" lines immediately after a function's
opening brace is touched -- the same block the style guide requires to be
declared "at the top of their scope, one per line ... ordered by first use".
Each block is reformatted in isolation through clang-format (which already
does exactly this alignment via AlignConsecutiveDeclarations /
AlignTrailingComments in .clang-format) and spliced back only if the result
is a pure whitespace rearrangement -- same token stream, same line count.
That guard is what makes this safe to run over files like Main.c that are
not otherwise clang-format-clean: it cannot touch braces, macros, or
anything clang-format would restructure, only column padding.

Usage: python3 align_declarations.py [--fix] [file.c ...]
Default: all *.c in libraries/ChaseHQ/Engine/, check-only unless --fix given.
"""

import glob
import os
import re
import subprocess
import sys

DEFAULT_GLOB = "libraries/ChaseHQ/Engine/*.c"
STYLE_PATH = os.path.join(os.path.dirname(__file__), "..", ".clang-format")

FUNC_OPEN_RE = re.compile(r"^\{$")
DECL_LINE_RE = re.compile(
    r"^[A-Za-z_][\w\s\*]*\*?\s+\**[A-Za-z_]\w*(\[\w*\])?;(\s*/\*.*\*/)?\s*$"
)
NOT_DECL_RE = re.compile(r"[=()]")


def is_decl_line(line):
    stripped = line.strip()
    if not stripped:
        return False
    code = stripped.split("/*", 1)[0].split("//", 1)[0]
    if NOT_DECL_RE.search(code):
        return False
    return bool(DECL_LINE_RE.match(stripped))


def find_blocks(lines):
    """Yield (start, end) line-index ranges (end exclusive) of declaration
    blocks: runs of >=2 decl lines directly after a function's opening
    brace, which stands alone on its own line at column 0."""
    i = 0
    n = len(lines)
    while i < n:
        if FUNC_OPEN_RE.match(lines[i]):
            start = i + 1
            j = start
            while j < n and is_decl_line(lines[j]):
                j += 1
            if j - start >= 2:
                yield start, j
            i = j
        else:
            i += 1


def reformat_block(block_lines):
    indent = re.match(r"\s*", block_lines[0]).group(0)
    snippet = "void f(void)\n{\n" + "".join(block_lines) + "}\n"
    result = subprocess.run(
        ["clang-format", "-style=file:%s" % STYLE_PATH, "--assume-filename=x.c"],
        input=snippet,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        raise RuntimeError("clang-format failed: %s" % result.stderr)
    out_lines = result.stdout.splitlines(keepends=True)
    return out_lines[2 : 2 + len(block_lines)], indent


def whitespace_only_diff(before, after):
    if len(before) != len(after):
        return False
    return all(b.split() == a.split() for b, a in zip(before, after))


def is_single_aligned_group(formatted_lines):
    """Reject a block clang-format split into sub-groups because aligning
    the whole run would push a comment past the column limit -- that
    produces two inconsistently-padded groups, worse than the unaligned
    original. Only accept a result where every trailing comment lands in
    the same column."""
    columns = {line.index("/*") for line in formatted_lines if "/*" in line}
    return len(columns) <= 1


def process_file(filename, fix):
    lines = open(filename).readlines()
    original = list(lines)

    edits = []  # (start, end, formatted_lines)
    for start, end in find_blocks(lines):
        formatted, _ = reformat_block(lines[start:end])
        if (
            formatted != lines[start:end]
            and whitespace_only_diff(lines[start:end], formatted)
            and is_single_aligned_group(formatted)
        ):
            edits.append((start, end, formatted))

    if not fix:
        for start, end, _ in edits:
            print("%s:%d: realign declaration block" % (filename, start + 1))
        return len(edits)

    for start, end, formatted in reversed(edits):
        lines[start:end] = formatted
    if lines != original:
        open(filename, "w").writelines(lines)
    return len(edits)


def main(argv):
    fix = "--fix" in argv[1:]
    files = [a for a in argv[1:] if a != "--fix"] or glob.glob(DEFAULT_GLOB)
    total = 0
    for filename in sorted(files):
        total += process_file(filename, fix)
    verb = "realigned" if fix else "would realign"
    print("%s %d block(s)" % (verb, total))
    return 1 if (total and not fix) else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
