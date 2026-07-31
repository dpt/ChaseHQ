#!/usr/bin/env python3
"""Check that convert_stage.py still reads the bank skool files correctly.

The converter's output is a skeleton that needs hand-completion, so it can
never be diffed against the committed Stage{2-5}Data.c files wholesale. But
three properties are cheap to assert and are exactly the ones that broke
silently before:

1. Every MAP_* macro the converter emits is defined in Stages.h. The lane
   transition names carry the side of both endpoints (MAP_LANES_3RTO4, not
   MAP_LANES_3TO4R); when the converter's table drifted from the header the
   generated files stopped compiling, and the failure looked like a dozen
   unrelated payload differences.

2. Each stage's chatter array holds all four lines of Nancy's dialogue. Bank 1
   writes those strings and the pointers to them without T/W directives, so
   SkoolKit splits them into DEFM-plus-DEFB runs and DEFB pairs. A converter
   that does not rejoin them loses whole lines of dialogue and mispairs every
   pointer after the split -- which is what put stage 2's LOD entries against
   the wrong bitmaps.

3. No LOD entry's sprite overruns the array it points into. The graphics runs
   are cut into one array per sprite, and where two sprites overlap in the
   original data -- stage 5's $D86C is 1x5 = 5 bytes but the next sprite starts
   4 bytes later -- the cut has to be refused, because nothing guarantees how
   the compiler lays two separate arrays out. Indexing one shared blob absorbed
   that; an array per sprite does not.

4. Every sprite array is written in the shape it declares. A masked sprite
   interleaves a mask byte with each pixel byte, so a 2-wide one is 4 elements
   across; written at 2 per row each visual row is split over two lines and the
   array stops reading as a picture. Seven arrays in the hand-written
   Stage1Data.c were like that, and one of them declared a height its data did
   not fill.

Usage: python3 check_stage_converter.py
"""

import glob
import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
SCRIPT = os.path.join(HERE, "convert_stage.py")
SKOOL = os.path.join(ROOT, "Speccy")
DATA_DIR = os.path.join(ROOT, "C", "libraries", "ChaseHQ", "Data")
STAGES_H = os.path.join(DATA_DIR, "Stages.h")

# Stage -> (bank skool basename, --obj-names value). Mirrors the convert_stages
# target in C/CMakeLists.txt; keep the two in step.
STAGES = {
    2: ("ChaseHQ-128K-bank-1.skool",
        "NONE,TUNNEL_LIGHT,OBJ2,SHORT_POLE,HUGE_ROCK,PALM_TREE,LEAVES,"
        "DOUBLE_LAMP,OBJ8"),
    3: ("ChaseHQ-128K-bank-6.skool",
        "NONE,TUNNEL_LIGHT,OVERHEAD_BRIDGE,SHORT_POLE,TOWER_BLOCK,"
        "SPEED_LIMIT_SIGN,TELEGRAPH_POLE,OBJ7"),
    4: ("ChaseHQ-128K-bank-6.skool",
        "NONE,TUNNEL_LIGHT,OBJ2,SHORT_POLE,NEAR_COLUMN,FAR_COLUMN,"
        "PILE_OF_ROCKS,STREET_LAMP,TURN_SIGN_POINTING_LEFT,"
        "TURN_SIGN_POINTING_RIGHT"),
    5: ("ChaseHQ-128K-bank-7.skool",
        "NONE,TUNNEL_LIGHT,OVERHEAD_BRIDGE,OBJ3,CACTUS,DOUBLE_STREET_LAMP,"
        "HUGE_ROCK,TELEGRAPH_POLE"),
}

# Per-stage object macros are defined by the generated file itself, so only the
# shared vocabulary from Stages.h is checked.
SHARED_MACRO_RE = re.compile(r"\b(MAP_(?:LANES|CURVE|HEIGHT|CMD)_[A-Z0-9_]+)\(")


