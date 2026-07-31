#!/usr/bin/env python3
"""
map_compile.py

Compiles a Chase H.Q. ".map" text file into C stage map data, and decompiles
committed Stage*Data.c map arrays back into that text format.

A stage's road layout lives as six parallel RLE byte streams -- curvature,
height, lanes, hazards, leftobjs and rightobjs -- written as MAP_* macros in
C/libraries/ChaseHQ/Data/Stage{1-6}Data.c. They are readable but not
visualisable: nothing in the source shows that a 4-lane road narrows on the
right while climbing into a left-hand bend.

The ".map" format is a table where one row is one slice of road and the six
streams are columns. See C/docs/map-text-format.md for the full specification.

Usage:
  python3 map_compile.py compile   <file.map>              > fragment.c
  python3 map_compile.py decompile <StageNData.c>          > stageN.map
  python3 map_compile.py --selftest

Options:
  --allow-desync   Tolerate sections whose streams violate the 2:1 rate rule
                   (see below), padding the short stream. Two sections in the
                   committed data need this.

The 2:1 rate rule
-----------------
rm_cycle_buffer_offset (Main.c) toggles session.no_objects_flag between 1 and
2 on each call and advances the hazards/leftobjs/rightobjs streams only on the
"== 1" pass. Those three therefore tick at HALF the rate of
curvature/height/lanes, and a well-formed section satisfies

    curvature == height == lanes == 2 * hazards == 2 * leftobjs == 2 * rightobjs

Enforcing that is the main reason this tool exists.
"""

import os
import re
import sys
import argparse
from typing import Dict, List, Optional, Tuple

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# The encoding tables are shared with convert_stage.py rather than restated
# here: CLAUDE.md requires they match Stages.h exactly, so there must be only
# one copy. convert_stage.py has an "if __name__" guard, so importing is safe.
from convert_stage import (  # noqa: E402
    CURVE_TYPES,
    HEIGHT_TYPES,
    LANE_VALS,
    HAZARD_CMDS,
)

REPO_ROOT = os.path.normpath(
    os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..")
)
DATA_DIR = os.path.join(REPO_ROOT, "C", "libraries", "ChaseHQ", "Data")

# ── Errors ───────────────────────────────────────────────────────────────────


class MapError(Exception):
    """A user-facing diagnostic. Carries no traceback to the terminal."""


# ── Streams ──────────────────────────────────────────────────────────────────

# Canonical stream order. The first three are full rate (one byte's worth of
# run per road unit), the last three half rate (one per two road units).
STREAMS = ["curvature", "height", "lanes", "hazards", "leftobjs", "rightobjs"]
FULL_RATE = STREAMS[:3]
HALF_RATE = STREAMS[3:]

# Suffixes seen in the committed data. Stage 1 spells them out; stages 2-6 use
# abbreviations, and Stage6Data.c drops the prefix entirely.
STREAM_ALIASES = {
    "curvature": "curvature",
    "curv": "curvature",
    "height": "height",
    "lanes": "lanes",
    "hazards": "hazards",
    "leftobjs": "leftobjs",
    "lobjs": "leftobjs",
    "leftside": "leftobjs",
    "rightobjs": "rightobjs",
    "robjs": "rightobjs",
    "rightside": "rightobjs",
}

# ── Curvature / height glyphs ────────────────────────────────────────────────

CURVE_GLYPHS = {
    "<<<": "LEFT_VERY_HARD",
    "<<": "LEFT_HARD",
    "<": "LEFT",
    ":": "STRAIGHT",
    ">": "RIGHT",
    ">>": "RIGHT_HARD",
    ">>>": "RIGHT_VERY_HARD",
}

HEIGHT_GLYPHS = {
    "^^^^": "UP7",
    "^^^": "UP5",
    "^^": "UP3",
    "^": "UP1",
    "-": "LEVEL",
    "v": "DOWN1",
    "vv": "DOWN3",
    "vvv": "DOWN5",
    "vvvv": "DOWN7",
}

CURVE_NAME_TO_CODE = {name: code for code, name in CURVE_TYPES.items()}
HEIGHT_NAME_TO_CODE = {name: code for code, name in HEIGHT_TYPES.items()}
CURVE_NAME_TO_GLYPH = {name: g for g, name in CURVE_GLYPHS.items()}
HEIGHT_NAME_TO_GLYPH = {name: g for g, name in HEIGHT_GLYPHS.items()}

# ── Lane canvas ──────────────────────────────────────────────────────────────
#
# Geometry is derived from draw_road at $C4AD (Main.c, "read lanes"):
#
#     left_rail  = 0xE7 + (lanes_byte & MAP_LANES_LEFT_OFFSET_MASK)
#     right_rail = left_rail + 3   if bit 7 set (3-lane family)
#                  left_rail + 2   if bit 7 clear (2-lane family)
#
# with the 4-lane case special-cased in dr_four_lane_highway to $E8/$EC, and
# tunnels forcing the right rail to $EB. Pages $E8..$EC are the five lane
# boundaries 0..4, so each lane state is an interval [left, right] over 0..4.
#
# The canvas prints rail k at column 2k: edge glyph at the two outer rails,
# ':' lane divider at each interior rail, space elsewhere.

CANVAS_WIDTH = 9

# name -> (left rail, right rail, edge glyph)
LANE_GEOMETRY = {
    "4": (0, 4, "H"),
    "3L": (0, 3, "H"),
    "3R": (1, 4, "H"),
    "2L": (0, 2, "H"),
    "2M": (1, 3, "H"),
    "2R": (2, 4, "H"),
    "TUNNEL_ENTRY": (0, 3, "#"),
    "TUNNEL_EXIT": (0, 3, "#"),
    "DIRTTRACK": (0, 4, "~"),
}

# Steady states an author may write directly. TUNNEL_ENTRY/EXIT share a canvas
# and are chosen by the tunnel entry/exit rule, not written apart.
TUNNEL_NAME = "TUNNEL_ENTRY"

# (from, to) -> macro suffix. These nine are the only defined pairs.
#
# Transitions are written out in full ("4>3R"), not inferred from the
# neighbouring steady states. The committed data rules inference out: stage 1's
# loop section has MAP_LANES_4TO3R(34) immediately followed by
# MAP_LANES_3RTO4(2), so a transition's neighbour is not always a steady state,
# and transitions are not always 2 units long.
TRANSITIONS = {
    ("4", "3L"): "4TO3L",
    ("4", "3R"): "4TO3R",
    ("3L", "4"): "3LTO4",
    ("3R", "4"): "3RTO4",
    ("3L", "2M"): "3LTO2M",
    ("3R", "2R"): "3RTO2R",
    ("3L", "2L"): "3LTO2L",
    ("2L", "3L"): "2LTO3L",
    ("2R", "3R"): "2RTO3R",
}

LANE_NAME_TO_VAL = {name: val for val, name in LANE_VALS.items()}

# "4>3R" <-> "4TO3R"
TRANSITION_TOKENS = {"%s>%s" % pair: macro for pair, macro in TRANSITIONS.items()}
TRANSITION_TO_TOKEN = {macro: token for token, macro in TRANSITION_TOKENS.items()}


def build_canvas(name: str) -> str:
    """Render a lane state as its CANVAS_WIDTH-character road canvas."""
    left, right, edge = LANE_GEOMETRY[name]
    cols = [" "] * CANVAS_WIDTH
    cols[2 * left] = edge
    cols[2 * right] = edge
    for rail in range(left + 1, right):
        cols[2 * rail] = ":"
    return "".join(cols).rstrip()


