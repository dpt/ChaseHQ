#!/usr/bin/env python3
r"""Reformat function signatures (declarations and definitions) to match the
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

The prologue \param blocks are reflowed too, per "Try to align parameters
horizontally" in the same template: within one block the parameter names line
up in a column and the descriptions line up in a column after them, each
padded by a single space beyond the widest entry. Descriptions are re-wrapped
to 80 columns with continuation lines indented to the description column. A
block whose description column would land beyond DESC_COLUMN_LIMIT is packed
with single spaces instead, since aligning it would leave too little width
for the text. Blank " *" lines immediately before a prologue's closing "*/"
are dropped.

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
import textwrap

DEFAULT_GLOB = "libraries/ChaseHQ/Engine/*.c"
STYLE_PATH = os.path.join(os.path.dirname(__file__), "..", ".clang-format")

LINE_LIMIT = 80
# Beyond this the aligned description column leaves too little room for text,
# so the block falls back to single-space packing.
DESC_COLUMN_LIMIT = 40

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


PROLOGUE_RE = re.compile(r"/\*\*.*?\*/", re.DOTALL)
PARAM_RE = re.compile(r"^(\s*)\* (\\param(?:\[[^\]]*\])?)\s+(\S+)\s*(.*)$")
CONTINUATION_RE = re.compile(r"^\s*\*\s+(\S.*?)\s*$")
TAG_RE = re.compile(r"^\s*\* \\")
BLANK_COMMENT_RE = re.compile(r"^\s*\*\s*$")
CLOSE_RE = re.compile(r"^\s*\*/\s*$")


NBSP = " "
# "(was HL')" and friends read as one token; never break one across lines.
# Length-capped so a note that has grown into a sentence -- "(was A after SUB
# $20; ...)" -- still wraps normally instead of becoming an unbreakable run.
REGISTER_NOTE_RE = re.compile(r"\((?:was|were) [^)]{1,16}\)")


def protect_register_notes(description):
    return REGISTER_NOTE_RE.sub(lambda m: m.group(0).replace(" ", NBSP), description)


def render_params(entries, prefix):
    """Lay out one contiguous run of \\param entries as a list of lines.

    entries is a list of (tag, name, description) triples; prefix is the
    " * " that opens each line of the enclosing comment block."""
    lines = []
    tag_width = max(len(tag) for tag, _, _ in entries)
    name_width = max(len(name) for _, name, _ in entries)
    desc_column = len(prefix) + tag_width + 1 + name_width + 1
    if desc_column > DESC_COLUMN_LIMIT:
        tag_width = 0
        name_width = 0
        desc_column = len(prefix) + 2
    for tag, name, description in entries:
        head = prefix + tag.ljust(tag_width) + " " + name.ljust(name_width) + " "
        wrapped = textwrap.wrap(
            protect_register_notes(description),
            width=LINE_LIMIT,
            initial_indent=head,
            subsequent_indent=prefix + " " * (desc_column - len(prefix)),
        )
        lines.extend(line.replace(NBSP, " ") for line in wrapped or [head.rstrip()])
    return lines


def reformat_prologue(block):
    """Reflow every \\param run in one /** ... */ comment block."""
    lines = block.split("\n")
    out = []
    i = 0
    while i < len(lines):
        m = PARAM_RE.match(lines[i])
        if m is None:
            out.append(lines[i])
            i += 1
            continue
        prefix = m.group(1) + "* "
        entries = []
        while i < len(lines):
            m = PARAM_RE.match(lines[i])
            if m is None:
                break
            description = [m.group(4)]
            i += 1
            # Absorb the entry's continuation lines: indented text that is
            # neither a new tag nor the closing "*/".
            while i < len(lines) and not TAG_RE.match(lines[i]):
                c = CONTINUATION_RE.match(lines[i])
                if c is None:
                    break
                description.append(c.group(1))
                i += 1
            entries.append((m.group(2), m.group(3), " ".join(description).strip()))
        out.extend(render_params(entries, prefix))
    # A prologue that trails off into blank " *" lines before its closing
    # "*/" gains nothing from them.
    while len(out) > 1 and CLOSE_RE.match(out[-1]) and BLANK_COMMENT_RE.match(out[-2]):
        del out[-2]
    return "\n".join(out)


def reformat_prologues(text):
    """Reflow every prologue in the file. Returns (text, changed linenos)."""
    linenos = []
    out = []
    pos = 0
    for m in PROLOGUE_RE.finditer(text):
        formatted = reformat_prologue(m.group(0))
        if formatted != m.group(0):
            linenos.append(text.count("\n", 0, m.start()) + 1)
        out.append(text[pos : m.start()])
        out.append(formatted)
        pos = m.end()
    out.append(text[pos:])
    return "".join(out), linenos


def process_file(filename, fix):
    text = open(filename).read()
    original_text = text
    text, prologue_linenos = reformat_prologues(text)

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
        for lineno in prologue_linenos:
            print("%s:%d: reflow \\param block" % (filename, lineno))
        for start, end, formatted, lineno in edits:
            print("%s:%d: reformat" % (filename, lineno))
        return len(edits) + len(prologue_linenos)

    for start, end, formatted, lineno in reversed(edits):
        text = text[:start] + formatted + text[end:]
    if text != original_text:
        open(filename, "w").write(text)
    return len(edits) + len(prologue_linenos)


def main(argv):
    fix = "--fix" in argv[1:]
    files = [a for a in argv[1:] if a != "--fix"] or glob.glob(DEFAULT_GLOB)
    total = 0
    for filename in sorted(files):
        total += process_file(filename, fix)
    verb = "reformatted" if fix else "would reformat"
    print("%s %d item(s)" % (verb, total))
    return 1 if (total and not fix) else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