def generate(stage, tmpdir):
    skool, obj_names = STAGES[stage]
    out = subprocess.run(
        ["python3", SCRIPT, os.path.join(SKOOL, skool), str(stage),
         "--obj-names", obj_names],
        capture_output=True, text=True,
    )
    if out.returncode != 0:
        print("convert_stage.py failed for stage %d:\n%s" % (stage, out.stderr))
        sys.exit(1)
    return out.stdout


# A sprite array declares its shape ('6 * 31'), not a byte count, whenever the
# LOD entry describing it accounts for every byte of the block. Where the block
# holds trailing bytes no entry reaches, the shape is followed by '+ N' for them.
LENGTH = r"[0-9]+(?:\s*\*\s*[0-9]+)*(?:\s*\+\s*[0-9]+)?"
ARRAY_RE = re.compile(
    r"static const u8 (stage\d_bitmap_[0-9A-F]{4})\[(" + LENGTH + r")\] = \{"
)


def declared_size(expr):
    """Byte count a '6 * 2 * 31' or '2 * 2 * 2 + 4' array length declares."""
    total = 0
    for term in expr.split("+"):
        n = 1
        for factor in term.split("*"):
            n *= int(factor.strip())
        total += n
    return total


def sprite_extent(expr):
    """Bytes of `expr` that are the sprite itself, before any '+ N' remainder."""
    return declared_size(expr.split("+")[0])

# One bitmap_t entry: width, flags, height, then the bitmap and pre-shifted
# pointers, each an &array[index] into a sprite block.
ENTRY_RE = re.compile(
    r"\{ (\d+), BITMAPFLAG_(\w+), (\d+),"
    r" &(stage\d_bitmap_[0-9A-F]{4})\[(\d+)\],"
    r" &(stage\d_bitmap_[0-9A-F]{4})\[(\d+)\] \}"
)


def sprite_overruns(text):
    """Report LOD entries at odds with the array they point into.

    Two ways they can disagree. The sprite may reach past the end of the array,
    which is a real out-of-bounds read waiting to happen. Or the array may
    declare a shape -- a length written as a product -- that is not the shape the
    entry describes, in which case the shape is decoration rather than fact.
    """
    sizes, extents = {}, {}
    for m in ARRAY_RE.finditer(text):
        sizes[m.group(1)] = declared_size(m.group(2))
        # Only a length written as a product claims to be a shape.
        if "*" in m.group(2):
            extents[m.group(1)] = sprite_extent(m.group(2))
    out = []
    for m in ENTRY_RE.finditer(text):
        width, flag, height = int(m.group(1)), m.group(2), int(m.group(3))
        # A masked sprite interleaves a mask byte with each pixel byte. Other
        # flag combinations are not modelled, so they are left unchecked rather
        # than guessed at.
        if flag == "DEFAULT":
            need = width * height
        elif flag == "MASKED":
            need = width * height * 2
        else:
            continue
        for name, index in ((m.group(4), int(m.group(5))),
                            (m.group(6), int(m.group(7)))):
            if name in sizes and index + need > sizes[name]:
                out.append("%s[%d] + %d bytes (%dx%d %s) overruns [%d]"
                           % (name, index, need, width, height, flag, sizes[name]))
            elif index == 0 and extents.get(name, need) != need:
                out.append("%s declares a %d-byte sprite but its entry is "
                           "%dx%d %s = %d bytes"
                           % (name, extents[name], width, height, flag, need))
    return out


SHAPED_RE = re.compile(
    r"static const u8 (\w+)\[(" + LENGTH + r")\] = \{\n(.*?)\n\};", re.S
)
# A Pixels.h macro name or a hex literal: one token, one byte.
BYTE_TOKEN_RE = re.compile(r"(?:[X_]{8}|0[xX][0-9A-Fa-f]{2})$")