def build_transition_canvas(source: str, dest: str) -> str:
    """Render a lane transition as a canvas whose moving rail is a ramp.

    The table reads bottom to top -- the start of the road is the last row --
    so a rail whose column grows as the car drives on slants '/', and one whose
    column shrinks slants '\\'. The two ramp characters sit on the outer side of
    the rail's travel, so '4' -> '3R' (left rail 0 -> 1) draws "//  : : H".
    """
    source_left, source_right, _edge = LANE_GEOMETRY[source]
    dest_left, dest_right, edge = LANE_GEOMETRY[dest]
    cols = [" "] * CANVAS_WIDTH
    for rail in range(dest_left + 1, dest_right):
        cols[2 * rail] = ":"
    if source_left == dest_left:
        cols[2 * dest_left] = edge
    else:
        outer = 2 * min(source_left, dest_left)
        cols[outer] = cols[outer + 1] = "/" if dest_left > source_left else "\\"
    if source_right == dest_right:
        cols[2 * dest_right] = edge
    else:
        outer = 2 * max(source_right, dest_right)
        cols[outer - 1] = cols[outer] = "/" if dest_right > source_right else "\\"
    return "".join(cols).rstrip()


CANVAS_TO_LANE = {}
for _n in LANE_GEOMETRY:
    if _n == "TUNNEL_EXIT":
        continue  # shares TUNNEL_ENTRY's canvas
    CANVAS_TO_LANE[build_canvas(_n)] = _n

# canvas -> macro suffix, e.g. "//  : : H" -> "4TO3R"
TRANSITION_CANVASES = {
    build_transition_canvas(a, b): macro for (a, b), macro in TRANSITIONS.items()
}
TRANSITION_TO_CANVAS = {macro: c for c, macro in TRANSITION_CANVASES.items()}
assert len(TRANSITION_CANVASES) == len(TRANSITIONS), "transition canvases collide"

# A written-out alternative to the canvas, for hand-editing. The canvas indent
# distinguishes 3R from 3L and 2M/2R from 2L, and is easy to lose by accident.
LANE_NAME_ALIASES = {
    "4": "4", "3L": "3L", "3R": "3R", "2L": "2L", "2M": "2M", "2R": "2R",
    "TUNNEL": TUNNEL_NAME, "DIRT": "DIRTTRACK", "DIRTTRACK": "DIRTTRACK",
}

# ── Hazards ──────────────────────────────────────────────────────────────────
#
# The engine (Main.c, "$BFE7: HAZARDS") folds commands 3..9 into a single
# latched value, rm_hazard_byte = cmd - 3, written to the road buffer until the
# next such command. Commands 10..14 and 15+ are edge triggered: they set
# floating_arrow / dont_spawn_cars / helicopter_control and jump back to read
# another byte without consuming a wait.

HAZARD_TOKENS = {
    "haz-": 3,   # latch 0 (MAP_CMD_STOP_BARRIERS)
    "haz1": 4,   # latch 1 (MAP_CMD_START_OBSTACLE_L)
    "haz2": 5,   # latch 2 (MAP_CMD_START_OBSTACLE_R)
    "haz3": 6,   # latch 3 (MAP_CMD_START_TWO_OBSTACLES)
    "bar<": 7,
    "bar>": 8,
    "bar<>": 9,
    "arrX": 10,
    "arr<": 11,
    "arr>": 12,
    "cars+": 13,
    "cars-": 14,
    # helicopter_control state = code - 11
    "heli1": 15,  # MAP_CMD_HELI_LEAVE
    "heli2": 16,  # state 2, "departing" — never written by a stage
    "heli3": 17,  # MAP_CMD_HELI_TURN_L
    "heli4": 18,  # MAP_CMD_HELI_TURN_R
}

HAZARD_CODE_TO_TOKEN = {code: tok for tok, code in HAZARD_TOKENS.items()}

# Commands that re-enter the read loop instead of consuming a wait, and so may
# legally appear back to back at the same offset.
HAZARD_RESTART_CODES = set(range(10, 256))

def hazard_macro(code: int) -> str:
    """C text for a hazard command byte, preferring the named macro."""
    named = HAZARD_CMDS.get(code)
    if named and named.startswith("MAP_CMD_"):
        return named
    return "MAP_ESC, (%d)" % code


# ── Encoding ceilings ────────────────────────────────────────────────────────

NIBBLE_MAX = 15   # curvature, height, objects: count packed into the high nibble
BYTE_MAX = 255    # lanes and hazard waits: count is a whole byte


def split_run(total: int, ceiling: int) -> List[int]:
    """Split a run length into chunks no larger than an encoding's ceiling."""
    if total <= 0:
        raise MapError("run length must be positive, got %d" % total)
    chunks = []
    while total > ceiling:
        chunks.append(ceiling)
        total -= ceiling
    chunks.append(total)
    return chunks


def runs_of(seq: List) -> List[Tuple[object, int]]:
    """Run-length encode a list into (value, count) pairs."""
    out = []
    for value in seq:
        if out and out[-1][0] == value:
            out[-1] = (value, out[-1][1] + 1)
        else:
            out.append((value, 1))
    return out


# ── Intermediate representation ──────────────────────────────────────────────


