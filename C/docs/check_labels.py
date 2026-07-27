#!/usr/bin/env python3
"""Check skool @label= names against the C function names they were ported to.

Every translated C function carries a "$XXXX: ..." prologue naming its Z80
address. The matching control file carries "@ $XXXX label=name" for the same
address. Renaming the C function leaves the control file's label stale, which
is how 42 of them drifted before commit 3ceaeb3 fixed them by hand.

Usage: python3 check_labels.py [--fix]

Check-only by default; --fix rewrites the stale label= values in place. It
touches the @label lines only -- prose mentions of the old name elsewhere in
the control file still need reviewing by hand.
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))

# C source -> control file, both relative to the repository root.
PAIRS = [
    ("C/libraries/ChaseHQ/Engine/Main.c", "Speccy/ChaseHQ-128K.ctl"),
    ("C/libraries/ChaseHQ/Engine/Bank3.c", "Speccy/ChaseHQ-128K-bank-3.ctl"),
    ("C/libraries/ChaseHQ/Engine/Bank7.c", "Speccy/ChaseHQ-128K-bank-7.ctl"),
]

TITLE_RE = re.compile(r"^ \* \$([0-9A-Fa-f]{4}):")
# A definition starts in column 1; the name is the identifier before the "(".
DEFN_RE = re.compile(r"^[A-Za-z_][\w \t*]*?(\w+)\(")
# The leading "*" marks an entry point in SkoolKit syntax and is not part of
# the name, so it is captured separately and preserved when rewriting.
LABEL_RE = re.compile(r"^(@ \$([0-9A-Fa-f]{4}) label=)(\*?)(\S+)\s*$")


def c_functions(path):
    """Map upper-case Z80 address -> C function name, for one source file."""
    lines = open(path).read().splitlines()
    out = {}
    addr = None
    in_prologue = False
    for line in lines:
        if line.startswith("/**"):
            in_prologue = True
            addr = None
        elif in_prologue:
            m = TITLE_RE.match(line)
            if m:
                addr = m.group(1).upper()
            if line.strip().endswith("*/"):
                in_prologue = False
        elif addr:
            m = DEFN_RE.match(line)
            if m:
                out[addr] = m.group(1)
                addr = None
    return out


def check(cpath, ctlpath, fix):
    funcs = c_functions(os.path.join(ROOT, cpath))
    full = os.path.join(ROOT, ctlpath)
    lines = open(full).read().splitlines(True)
    # An address may carry several labels -- one per alternate entry point, and
    # some are symbols that ssub= expressions do arithmetic on. The C name only
    # has to match one of them, and rewriting the wrong one of a set silently
    # deletes a symbol the assembly still references, so multi-label addresses
    # are reported for review rather than fixed.
    at = {}
    for i, line in enumerate(lines):
        m = LABEL_RE.match(line)
        if m:
            at.setdefault(m.group(2).upper(), []).append((i, m))

    stale = []
    ambiguous = []
    for addr, hits in at.items():
        want = funcs.get(addr)
        if not want or any(m.group(4) == want for _, m in hits):
            continue
        if len(hits) > 1:
            ambiguous.append((addr, [m.group(4) for _, m in hits], want))
            continue
        i, m = hits[0]
        stale.append((addr, m.group(4), want))
        lines[i] = m.group(1) + m.group(3) + want + "\n"
    stale.sort()
    ambiguous.sort()
    missing = sorted(a for a in funcs if a not in at)

    for addr, old, want in stale:
        print("%s:$%s: label=%s should be %s" % (ctlpath, addr, old, want))
    for addr, olds, want in ambiguous:
        print(
            "%s:$%s: %d labels (%s), none is %s -- fix by hand"
            % (ctlpath, addr, len(olds), ", ".join(olds), want)
        )
    for addr in missing:
        print("%s:$%s: no label for C function %s" % (ctlpath, addr, funcs[addr]))
    if stale and fix:
        open(full, "w").writelines(lines)
        print("%s: rewrote %d label(s)" % (ctlpath, len(stale)))
    return len(stale)


def main():
    fix = "--fix" in sys.argv[1:]
    stale = 0
    for cpath, ctlpath in PAIRS:
        stale += check(cpath, ctlpath, fix)
    if stale and not fix:
        print("\n%d stale label(s); rerun with --fix to rewrite them." % stale)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
