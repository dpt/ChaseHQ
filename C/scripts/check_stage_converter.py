#!/usr/bin/env python3
"""Check that convert_stage.py still reads the bank skool files correctly.

The converter's output is a skeleton that needs hand-completion, so it can
never be diffed against the committed Stage{2-5}Data.c files wholesale. But two
properties are cheap to assert and are exactly the ones that broke silently
before:

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

Usage: python3 check_stage_converter.py
"""

import os
import re
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
SCRIPT = os.path.join(ROOT, "Speccy", "scripts", "convert_stage.py")
SKOOL = os.path.join(ROOT, "Speccy")
STAGES_H = os.path.join(
    ROOT, "C", "libraries", "ChaseHQ", "Data", "Stages.h"
)

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

    if failures:
        print("\n%d check(s) failed." % failures)
        return 1
    print("PASS  convert_stage.py: macros resolve and all chatter lines decode")
    return 0


if __name__ == "__main__":
    sys.exit(main())