class Section:
    """One map section: six streams that advance in lockstep.

    Full-rate streams hold one entry per road unit; half-rate streams hold one
    entry per two road units. `hazards` is instead a list of
    (half_unit_offset, command_code) events plus a total length, because the
    stream encodes commands and waits rather than a value per step.
    """

    def __init__(self, name: str):
        self.name = name
        self.curvature: List[str] = []   # curve type names
        self.height: List[str] = []      # height type names
        self.lanes: List[int] = []       # raw lane byte values
        self.leftobjs: List[int] = []    # object nibbles, half-rate
        self.rightobjs: List[int] = []
        self.hazards: List[Tuple[int, int]] = []  # (half offset, cmd code)
        self.hazard_len = 0              # half-units
        self.terminator = None           # ("goto", label) | ("split", a, b) | ("end",)
        self.addrs: Dict[str, int] = {}  # stream -> pinned Z80 address
        self.flow = None                 # resolved terminator, section names
        self.desynced = False            # padded to satisfy the 2:1 rate rule

    def units(self) -> int:
        return len(self.curvature)

    def stream_lengths(self) -> Dict[str, int]:
        return {
            "curvature": len(self.curvature),
            "height": len(self.height),
            "lanes": len(self.lanes),
            "hazards": self.hazard_len,
            "leftobjs": len(self.leftobjs),
            "rightobjs": len(self.rightobjs),
        }

    def check_sync(self, allow_desync: bool, warn) -> None:
        """Enforce the 2:1 rate rule, padding short streams if permitted."""
        lengths = self.stream_lengths()
        units = max(lengths["curvature"], lengths["height"], lengths["lanes"])
        expected = {s: units for s in FULL_RATE}
        expected.update({s: units // 2 for s in HALF_RATE})

        bad = [s for s in STREAMS if lengths[s] != expected[s]]
        if not bad:
            return

        detail = ", ".join("%s=%d" % (s, lengths[s]) for s in STREAMS)
        message = (
            "section '%s' violates the 2:1 rate rule "
            "(curvature == height == lanes == 2 x hazards == 2 x leftobjs "
            "== 2 x rightobjs).\n  lengths: %s\n  expected: %s"
            % (
                self.name,
                detail,
                ", ".join("%s=%d" % (s, expected[s]) for s in STREAMS),
            )
        )
        if not allow_desync:
            raise MapError(message + "\n  Re-run with --allow-desync to pad.")

        warn(message + "\n  --allow-desync: padding short streams.")
        self.desynced = True
        for stream in ("curvature", "height"):
            seq = getattr(self, stream)
            while len(seq) < expected[stream]:
                seq.append(seq[-1] if seq else "LEVEL")
            del seq[expected[stream]:]
        while len(self.lanes) < expected["lanes"]:
            self.lanes.append(self.lanes[-1] if self.lanes else 0x00)
        del self.lanes[expected["lanes"]:]
        for stream in ("leftobjs", "rightobjs"):
            seq = getattr(self, stream)
            while len(seq) < expected[stream]:
                seq.append(0)
            del seq[expected[stream]:]
        self.hazard_len = expected["hazards"]
        self.hazards = [e for e in self.hazards if e[0] < self.hazard_len]


# ── C array tokenizer ────────────────────────────────────────────────────────

ARRAY_RE = re.compile(
    r"static\s+const\s+u8\s+(\w+)\s*\[\s*\d*\s*\]\s*=\s*\{(.*?)\n\};",
    re.S,
)
DEFINE_RE = re.compile(r"#define\s+(\w*_ADDR)\s*\(?(0x[0-9A-Fa-f]+)\)?")
OBJ_DEF_RE = re.compile(r"#define\s+MAP_OBJ_S(\d+)_(\w+)_VAL\s*\(?(\d+)\)?")
GOTO_TABLE_RE = re.compile(
    r"static\s+const\s+struct\s*\{[^}]*\}\s*\w*_map_goto_table\s*\[\s*\]\s*=\s*\{(.*?)\n\};",
    re.S,
)
GOTO_ROW_RE = re.compile(r"\{\s*([\w+ ]+?)\s*,\s*&(\w+)\s*\[")

# One macro call, a bare MAP_ESC pair, or a bare integer.
TOKEN_RE = re.compile(
    r"(MAP_[A-Z0-9_]+)\s*(?:\(\s*([^)]*)\s*\))?"
    r"|(0[xX][0-9A-Fa-f]+|\d+)"
)


def strip_comments(text: str) -> str:
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    text = re.sub(r"//[^\n]*", " ", text)
    return text


class Item:
    """One decoded element of a map array."""

    def __init__(self, kind: str, name: str = "", count: int = 0, args=None):
        self.kind = kind    # "run" | "cmd" | "goto" | "split" | "end" | "raw"
        self.name = name
        self.count = count
        self.args = args or []

    def __repr__(self):
        return "Item(%s,%s,%s,%s)" % (self.kind, self.name, self.count, self.args)


def tokenize_array(body: str, array_name: str) -> List[Item]:
    """Parse a map array body into an ordered list of Items."""
    body = strip_comments(body)
    items: List[Item] = []
    pending_esc = False

    for match in TOKEN_RE.finditer(body):
        macro, arg, number = match.group(1), match.group(2), match.group(3)

        if number is not None:
            value = int(number, 0)
            if pending_esc:
                pending_esc = False
                items.append(Item("cmd", count=value))
            else:
                # A bare pair such as the "2, 0x3D" unknown-lanes escape in
                # Stage2Data.c. Stash it; the lanes decoder pairs it up.
                items.append(Item("raw", count=value))
            continue

        if macro == "MAP_ESC":
            pending_esc = True
            continue

        if pending_esc:
            raise MapError("%s: MAP_ESC followed by %s" % (array_name, macro))

        if macro == "MAP_CMD_GOTO":
            items.append(Item("goto", args=[arg.strip()]))
        elif macro == "MAP_CMD_SPLIT":
            parts = [p.strip() for p in arg.split(",")]
            if len(parts) != 2:
                raise MapError("%s: MAP_CMD_SPLIT needs 2 arguments" % array_name)
            items.append(Item("split", args=parts))
        elif macro == "MAP_CMD_FORK_END":
            items.append(Item("end"))
        elif macro.startswith("MAP_CMD_"):
            name = macro[len("MAP_CMD_"):]
            code = next(
                (c for c, m in HAZARD_CMDS.items() if m == macro), None
            )
            if code is None:
                raise MapError("%s: unknown command macro %s" % (array_name, macro))
            items.append(Item("cmd", name=name, count=code))
        elif macro.startswith("MAP_CURVE_"):
            items.append(Item("run", macro[len("MAP_CURVE_"):], int(arg)))
        elif macro.startswith("MAP_HEIGHT_"):
            items.append(Item("run", macro[len("MAP_HEIGHT_"):], int(arg)))
        elif macro.startswith("MAP_LANES_"):
            items.append(Item("run", macro[len("MAP_LANES_"):], int(arg)))
        elif macro.startswith("MAP_HAZARD_WAIT"):
            items.append(Item("run", "WAIT", int(arg)))
        elif macro.startswith("MAP_OBJ_S"):
            name = re.sub(r"^MAP_OBJ_S\d+_", "", macro)
            items.append(Item("run", name, int(arg)))
        else:
            raise MapError("%s: unhandled macro %s" % (array_name, macro))

    if pending_esc:
        raise MapError("%s: trailing MAP_ESC" % array_name)
    return items


# ── Expansion: Items -> per-unit sequences ───────────────────────────────────


def expand_curvature(items: List[Item], name: str) -> Tuple[List[str], object]:
    out, term = [], None
    for item in items:
        if item.kind == "run":
            if item.name not in CURVE_NAME_TO_CODE:
                raise MapError("%s: unknown curvature type %s" % (name, item.name))
            out.extend([item.name] * item.count)
        else:
            term = item
            break
    return out, term


def expand_height(items: List[Item], name: str) -> Tuple[List[str], object]:
    out, term = [], None
    for item in items:
        if item.kind == "run":
            if item.name not in HEIGHT_NAME_TO_CODE:
                raise MapError("%s: unknown height type %s" % (name, item.name))
            out.extend([item.name] * item.count)
        else:
            term = item
            break
    return out, term


def expand_lanes(items: List[Item], name: str) -> Tuple[List[int], object]:
    """Expand a lanes stream, folding TUNNEL_EXIT back into TUNNEL_ENTRY.

    A tunnel is always written entry(N-2), exit(2); collapsing the two to one
    value makes the whole tunnel a single run, which encode_lanes splits apart
    again. Left as two values they would re-encode as entry(N-2-2), exit(2).
    """
    entry = LANE_NAME_TO_VAL["TUNNEL_ENTRY"]
    exit_ = LANE_NAME_TO_VAL["TUNNEL_EXIT"]
    out, term = [], None
    index = 0
    while index < len(items):
        item = items[index]
        if item.kind == "run":
            if item.name not in LANE_NAME_TO_VAL:
                raise MapError("%s: unknown lanes type %s" % (name, item.name))
            lane_value = LANE_NAME_TO_VAL[item.name]
            out.extend([entry if lane_value == exit_ else lane_value] * item.count)
            index += 1
        elif item.kind == "raw":
            # count, value pair for a lane byte with no named macro
            if index + 1 >= len(items) or items[index + 1].kind != "raw":
                raise MapError("%s: dangling raw lanes byte" % name)
            out.extend([items[index + 1].count] * item.count)
            index += 2
        else:
            term = item
            break
    return out, term


def expand_objects(items: List[Item], name: str, obj_vals: Dict[str, int]):
    out, term = [], None
    for item in items:
        if item.kind == "run":
            if item.name not in obj_vals:
                raise MapError("%s: unknown object %s" % (name, item.name))
            out.extend([obj_vals[item.name]] * item.count)
        else:
            term = item
            break
    return out, term


def expand_hazards(items: List[Item], name: str):
    """Return (events, total_half_units, terminator)."""
    events: List[Tuple[int, int]] = []
    offset, term = 0, None
    for item in items:
        if item.kind == "run":
            offset += item.count
        elif item.kind == "cmd":
            events.append((offset, item.count))
        else:
            term = item
            break
    return events, offset, term


# ── Reading a Stage*Data.c file ──────────────────────────────────────────────


class StageFile:
    def __init__(self, path: str):
        self.path = path
        self.text = open(path).read()
        self.stage = self._detect_stage()
        self.obj_names, self.obj_vals = self._read_objects()
        self.addr_defines = self._read_addr_defines()
        self.arrays = self._read_arrays()
        self.goto_table = self._read_goto_table()

    def _detect_stage(self) -> int:
        match = re.search(r"Stage(\d)Data\.c", os.path.basename(self.path))
        if match:
            return int(match.group(1))
        match = re.search(r"MAP_OBJ_S(\d)_", self.text)
        if not match:
            raise MapError("%s: cannot determine stage number" % self.path)
        return int(match.group(1))

    def _read_objects(self):
        """value -> NAME and NAME -> value, from the stage file or Stages.h."""
        names, vals = {0: "NONE"}, {"NONE": 0}
        sources = [self.text]
        stages_h = os.path.join(os.path.dirname(self.path), "Stages.h")
        if os.path.exists(stages_h):
            sources.append(open(stages_h).read())
        for source in sources:
            for stage, name, value in OBJ_DEF_RE.findall(source):
                if int(stage) != self.stage:
                    continue
                names.setdefault(int(value), name)
                vals.setdefault(name, int(value))
        return names, vals

    def _read_addr_defines(self) -> Dict[str, int]:
        return {n: int(v, 16) for n, v in DEFINE_RE.findall(self.text)}

    def _read_arrays(self) -> List[Tuple[str, List[Item]]]:
        """Map arrays in file order: those whose body is entirely MAP_* data."""
        found = []
        for match in ARRAY_RE.finditer(self.text):
            name, body = match.group(1), match.group(2)
            if "MAP_" not in body:
                continue
            if not re.search(r"MAP_(CURVE|HEIGHT|LANES|HAZARD|OBJ|CMD|ESC)", body):
                continue
            found.append((name, tokenize_array(body, name)))
        return found

    def _read_goto_table(self) -> Dict[str, str]:
        """ADDR define name -> array name."""
        match = GOTO_TABLE_RE.search(self.text)
        if not match:
            return {}
        return {a.strip(): b for a, b in GOTO_ROW_RE.findall(match.group(1))}

    def stream_of(self, array_name: str) -> Optional[str]:
        parts = array_name.split("_")
        for index in range(len(parts) - 1, -1, -1):
            candidate = STREAM_ALIASES.get(parts[index])
            if candidate:
                return candidate
        return None

    def section_label(self, array_name: str, stream: str) -> str:
        """Best-effort human label for the section an array belongs to."""
        parts = array_name.split("_")
        for index, part in enumerate(parts):
            if STREAM_ALIASES.get(part) == stream:
                if index > 0 and parts[index - 1] not in ("map", "stage%d" % self.stage):
                    return parts[index - 1]
                if index + 1 < len(parts):
                    return "s" + parts[index + 1].lower()
                return "main"
        return "main"


def read_sections(stage_file: StageFile, allow_desync: bool, warn) -> List[Section]:
    """Group a stage file's map arrays into lockstep sections of six."""
    arrays = stage_file.arrays
    if len(arrays) % 6 != 0:
        raise MapError(
            "%s: found %d map arrays, not a multiple of 6"
            % (stage_file.path, len(arrays))
        )

    addr_of_array = {v: k for k, v in stage_file.goto_table.items()}
    sections = []

    for chunk_start in range(0, len(arrays), 6):
        chunk = arrays[chunk_start:chunk_start + 6]
        by_stream = {}
        for array_name, items in chunk:
            stream = stage_file.stream_of(array_name)
            if stream is None:
                raise MapError("%s: cannot classify array" % array_name)
            if stream in by_stream:
                raise MapError(
                    "arrays are not grouped six-per-section: %s repeats stream %s"
                    % (array_name, stream)
                )
            by_stream[stream] = (array_name, items)

        missing = [s for s in STREAMS if s not in by_stream]
        if missing:
            raise MapError(
                "section starting at %s is missing streams: %s"
                % (chunk[0][0], ", ".join(missing))
            )

        section = Section(
            stage_file.section_label(by_stream["curvature"][0], "curvature")
        )

        terminators = {}
        for stream in STREAMS:
            array_name, items = by_stream[stream]
            if stream == "curvature":
                section.curvature, term = expand_curvature(items, array_name)
            elif stream == "height":
                section.height, term = expand_height(items, array_name)
            elif stream == "lanes":
                section.lanes, term = expand_lanes(items, array_name)
            elif stream == "hazards":
                section.hazards, section.hazard_len, term = expand_hazards(
                    items, array_name
                )
            else:
                seq, term = expand_objects(items, array_name, stage_file.obj_vals)
                setattr(section, stream, seq)
            terminators[stream] = term
            addr_name = addr_of_array.get(array_name)
            if addr_name and addr_name in stage_file.addr_defines:
                section.addrs[stream] = stage_file.addr_defines[addr_name]

        section.terminator = terminators["curvature"]
        section.raw_terminators = terminators
        section.arrays = {s: by_stream[s][0] for s in STREAMS}
        section.check_sync(allow_desync, warn)
        sections.append(section)

    _name_sections(sections, stage_file)
    return sections


def _name_sections(sections: List[Section], stage_file: StageFile) -> None:
    """Give sections stable, unique labels, resolving terminator targets."""
    # Map ADDR define names to section labels via the goto table.
    array_to_section = {}
    for section in sections:
        for stream, array_name in section.arrays.items():
            array_to_section[array_name] = section

    addr_to_section = {}
    for addr_name, array_name in stage_file.goto_table.items():
        if array_name in array_to_section:
            addr_to_section[addr_name] = array_to_section[array_name].name

    seen = {}
    for section in sections:
        base = section.name
        if base in seen:
            seen[base] += 1
            section.name = "%s%d" % (base, seen[base])
        else:
            seen[base] = 0

    # Rebuild the mapping now that names are unique.
    addr_to_section = {}
    for addr_name, array_name in stage_file.goto_table.items():
        if array_name in array_to_section:
            addr_to_section[addr_name] = array_to_section[array_name].name

    for section in sections:
        term = section.terminator
        if term is None:
            section.flow = None
        elif term.kind == "goto":
            section.flow = ("goto", addr_to_section.get(term.args[0], term.args[0]))
        elif term.kind == "split":
            section.flow = (
                "split",
                addr_to_section.get(term.args[0], term.args[0]),
                addr_to_section.get(term.args[1], term.args[1]),
            )
        elif term.kind == "end":
            section.flow = ("end",)
        else:
            section.flow = None


# ── Decompile: Sections -> .map text ─────────────────────────────────────────


def object_legend(stage_file: StageFile, sections: List[Section]) -> Dict[int, str]:
    """Pick a one-character symbol for each object value that is actually used."""
    used = set()
    for section in sections:
        used.update(section.leftobjs)
        used.update(section.rightobjs)
    used.discard(0)

    legend, taken = {0: "."}, {"."}
    for value in sorted(used):
        name = stage_file.obj_names.get(value, "OBJ%d" % value)
        symbol = None
        for char in name:
            if char.isalpha() and char.upper() not in taken:
                symbol = char.upper()
                break
        if symbol is None:
            symbol = str(value)
            if symbol in taken:
                raise MapError("cannot pick a symbol for object %d (%s)" % (value, name))
        taken.add(symbol)
        legend[value] = symbol
    return legend


def row_boundaries(section: Section) -> List[int]:
    """Road-unit offsets where a new table row must start."""
    bounds = {0, section.units()}
    for seq in (section.curvature, section.height, section.lanes):
        offset = 0
        for _value, count in runs_of(seq):
            offset += count
            bounds.add(offset)
    for half_offset, _code in section.hazards:
        bounds.add(half_offset * 2)
    return sorted(b for b in bounds if 0 <= b <= section.units())


def format_object_cell(runs: List[Tuple[int, int]], legend: Dict[int, str]) -> str:
    parts = []
    for value, count in runs:
        symbol = legend.get(value, "?")
        parts.append(symbol if count == 1 else "%s%d" % (symbol, count))
    return " ".join(parts)


def half_span(start: int, end: int) -> Tuple[int, int]:
    """Half-units owned by a row spanning road units [start, end).

    Half-unit h covers road units 2h and 2h+1, and belongs to whichever row
    contains road unit 2h. Rounding both ends up partitions the half-units
    exactly, even across rows whose boundaries fall on odd road units.
    """
    return (start + 1) // 2, (end + 1) // 2


def decompile_section(section: Section, legend: Dict[int, str]) -> List[List[str]]:
    """Build the table rows for one section as lists of cell strings."""
    bounds = row_boundaries(section)

    hazards_at = {}
    for half_offset, code in section.hazards:
        hazards_at.setdefault(half_offset, []).append(code)

    rows = []
    for index in range(len(bounds) - 1):
        start, end = bounds[index], bounds[index + 1]
        count = end - start

        lane_val = section.lanes[start]
        lane_name = LANE_VALS.get(lane_val)
        if lane_name in TRANSITION_TO_CANVAS:
            road = TRANSITION_TO_CANVAS[lane_name]
        elif lane_name in ("TUNNEL_ENTRY", "TUNNEL_EXIT"):
            road = build_canvas(TUNNEL_NAME)
        elif lane_name in LANE_GEOMETRY:
            road = build_canvas(lane_name)
        else:
            road = "raw 0x%02X" % lane_val

        curve = CURVE_NAME_TO_GLYPH.get(section.curvature[start], "?")
        height = HEIGHT_NAME_TO_GLYPH.get(section.height[start], "?")

        half_start, half_end = half_span(start, end)
        cells = {}
        for stream in ("leftobjs", "rightobjs"):
            slice_ = getattr(section, stream)[half_start:half_end]
            cells[stream] = format_object_cell(runs_of(slice_), legend)

        haz = " ".join(
            HAZARD_CODE_TO_TOKEN.get(code, "esc%d" % code)
            for code in hazards_at.get(start // 2, [])
        ) if start % 2 == 0 else ""

        loop = ""
        if index == 0:
            loop = section.name
        rows.append([
            str(count), cells["leftobjs"], road, cells["rightobjs"],
            curve, height, haz, loop,
        ])

    if rows:
        flow = section.flow
        if flow and flow[0] == "goto":
            rows.append(["", "", "", "", "", "", "", "goto " + flow[1]])
        elif flow and flow[0] == "split":
            rows.append(["", "", "", "", "", "", "", "split %s %s" % (flow[1], flow[2])])
        elif flow and flow[0] == "end":
            rows.append(["", "", "", "", "", "", "", "end"])
    return rows


HEADERS = ["Dst", "Left objs", "Road", "Right objs", "Curve", "Height", "Haz", "Loop"]
ROAD_COLUMN = HEADERS.index("Road")


def render_table(rows: List[List[str]]) -> List[str]:
    widths = [len(h) for h in HEADERS]
    for row in rows:
        for index, cell in enumerate(row):
            widths[index] = max(widths[index], len(cell))

    def line(cells):
        return "| " + " | ".join(
            c.ljust(widths[i]) for i, c in enumerate(cells)
        ) + " |"

    out = [line(HEADERS), "| " + " | ".join("-" * w for w in widths) + " |"]
    out.extend(line(r) for r in rows)
    return out


def decompile(path: str, allow_desync: bool, warn) -> str:
    stage_file = StageFile(path)
    sections = read_sections(stage_file, allow_desync, warn)
    legend = object_legend(stage_file, sections)

    prefix = "stage%d_map" % stage_file.stage
    lines = [
        "; Decompiled from %s by map_compile.py." % os.path.basename(path),
        "; One row is one slice of road, read bottom to top: the last row is",
        "; the start of the stage. See C/docs/map-text-format.md.",
        "; stage: %d" % stage_file.stage,
        "; prefix: %s" % prefix,
    ]
    for value in sorted(v for v in legend if v != 0):
        lines.append(
            "; obj %s = %s"
            % (legend[value], stage_file.obj_names.get(value, "OBJ%d" % value))
        )
    for section in sections:
        for stream in STREAMS:
            if stream in section.addrs:
                lines.append(
                    "; addr %s %s = 0x%04X"
                    % (section.name, stream, section.addrs[stream])
                )
    lines.append("")

    all_rows = []
    for section in sections:
        if section.desynced:
            lines.append("; DESYNC: section '%s' was padded" % section.name)
        all_rows.extend(decompile_section(section, legend))
    # The table is written last-row-first so the road reads as ASCII art with
    # the start of the stage at the bottom, the way the player meets it.
    all_rows.reverse()
    lines.extend(render_table(all_rows))
    return "\n".join(lines) + "\n"


# ── Parsing a .map file ──────────────────────────────────────────────────────


class Row:
    def __init__(self, lineno: int, cells: List[str]):
        self.lineno = lineno
        self.cells = cells


class MapText:
    def __init__(self):
        self.stage = 1
        self.prefix = "stage1_map"
        self.scale = 1
        self.base = 0xC000
        self.obj_symbols: Dict[str, str] = {}   # symbol -> NAME
        self.addrs: Dict[Tuple[str, str], int] = {}
        self.rows: List[Row] = []


DIRECTIVE_RE = re.compile(r";\s*(\w+)\s*:\s*(\S+)")
OBJ_DIRECTIVE_RE = re.compile(r";\s*obj\s+(\S)\s*=\s*(\w+)")
ADDR_DIRECTIVE_RE = re.compile(r";\s*addr\s+(\w+)\s+(\w+)\s*=\s*(0x[0-9A-Fa-f]+)")


def parse_map_text(text: str, path: str) -> MapText:
    result = MapText()
    for lineno, raw in enumerate(text.splitlines(), 1):
        line = raw.strip()
        if not line:
            continue
        if line.startswith(";"):
            match = OBJ_DIRECTIVE_RE.match(line)
            if match:
                result.obj_symbols[match.group(1)] = match.group(2)
                continue
            match = ADDR_DIRECTIVE_RE.match(line)
            if match:
                stream = STREAM_ALIASES.get(match.group(2))
                if stream is None:
                    raise MapError(
                        "%s:%d: unknown stream '%s'" % (path, lineno, match.group(2))
                    )
                result.addrs[(match.group(1), stream)] = int(match.group(3), 16)
                continue
            match = DIRECTIVE_RE.match(line)
            if match:
                key, value = match.group(1), match.group(2)
                if key == "stage":
                    result.stage = int(value)
                elif key == "prefix":
                    result.prefix = value
                elif key == "scale":
                    result.scale = int(value)
                elif key == "base":
                    result.base = int(value, 16)
            continue

        if not line.startswith("|"):
            continue
        if set(line) <= set("|- "):
            continue

        # The Road cell keeps its leading spaces: the canvas is positional, and
        # 3R ("  H : : H") differs from 3L ("H : : H") only by that indent.
        # Renderers emit exactly one space of separator padding, so strip that
        # one and no more.
        raw_cells = line.strip("|").split("|")
        cells = [c.strip() for c in raw_cells]
        if len(raw_cells) > ROAD_COLUMN:
            road = raw_cells[ROAD_COLUMN]
            cells[ROAD_COLUMN] = (road[1:] if road.startswith(" ") else road).rstrip()
        if len(cells) != len(HEADERS):
            raise MapError(
                "%s:%d: expected %d columns, got %d"
                % (path, lineno, len(HEADERS), len(cells))
            )
        if [c.lower() for c in cells] == [h.lower() for h in HEADERS]:
            continue
        result.rows.append(Row(lineno, cells))
    # Rows are written bottom-up; everything downstream works in road order.
    result.rows.reverse()
    return result


def parse_object_cell(cell: str, symbols: Dict[str, str], where: str):
    """"B .3 T" -> [(NAME, count), ...] in half-units."""
    runs = []
    for token in cell.split():
        symbol, digits = token[0], token[1:]
        if digits and not digits.isdigit():
            raise MapError("%s: bad object token '%s'" % (where, token))
        count = int(digits) if digits else 1
        if symbol == ".":
            name = "NONE"
        elif symbol in symbols:
            name = symbols[symbol]
        else:
            raise MapError(
                "%s: object symbol '%s' has no '; obj %s = NAME' directive"
                % (where, symbol, symbol)
            )
        runs.append((name, count))
    return runs


# ── Compile: .map text -> C ──────────────────────────────────────────────────


class CompiledSection:
    def __init__(self, name: str):
        self.name = name
        self.rows: List[Row] = []
        self.flow = None
        self.flow_lineno = 0


def split_sections(mapping: MapText, path: str) -> List[CompiledSection]:
    if not mapping.rows:
        raise MapError("%s: no table rows found" % path)
    sections: List[CompiledSection] = []
    current: Optional[CompiledSection] = None
    closed = False

    for row in mapping.rows:
        loop = row.cells[7].strip()
        where = "%s:%d" % (path, row.lineno)
        body = [c for i, c in enumerate(row.cells) if i != 7]
        has_body = any(c for c in body)

        token = loop.split()
        starts_section = bool(token) and token[0] not in ("goto", "split", "end")

        if starts_section:
            if current is not None and not closed:
                raise MapError(
                    "%s: section '%s' starts before '%s' ends with "
                    "goto/split/end" % (where, token[0], current.name)
                )
            current = CompiledSection(token[0])
            sections.append(current)
            closed = False
            loop_rest = " ".join(token[1:])
        else:
            loop_rest = loop

        if current is None:
            raise MapError("%s: row before any section label" % where)
        if closed and has_body:
            raise MapError(
                "%s: unreachable row after section '%s' ended"
                % (where, current.name)
            )

        if has_body:
            current.rows.append(row)

        rest = loop_rest.split()
        if rest:
            verb = rest[0]
            if verb == "goto":
                if len(rest) != 2:
                    raise MapError("%s: 'goto' takes one label" % where)
                current.flow = ("goto", rest[1])
            elif verb == "split":
                if len(rest) != 3:
                    raise MapError("%s: 'split' takes two labels" % where)
                if has_body:
                    raise MapError(
                        "%s: a 'split' row must leave every other cell blank "
                        "-- the fork body comes from the shared forked_road_* "
                        "tables in CommonData.c" % where
                    )
                current.flow = ("split", rest[1], rest[2])
            elif verb == "end":
                current.flow = ("end",)
            else:
                raise MapError("%s: unknown flow token '%s'" % (where, verb))
            current.flow_lineno = row.lineno
            closed = True

    labels = {s.name for s in sections}
    for section in sections:
        if section.flow is None:
            raise MapError(
                "section '%s' has no goto/split/end terminator" % section.name
            )
        for target in section.flow[1:]:
            if target not in labels:
                raise MapError(
                    "section '%s': flow target '%s' is not a defined section"
                    % (section.name, target)
                )
    return sections


def build_section(compiled: CompiledSection, mapping: MapText, path: str) -> Section:
    section = Section(compiled.name)
    half_offset = 0

    for row in compiled.rows:
        where = "%s:%d" % (path, row.lineno)
        dst_text, lobj, road, robj, curve, height, haz, _loop = row.cells

        count = (int(dst_text) if dst_text else 1) * mapping.scale
        if count <= 0:
            raise MapError("%s: Dst must be positive" % where)
        start = len(section.curvature)

        curve_name = CURVE_GLYPHS.get(curve or ":")
        if curve_name is None:
            raise MapError(
                "%s: bad Curve cell '%s' (expected one of %s)"
                % (where, curve, " ".join(sorted(CURVE_GLYPHS)))
            )
        section.curvature.extend([curve_name] * count)

        height_name = HEIGHT_GLYPHS.get(height or "-")
        if height_name is None:
            raise MapError(
                "%s: bad Height cell '%s' (expected one of %s)"
                % (where, height, " ".join(sorted(HEIGHT_GLYPHS)))
            )
        section.height.extend([height_name] * count)

        canvas = road.rstrip()
        raw = re.fullmatch(r"raw\s+(0[xX][0-9A-Fa-f]+)", canvas)
        if canvas in TRANSITION_CANVASES:
            lane_value = LANE_NAME_TO_VAL[TRANSITION_CANVASES[canvas]]
        elif canvas in TRANSITION_TOKENS:
            # "4>3R" spelled out, for hand-editing without drawing the ramp.
            lane_value = LANE_NAME_TO_VAL[TRANSITION_TOKENS[canvas]]
        elif raw:
            lane_value = int(raw.group(1), 16)
        elif canvas in CANVAS_TO_LANE:
            lane_value = LANE_NAME_TO_VAL[CANVAS_TO_LANE[canvas]]
        elif canvas.strip().upper() in LANE_NAME_ALIASES:
            # Spelling the state out ("3R") avoids the canvas indent, which is
            # significant and easy to lose when hand-editing.
            lane_value = LANE_NAME_TO_VAL[LANE_NAME_ALIASES[canvas.strip().upper()]]
        elif ">" in canvas or "/" in canvas or "\\" in canvas:
            raise MapError(
                "%s: no lane transition exists for '%s'.\n"
                "  Defined transitions:\n%s\n"
                "  See C/docs/lane-transitions-3lto2l-3rto2m.md, which plans "
                "the missing mirror shape 3R>2M."
                % (
                    where,
                    canvas,
                    "\n".join(
                        "    %-9s %s" % (TRANSITION_TO_CANVAS[macro], token)
                        for token, macro in TRANSITION_TOKENS.items()
                    ),
                )
            )
        else:
            raise MapError(
                "%s: unrecognised Road cell '%s'.\n  Valid canvases:\n%s\n"
                "  Or a transition: %s"
                % (
                    where,
                    road,
                    "\n".join(
                        "    %-9s %s" % (build_canvas(n), n)
                        for n in LANE_GEOMETRY
                        if n != "TUNNEL_EXIT"
                    ),
                    ", ".join(TRANSITION_CANVASES),
                )
            )
        section.lanes.extend([lane_value] * count)

        # Half-rate columns. Objects are positional: a row owns the half-units
        # whose first road unit falls inside it (see half_span).
        half_start, _half_end = half_span(start, start + count)
        for stream, cell in (("leftobjs", lobj), ("rightobjs", robj)):
            runs = parse_object_cell(cell, mapping.obj_symbols, where)
            if runs and len(getattr(section, stream)) != half_start:
                raise MapError(
                    "%s: %s cell is out of step -- the stream is at half-unit "
                    "%d but this row starts at half-unit %d"
                    % (where, stream, len(getattr(section, stream)), half_start)
                )
            for name, run_count in runs:
                getattr(section, stream).extend([name] * run_count)

        # Hazard commands must land exactly on a half-unit, so their row has to
        # start on an even road unit. The decompiler guarantees this by making
        # every hazard offset a row boundary.
        if haz and start % 2 != 0:
            raise MapError(
                "%s: this row starts at road unit %d (odd), so it cannot carry "
                "hazard commands -- the hazard stream ticks once per two road "
                "units" % (where, start)
            )
        for token in haz.split():
            code = HAZARD_TOKENS.get(token)
            if code is None:
                match = re.fullmatch(r"esc(\d+)", token)
                if not match:
                    raise MapError(
                        "%s: unknown hazard token '%s' (known: %s, or escN)"
                        % (where, token, " ".join(sorted(HAZARD_TOKENS)))
                    )
                code = int(match.group(1))
            section.hazards.append((start // 2, code))

        half_offset = max(half_offset, (start + count) // 2)

    section.hazard_len = half_offset
    for stream in ("leftobjs", "rightobjs"):
        setattr(
            section, stream,
            [mapping_obj_value(mapping, n) for n in getattr(section, stream)],
        )
    return section


_OBJ_VALUE_CACHE: Dict[Tuple[int, str], int] = {}


def mapping_obj_value(mapping: MapText, name: str) -> int:
    """Object NAME -> nibble value, read from the stage's own data file."""
    key = (mapping.stage, name)
    if key in _OBJ_VALUE_CACHE:
        return _OBJ_VALUE_CACHE[key]
    if name == "NONE":
        _OBJ_VALUE_CACHE[key] = 0
        return 0
    for candidate in (
        os.path.join(DATA_DIR, "Stage%dData.c" % mapping.stage),
        os.path.join(DATA_DIR, "Stages.h"),
    ):
        if not os.path.exists(candidate):
            continue
        for stage, obj_name, value in OBJ_DEF_RE.findall(open(candidate).read()):
            if int(stage) == mapping.stage and obj_name == name:
                _OBJ_VALUE_CACHE[key] = int(value)
                return int(value)
    raise MapError(
        "no MAP_OBJ_S%d_%s_VAL is defined in Stage%dData.c or Stages.h"
        % (mapping.stage, name, mapping.stage)
    )


# ── Emission ─────────────────────────────────────────────────────────────────


def addr_define(prefix: str, section_name: str, stream: str) -> str:
    return "%s_%s_%s_ADDR" % (prefix.upper(), section_name.upper(), stream.upper())


def array_name(prefix: str, section_name: str, stream: str) -> str:
    return "%s_%s_%s" % (prefix, section_name, stream)


def assign_addresses(sections: List[Section], mapping: MapText) -> Dict[Tuple[str, str], int]:
    """Pinned '; addr' values first, then synthetic ones from '; base'."""
    addrs: Dict[Tuple[str, str], int] = dict(mapping.addrs)
    pinned = set(addrs.values())
    next_addr = mapping.base
    for section in sections:
        for stream in STREAMS:
            key = (section.name, stream)
            if key in addrs:
                continue
            while next_addr in pinned:
                next_addr += 1
            addrs[key] = next_addr
            pinned.add(next_addr)
            next_addr += 1
    return addrs


def encode_curvature(section: Section) -> List[str]:
    out = []
    for name, count in runs_of(section.curvature):
        for chunk in split_run(count, NIBBLE_MAX):
            out.append("MAP_CURVE_%s(%d)," % (name, chunk))
    return out


def encode_height(section: Section) -> List[str]:
    out = []
    for name, count in runs_of(section.height):
        for chunk in split_run(count, NIBBLE_MAX):
            out.append("MAP_HEIGHT_%s(%d)," % (name, chunk))
    return out


def encode_lanes(section: Section) -> List[str]:
    out = []
    tunnel_entry = LANE_NAME_TO_VAL["TUNNEL_ENTRY"]
    tunnel_exit = LANE_NAME_TO_VAL["TUNNEL_EXIT"]
    for value, count in runs_of(section.lanes):
        if value == tunnel_entry:
            # A tunnel of N units is always entry(N-2) then exit(2): the entry
            # value covers the body and only the last 2 units are the exit.
            if count < 3:
                raise MapError(
                    "section '%s': a tunnel needs at least 3 road units, got %d"
                    % (section.name, count)
                )
            for chunk in split_run(count - 2, BYTE_MAX):
                out.append("MAP_LANES_TUNNEL_ENTRY(%d)," % chunk)
            out.append("MAP_LANES_TUNNEL_EXIT(2),")
            continue
        if value == tunnel_exit:
            raise MapError(
                "section '%s': tunnel exit written explicitly; write the whole "
                "tunnel as one run" % section.name
            )
        name = LANE_VALS.get(value)
        for chunk in split_run(count, BYTE_MAX):
            if name:
                out.append("MAP_LANES_%s(%d)," % (name, chunk))
            else:
                out.append(
                    "/* unknown lanes val=0x%02X */ %d, 0x%02X," % (value, chunk, value)
                )
    return out


def encode_objects(section: Section, stream: str, stage: int, names: Dict[int, str]) -> List[str]:
    out = []
    for value, count in runs_of(getattr(section, stream)):
        macro = "MAP_OBJ_S%d_%s" % (
            stage, "NONE" if value == 0 else names.get(value, "OBJ%d" % value)
        )
        for chunk in split_run(count, NIBBLE_MAX):
            out.append("%s(%d)," % (macro, chunk))
    return out


def encode_hazards(section: Section) -> List[str]:
    out = []
    offset = 0
    events = sorted(section.hazards)
    for index, (at, code) in enumerate(events):
        if at < offset:
            raise MapError(
                "section '%s': hazard commands are out of order at half-unit %d"
                % (section.name, at)
            )
        if at > offset:
            for chunk in split_run(at - offset, BYTE_MAX):
                out.append("MAP_HAZARD_WAIT(%d)," % chunk)
            offset = at
        elif index > 0 and events[index - 1][0] == at:
            # Two commands at the same offset. Only the codes that re-enter the
            # read loop (10+) survive this; a latch command would read the
            # following escape byte as its counter and stall.
            if events[index - 1][1] not in HAZARD_RESTART_CODES:
                raise MapError(
                    "section '%s': hazard commands %s and %s share half-unit "
                    "%d; latching commands need at least one unit between them"
                    % (
                        section.name,
                        HAZARD_CODE_TO_TOKEN.get(events[index - 1][1]),
                        HAZARD_CODE_TO_TOKEN.get(code),
                        at,
                    )
                )
        out.append(hazard_macro(code) + ",")
    if section.hazard_len > offset:
        for chunk in split_run(section.hazard_len - offset, BYTE_MAX):
            out.append("MAP_HAZARD_WAIT(%d)," % chunk)
    return out


def line_bytes(text: str) -> int:
    """How many bytes one emitted line contributes to its array.

    Curvature, height and object runs pack count and type into one byte; lanes
    are a (count, value) pair; a hazard wait is one byte and a hazard command
    is the MAP_ESC escape plus its code.
    """
    if text.startswith("MAP_CMD_GOTO"):
        return 4
    if text.startswith("MAP_CMD_SPLIT"):
        return 6
    if text.startswith("MAP_LANES_") or text.startswith("/* unknown lanes"):
        return 2
    if text.startswith("MAP_ESC") or text.startswith("MAP_CMD_"):
        return 2  # escape byte + command code
    if text.startswith(("MAP_CURVE_", "MAP_HEIGHT_", "MAP_OBJ_", "MAP_HAZARD_WAIT")):
        return 1
    raise MapError("cannot size emitted line: %s" % text)


def encode_terminator(section: Section, prefix: str, stream: str) -> str:
    flow = section.flow
    if flow[0] == "goto":
        return "MAP_CMD_GOTO(%s)" % addr_define(prefix, flow[1], stream)
    if flow[0] == "split":
        return "MAP_CMD_SPLIT(%s, %s)" % (
            addr_define(prefix, flow[1], stream),
            addr_define(prefix, flow[2], stream),
        )
    return "MAP_CMD_FORK_END"


def compile_text(text: str, path: str) -> str:
    mapping = parse_map_text(text, path)
    compiled = split_sections(mapping, path)

    names = {}
    for symbol, name in mapping.obj_symbols.items():
        names[mapping_obj_value(mapping, name)] = name
    names[0] = "NONE"

    sections = []
    for item in compiled:
        section = build_section(item, mapping, path)
        section.flow = item.flow
        section.check_sync(False, lambda m: sys.stderr.write("warning: " + m + "\n"))
        sections.append(section)

    addrs = assign_addresses(sections, mapping)

    out = [
        "/* Generated by C/scripts/map_compile.py from %s."
        % os.path.basename(path),
        " * Paste into Stage%dData.c; this is a fragment, not a whole file. */"
        % mapping.stage,
        "",
        "// clang-format off",
        "",
        "/* Z80 addresses of the map sections, as referenced by the map",
        " * GOTO/SPLIT commands and stage%d_lookup_map_goto(). */" % mapping.stage,
    ]
    for section in sections:
        for stream in STREAMS:
            out.append(
                "#define %-40s (0x%04X)"
                % (addr_define(mapping.prefix, section.name, stream),
                   addrs[(section.name, stream)])
            )
    out.append("")

    encoders = {
        "curvature": lambda s: encode_curvature(s),
        "height": lambda s: encode_height(s),
        "lanes": lambda s: encode_lanes(s),
        "hazards": lambda s: encode_hazards(s),
        "leftobjs": lambda s: encode_objects(s, "leftobjs", mapping.stage, names),
        "rightobjs": lambda s: encode_objects(s, "rightobjs", mapping.stage, names),
    }

    # Encode once; the byte lengths go in both the forward declarations and the
    # definitions, matching Stage1Data.c, so the fragment can be pasted whole.
    bodies = []
    for section in sections:
        for stream in STREAMS:
            lines = encoders[stream](section)
            lines.append(encode_terminator(section, mapping.prefix, stream))
            bodies.append((
                array_name(mapping.prefix, section.name, stream),
                addrs[(section.name, stream)],
                lines,
                sum(line_bytes(line) for line in lines),
            ))

    out.append("/* Forward declarations; sizes must match the definitions. */")
    for name, _addr, _lines, size in bodies:
        out.append("static const u8 %s[%d];" % (name, size))
    out.append("")

    for name, addr, lines, size in bodies:
        out.append("/** $%04X: %s */" % (addr, name))
        out.append("static const u8 %s[%d] = {" % (name, size))
        for line in lines:
            out.append("  " + line)
        out.append("};")
        out.append("")

    out.append("/* Goto table rows, sorted by address (the lookup binary-searches). */")
    rows = sorted(
        (addrs[(s.name, stream)], addr_define(mapping.prefix, s.name, stream),
         array_name(mapping.prefix, s.name, stream))
        for s in sections for stream in STREAMS
    )
    width = max(len(r[1]) for r in rows)
    for _addr, define, array in rows:
        out.append("  { %-*s &%s[0] }," % (width + 1, define + ",", array))
    out.append("")
    out.append("// clang-format on")
    return "\n".join(out) + "\n"


# ── Self test ────────────────────────────────────────────────────────────────


def _fingerprint(section: Section) -> Dict[str, object]:
    """The semantic content of a section, independent of how runs are split."""
    return {
        "curvature": list(section.curvature),
        "height": list(section.height),
        "lanes": list(section.lanes),
        "leftobjs": list(section.leftobjs),
        "rightobjs": list(section.rightobjs),
        "hazards": sorted(section.hazards),
        "hazard_len": section.hazard_len,
        "flow": section.flow,
    }


def _sections_from_fragment(fragment: str, stage: int) -> List[Section]:
    """Read back a compiled fragment using the same reader as a stage file."""
    import tempfile

    header = "\n".join(
        "#define MAP_OBJ_S%d_%s_VAL (%d)" % (stage, name, value)
        for value, name in _stage_obj_names(stage).items()
    )
    body = header + "\n" + fragment
    body = re.sub(
        r"/\* Goto table rows.*?\n(  \{.*?\n)+",
        "static const struct { u16 z80; const void *ptr; } "
        "stage%d_map_goto_table[] = {\n" % stage
        + "\n".join(
            line for line in fragment.splitlines() if line.strip().startswith("{ ")
        )
        + "\n};\n",
        body,
        flags=re.S,
    )
    with tempfile.NamedTemporaryFile(
        "w", suffix="Stage%dData.c" % stage, delete=False
    ) as handle:
        handle.write(body)
        temp = handle.name
    try:
        stage_file = StageFile(temp)
        return read_sections(stage_file, False, lambda m: None)
    finally:
        os.unlink(temp)


def _stage_obj_names(stage: int) -> Dict[int, str]:
    names = {0: "NONE"}
    for candidate in (
        os.path.join(DATA_DIR, "Stage%dData.c" % stage),
        os.path.join(DATA_DIR, "Stages.h"),
    ):
        if not os.path.exists(candidate):
            continue
        for found, name, value in OBJ_DEF_RE.findall(open(candidate).read()):
            if int(found) == stage:
                names.setdefault(int(value), name)
    return names


def selftest() -> int:
    failures = []

    def check(label, condition, detail=""):
        if condition:
            print("  ok   %s" % label)
        else:
            print("  FAIL %s%s" % (label, ("\n       " + detail) if detail else ""))
            failures.append(label)

    # Stages 1, 2, 4 and 6 satisfy the 2:1 rate rule throughout, so they must
    # round-trip with no loss. Between them they cover SPLIT, FORK_END, GOTO,
    # tunnels, dirt track, objects, barriers, arrows, cars and helicopters.
    # Stage 6 is this tool's own output: it was authored as C/maps/stage6.map.
    for stage in (1, 2, 4, 6):
        print("round-trip Stage%dData.c" % stage)
        path = os.path.join(DATA_DIR, "Stage%dData.c" % stage)
        original = read_sections(StageFile(path), False, lambda m: None)
        text = decompile(path, False, lambda m: None)
        fragment = compile_text(text, "stage%d.map" % stage)
        rebuilt = _sections_from_fragment(fragment, stage)

        check(
            "stage %d: %d sections preserved" % (stage, len(original)),
            len(original) == len(rebuilt),
            "was %d, now %d" % (len(original), len(rebuilt)),
        )
        for before, after in zip(original, rebuilt):
            first, second = _fingerprint(before), _fingerprint(after)
            differing = [k for k in first if first[k] != second[k]]
            check(
                "stage %d: section '%s' round-trips" % (stage, before.name),
                not differing,
                "differs in: %s" % ", ".join(differing),
            )

    # stage3_map_height_C5A0 is 107 units against 192 of curvature, and
    # stage5_map_height_C684 is 358 against 444: genuine desyncs in the
    # original data, not tool bugs.
    print("the two desynced sections are rejected without --allow-desync")
    for stage in (3, 5):
        path = os.path.join(DATA_DIR, "Stage%dData.c" % stage)
        try:
            decompile(path, False, lambda m: None)
            check("stage %d rejected by default" % stage, False, "no error raised")
        except MapError as error:
            check(
                "stage %d rejected by default" % stage,
                "2:1 rate rule" in str(error),
                str(error),
            )
        try:
            text = decompile(path, True, lambda m: None)
            compile_text(text, "stage%d.map" % stage)
            check("stage %d round-trips with --allow-desync" % stage, True)
        except MapError as error:
            check(
                "stage %d round-trips with --allow-desync" % stage, False, str(error)
            )

    print()
    if failures:
        print("%d check(s) failed" % len(failures))
        return 1
    print("all checks passed")
    return 0


# ── Entry point ──────────────────────────────────────────────────────────────


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Compile and decompile Chase H.Q. stage map data.",
    )
    parser.add_argument(
        "--selftest", action="store_true",
        help="round-trip the committed stage data and check the diagnostics",
    )
    # Subcommands rather than bare positionals: argparse cannot split two
    # optional-nargs positionals around an interleaved flag, so a flat parser
    # rejects "decompile --allow-desync FILE".
    subcommands = parser.add_subparsers(dest="command")

    to_c = subcommands.add_parser("compile", help="compile a .map file to C")
    to_c.add_argument("file", help="the .map file to compile")

    to_map = subcommands.add_parser(
        "decompile", help="decompile a Stage*Data.c file to .map"
    )
    to_map.add_argument("file", help="the Stage*Data.c file to decompile")
    to_map.add_argument(
        "--allow-desync", action="store_true",
        help="tolerate sections that break the 2:1 rate rule, padding them",
    )

    args = parser.parse_args()

    if args.selftest:
        return selftest()
    if not args.command:
        parser.print_help()
        return 2

    def warn(message):
        sys.stderr.write("warning: " + message + "\n")

    if args.command == "decompile":
        sys.stdout.write(decompile(args.file, args.allow_desync, warn))
    else:
        sys.stdout.write(compile_text(open(args.file).read(), args.file))
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except MapError as error:
        sys.stderr.write("error: %s\n" % error)
        sys.exit(1)