def shape_faults(text):
    """Report sprite arrays not written in the shape they declare.

    Every array is checked for holding exactly the bytes it declares. A short
    initialiser is zero-filled by C rather than rejected, so it goes unnoticed;
    an over-long one is only a warning.

    An array whose length is written as a product is checked for its row shape
    too. 'width * 2 * height' marks a masked sprite, whose mask byte is
    interleaved with each pixel byte: it is width * 2 elements across. Anything
    else is width across. A row count that disagrees means the array no longer
    reads as the picture it holds. Only the sprite's own rows are counted: a
    '+ N' remainder is unidentified data whose row width is not known.
    """
    out = []
    for m in SHAPED_RE.finditer(text):
        toks = [t.strip() for t in m.group(3).replace("\n", " ").split(",") if t.strip()]
        # One token must mean one byte for the count to be meaningful. Map and
        # arrest-message arrays are built from macros that each expand to
        # several bytes, so they are not comparable and are skipped.
        if not all(BYTE_TOKEN_RE.match(t) for t in toks):
            continue
        shape = m.group(2).split("+")[0]
        factors = [int(f.strip()) for f in shape.split("*")]
        per_row = factors[0] * 2 if len(factors) == 3 and factors[1] == 2 else factors[0]
        extent = sprite_extent(m.group(2))
        rows = [r for r in m.group(3).split("\n") if r.strip()]
        sprite_rows = rows[: extent // per_row] if per_row else []
        held = len(toks)
        if held != declared_size(m.group(2)):
            out.append("%s[%s] holds %d bytes, declares %d"
                       % (m.group(1), m.group(2), held, declared_size(m.group(2))))
        elif len(factors) > 1 and sum(
            len([t for t in r.split(",") if t.strip()]) for r in sprite_rows
        ) != extent:
            out.append("%s[%s] does not start with %d rows of %d"
                       % (m.group(1), m.group(2), extent // per_row, per_row))
    return out


def main():
    defined = set(re.findall(r"#define\s+(MAP_[A-Z0-9_]+)\(", open(STAGES_H).read()))
    failures = 0

    with tempfile.TemporaryDirectory() as tmpdir:
        for stage in sorted(STAGES):
            text = generate(stage, tmpdir)

            used = set(SHARED_MACRO_RE.findall(text))
            missing = sorted(used - defined)
            if missing:
                failures += 1
                print("stage %d: %d macro(s) not defined in Stages.h: %s"
                      % (stage, len(missing), ", ".join(missing)))

            overruns = sprite_overruns(text)
            if overruns:
                failures += 1
                print("stage %d: %d sprite(s) overrun their array:" % (stage, len(overruns)))
                for o in sorted(set(overruns)):
                    print("      %s" % o)

            m = re.search(
                r"stage%d_chatter_strings\[(\d+)\] = \{\n(.*?)\n\};" % stage,
                text, re.S,
            )
            if m is None:
                failures += 1
                print("stage %d: no chatter_strings array emitted" % stage)
                continue
            if int(m.group(1)) != 4:
                failures += 1
                print("stage %d: chatter_strings has %s entries, expected 4"
                      % (stage, m.group(1)))
            # Each line ends on the byte with bit 7 set, which renders as a \xNN
            # escape. A line that stops on a plain character was truncated where
            # the control file broke the string into DEFM plus DEFB runs.
            for line in re.findall(r'"((?:[^"\\]|\\.)*)"', m.group(2)):
                if not re.search(r"\\x[0-9A-F]{2}$", line):
                    failures += 1
                    print('stage %d: chatter line "%s" is unterminated -- '
                          "text run not stitched" % (stage, line[:24]))

    # The committed files are laid out the same way, and there the property is
    # load-bearing rather than advisory: they are what the game actually
    # compiles against.
    for path in sorted(glob.glob(os.path.join(DATA_DIR, "*.c"))):
        text = open(path).read()
        for label, found in (("overrun their array", sprite_overruns(text)),
                             ("disagree with their shape", shape_faults(text))):
            if not found:
                continue
            failures += 1
            print("%s: %d sprite(s) %s:"
                  % (os.path.basename(path), len(found), label))
            for o in sorted(set(found)):
                print("      %s" % o)

    if failures:
        print("\n%d check(s) failed." % failures)
        return 1
    print("PASS  convert_stage.py: macros resolve, chatter lines decode, "
          "sprites fit their arrays and are written in the shape they declare "
          "(generated and committed)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
