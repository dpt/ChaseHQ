#!/usr/bin/env python3
"""Reformat function signatures (declarations and definitions) to match the
house style enforced by C/.clang-format (PointerAlignment: Right,
AlignAfterOpenBracket: Align, BinPackParameters: false — one parameter per
wrapped line, aligned under the opening parenthesis).

Each top-level function signature is reformatted in isolation by feeding it
through clang-format with the project's .clang-format, then the result is
spliced back into the file. Everything else (prologues, bodies, macros,
typedefs) is left untouched.

The script only ever *adds* line breaks. A signature the author stacked one
parameter per line stays stacked even when it would now fit on one line, per
"Try to align parameters horizontally" in
docs/function_comment_template_example.c -- the worked example there is a
69-column signature deliberately split across three lines. clang-format,
judging the snippet in isolation, would join it; that is a layout choice the
author already made and this script must not undo. Nor may it re-pack a
stacked signature into clang-format's
AllowAllParametersOfDeclarationOnNextLine form, which is the same loss of
alignment by a different route.

Usage: python3 format_signatures.py [--fix] [file.c ...]
Default: all *.c in libraries/ChaseHQ/Engine/, check-only unless --fix given.
Running it over docs/function_comment_template_example.c must report no
changes: the template is the specification this script is measured against.
"""

import glob
import os
import re
import subprocess
import sys

DEFAULT_GLOB = "libraries/ChaseHQ/Engine/*.c"
STYLE_PATH = os.path.join(os.path.dirname(__file__), "..", ".clang-format")

# Return type and name on the same physical line (the normal case).
HEADER_START_RE = re.compile(r"^[A-Za-z_][^\n;{}]*?\(", re.MULTILINE)

# Return type alone on one line, name+'(' starting the next (a leftover from
# a mis-tuned clang-format run that broke after the return type -- see
# PenaltyReturnTypeOnItsOwnLine in .clang-format). Matched separately so
# such stragglers get rejoined rather than silently ignored.
BROKEN_RETURN_RE = re.compile(r"^[A-Za-z_][\w \*]*[\w\*]\n[A-Za-z_]\w*\(", re.MULTILINE)


def find_matching_paren(text, open_idx):
    depth = 0
    for i in range(open_idx, len(text)):
        if text[i] == "(":
            depth += 1
        elif text[i] == ")":
            depth -= 1
            if depth == 0:
                return i
    return -1


def find_signatures(text):
    """Yield (start, end, is_definition) for each top-level function
    signature. start/end span from the first character of the return type
    through the closing ')' (exclusive of the trailing ';' or '{')."""
    starts = set()
    matches = list(HEADER_START_RE.finditer(text)) + list(
        BROKEN_RETURN_RE.finditer(text)
    )
    matches.sort(key=lambda m: m.start())
    for m in matches:
        start = m.start()
        if start in starts:
            continue
        starts.add(start)
        open_idx = m.end() - 1
        header = text[start:open_idx]
        if not re.search(r"\s", header.strip()):
            continue  # bare "NAME(" -- a macro call, not a declaration
        if "typedef" in header:
            continue
        if "=" in header:
            continue
        close_idx = find_matching_paren(text, open_idx)
        if close_idx == -1:
            continue
        rest = text[close_idx + 1 :].lstrip()
        if rest.startswith(";"):
            yield start, close_idx + 1, False
        elif rest.startswith("{"):
            yield start, close_idx + 1, True


def reformat_signature(original, is_definition):
    snippet = original + (";\n" if not is_definition else "\n{\n}\n")
    result = subprocess.run(
        ["clang-format", "-style=file:%s" % STYLE_PATH, "--assume-filename=x.c"],
        input=snippet,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        raise RuntimeError("clang-format failed: %s" % result.stderr)
    marker = ";" if not is_definition else "{"
    formatted = result.stdout[: result.stdout.rindex(marker)]
    return formatted.rstrip("\n")


def process_file(filename, fix):
    text = open(filename).read()
    edits = []  # (start, end, replacement), in file order
    for start, end, is_definition in find_signatures(text):
        original = text[start:end]
        formatted = reformat_signature(original, is_definition)
        if formatted.count("\n") < original.count("\n"):
            continue  # would unstack a deliberate layout -- see module docstring
        if formatted != original:
            lineno = text.count("\n", 0, start) + 1
            edits.append((start, end, formatted, lineno))

    if not fix:
        for start, end, formatted, lineno in edits:
            print("%s:%d: reformat" % (filename, lineno))
        return len(edits)

    for start, end, formatted, lineno in reversed(edits):
        text = text[:start] + formatted + text[end:]
    if edits:
        open(filename, "w").write(text)
    return len(edits)


def main(argv):
    fix = "--fix" in argv[1:]
    files = [a for a in argv[1:] if a != "--fix"] or glob.glob(DEFAULT_GLOB)
    total = 0
    for filename in sorted(files):
        total += process_file(filename, fix)
    verb = "reformatted" if fix else "would reformat"
    print("%s %d signature(s)" % (verb, total))
    return 1 if (total and not fix) else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
