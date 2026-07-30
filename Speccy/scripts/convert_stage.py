#!/usr/bin/env python3
"""
convert_stage.py

Converts a Chase H.Q. skool file to a C stage data skeleton.

The output is a skeleton and does NOT compile as generated - see "What
requires manual completion" in C/docs/convert-stage.md. The committed
Stage{2-5}Data.c files contain that manual work, so redirecting this over one
of them discards it. Use this to scaffold a new stage, not to regenerate an
existing one.

Usage:
  python3 convert_stage.py <skool_file> <stage_num> [options] > StageNData.c

Options:
  --obj-names OBJ4,OBJ5,OBJ6,OBJ7,OBJ8
        Comma-separated names for object types 4-8 (beyond NONE/LIGHT/UNUSED/SHORT_POLE).
        Default: OBJ4,OBJ5,OBJ6,OBJ7,OBJ8

  --no-turn-signs
        Stage has no turn signs (objects 8 and 9 absent). Default.

  --turn-signs
        Stage has turn signs (objects 8 and 9).

The script generates C code to stdout. Sections it cannot fully decode
are emitted as raw u8 arrays; sections that are truly unresolvable are
emitted as /* TODO */ comments with raw hex.

What the script generates:
  - Backdrop (u8 array inline in stage_t, Pixels.h macro names)
  - All map data sections (curvature/height/lanes/hazards/leftobjs/rightobjs)
    decoded into MAP_ macros
  - Vehicle/hazard bitmap data (u8 arrays, Pixels.h macro names)
  - LOD table entries (bitmap_t arrays from 7-byte LOD records)
  - Mugshot/face data (u8 arrays, Pixels.h macro names)
  - Perp description, arrest messages (raw u8 arrays)
  - Helicopter data, object definitions, hazard LODs (raw u8 arrays)
  - LOD address table (raw u8 array)
  - Stretchy graphic data (raw u8 arrays)
  - Forward declarations for all generated arrays
  - const stage_t stageN = { ... } initialiser (partially filled)
  - stageN_lookup_map_goto() switch body

What requires manual completion:
  - obj_t / hittable_t graphic definition arrays (typed structs)
  - stretchy_t / depthset_t tables (typed structs)
"""

import sys
import re
import argparse
from typing import Optional, List, Tuple, Dict

# ── Encoding tables ──────────────────────────────────────────────────────────

CURVE_TYPES = {
    0: "STRAIGHT",
    1: "RIGHT",
    2: "RIGHT_HARD",
    3: "RIGHT_VERY_HARD",
    9: "LEFT",
    10: "LEFT_HARD",
    11: "LEFT_VERY_HARD",
}

HEIGHT_TYPES = {
    1: "UP7",
    3: "UP5",
    5: "UP3",
    7: "UP1",
    8: "LEVEL",
    9: "DOWN1",
    11: "DOWN3",
    13: "DOWN5",
    15: "DOWN7",
}

LANE_VALS = {
    0x00: "4",
    0x81: "3L",
    0x82: "3R",
    0x01: "2L",
    0x02: "2M",
    0x03: "2R",
    # Transition names carry the side of *both* endpoints, so the "from" lane
    # keeps its L/R too: 0x9E is 3R-to-4, not 3-to-4R. These must match the
    # MAP_LANES_* macros in C/libraries/ChaseHQ/Data/Stages.h exactly or the
    # generated file will not compile.
    0xBD: "4TO3L",
    0x8E: "4TO3R",
    0xAD: "3LTO4",
    0x9E: "3RTO4",
    0x06: "3LTO2M",
    0x0F: "3RTO2R",
    0x2D: "2LTO3L",
    0x1F: "2RTO3R",
    0x45: "TUNNEL_ENTRY",
    0x59: "TUNNEL_EXIT",
    0xC1: "DIRTTRACK",
    0xED: "FORKED",
}

# ESC command byte → macro name (None = needs special handling)
HAZARD_CMDS = {
    0: None,  # GOTO/LOOP
    1: "MAP_CMD_FORK_END",
    2: None,  # SPLIT
    3: "MAP_CMD_STOP_BARRIERS",
    4: "MAP_ESC, (4)",  # HAZARD1_L (Stage 2+) / START_BARRIERS_L (S1)
    5: "MAP_ESC, (5)",  # HAZARD1_R
    6: "MAP_CMD_UNKNOWN_HAZARD_6",
    7: "MAP_CMD_START_BARRIERS_L",
    8: "MAP_CMD_START_BARRIERS_R",
    9: "MAP_CMD_START_TWO_BARRIERS",
    10: "MAP_CMD_ARROW_OFF",
    11: "MAP_CMD_ARROW_L",
    12: "MAP_CMD_ARROW_R",
    13: "MAP_CMD_START_CARS",
    14: "MAP_CMD_STOP_CARS",
    15: "MAP_ESC, (15)",  # stop helicopter
    17: "MAP_ESC, (17)",  # start heli left
    18: "MAP_ESC, (18)",  # start heli right (Stage4Data.c)
}

STRETCHY_TYPE_NAMES: Dict[int, str] = {
    1: "STRETCHY_TYPE_END",
    2: "STRETCHY_TYPE_FIXED",
    3: "STRETCHY_TYPE_150PC",
    4: "STRETCHY_TYPE_50PC",
    5: "STRETCHY_TYPE_113PC",
    6: "STRETCHY_TYPE_38PC",
    7: "STRETCHY_TYPE_75PC",
    8: "STRETCHY_TYPE_25PC",
    9: "STRETCHY_TYPE_100PC",
    10: "STRETCHY_TYPE_200PC",
}

# Maps raw DEFW values (before adding bank_offset) to common depthset C names.
# These depthsets are shared across all stages; their pointers appear in every
# stage's stretchy data as [out-of-bounds] DEFWs because the depthsets live in
# a different bank (bank 1, common data). The raw values are constant across
# all stages regardless of the stage's bank_offset.
COMMON_DEPTHSET_RAW_MAP: Dict[int, str] = {
    0x7E38: "streetlampbottom_left",
    0x7E4E: "streetlampbottom_right",
    0x7E64: "streetlampmiddle2_left",
    0x7E7A: "streetlampmiddle2_right",
    0x7E90: "streetlampmiddle_left",
    0x7EA6: "streetlampmiddle_right",
}

BITMAP_FLAG_DEFAULT_VAL = 0
BITMAP_FLAG_MASKED_VAL = 1

BITMAP_FLAGS = {
    BITMAP_FLAG_DEFAULT_VAL: "BITMAPFLAG_DEFAULT",
    BITMAP_FLAG_MASKED_VAL: "BITMAPFLAG_MASKED",
    2: "BITMAPFLAG_FLIPPED",
    3: "BITMAPFLAG_MASKED|BITMAPFLAG_FLIPPED",
}

# Z80 runtime addresses of handler routines (stable across all stages).
HANDLER_ADDRESS_MAP: Dict[int, str] = {
    0x9252: "draw_tunnel_light_right",
    0x924D: "draw_tunnel_light_left",
    0x9171: "draw_stretchy_object_right",
    0x916C: "draw_stretchy_object_left",
    0x9052: "draw_overhead",
    0x92E1: "draw_object_right",
    0x9278: "draw_object_left",
}

CHATTERCHR_NAMES = {
    0: "CHATTERCHR_PILOT",
    1: "CHATTERCHR_NANCY",
    2: "CHATTERCHR_RAYMOND",
    3: "CHATTERCHR_TONY",
}

DRAWCHARSTYLE_NAMES = {
    1: "DRAWCHARSTYLE_GENERIC",
    2: "DRAWCHARSTYLE_SINGLE",
    3: "DRAWCHARSTYLE_DOUBLE",
    4: "DRAWCHARSTYLE_SINGLE_INVERTED",
    5: "DRAWCHARSTYLE_DOUBLE_INVERTED",
}

TRANSITIONCONTROL_NAMES = {
    0: "TRANSITIONCONTROL_STOP",
    1: "TRANSITIONCONTROL_DRAW_MUGSHOTS",
    2: "TRANSITIONCONTROL_OVERLAY_MESSAGES",
    3: "TRANSITIONCONTROL_FILL_ATTRIBUTES",
    4: "TRANSITIONCONTROL_FADE",
}

ATTR_COLOR_NAMES = {
    0: "ATTR_BLACK",
    1: "ATTR_BLUE",
    2: "ATTR_RED",
    3: "ATTR_MAGENTA",
    4: "ATTR_GREEN",
    5: "ATTR_CYAN",
    6: "ATTR_YELLOW",
    7: "ATTR_WHITE",
}


def _mkattr(f: int, b: int, p: int, i: int) -> int:
    return (f << 7) | (b << 6) | (p << 3) | i


# Maps the byte values of every named enum entry in ZXSpectrum/Spectrum.h
ATTRIBUTE_MAP: Dict[int, str] = {
    _mkattr(0, 0, 0, 0): "attribute_BLACK_OVER_BLACK",
    _mkattr(0, 0, 0, 1): "attribute_BLUE_OVER_BLACK",
    _mkattr(0, 0, 0, 2): "attribute_RED_OVER_BLACK",
    _mkattr(0, 0, 0, 3): "attribute_MAGENTA_OVER_BLACK",
    _mkattr(0, 0, 0, 4): "attribute_GREEN_OVER_BLACK",
    _mkattr(0, 0, 0, 5): "attribute_CYAN_OVER_BLACK",
    _mkattr(0, 0, 0, 6): "attribute_YELLOW_OVER_BLACK",
    _mkattr(0, 0, 0, 7): "attribute_WHITE_OVER_BLACK",
    _mkattr(0, 1, 0, 1): "attribute_BRIGHT_BLUE_OVER_BLACK",
    _mkattr(0, 1, 0, 2): "attribute_BRIGHT_RED_OVER_BLACK",
    _mkattr(0, 1, 0, 3): "attribute_BRIGHT_MAGENTA_OVER_BLACK",
    _mkattr(0, 1, 0, 4): "attribute_BRIGHT_GREEN_OVER_BLACK",
    _mkattr(0, 1, 0, 5): "attribute_BRIGHT_CYAN_OVER_BLACK",
    _mkattr(0, 1, 0, 6): "attribute_BRIGHT_YELLOW_OVER_BLACK",
    _mkattr(0, 1, 0, 7): "attribute_BRIGHT_WHITE_OVER_BLACK",
    _mkattr(0, 1, 1, 0): "attribute_BRIGHT_BLACK_OVER_BLUE",
    _mkattr(0, 1, 2, 0): "attribute_BRIGHT_BLACK_OVER_RED",
    _mkattr(0, 1, 3, 0): "attribute_BRIGHT_BLACK_OVER_MAGENTA",
    _mkattr(0, 1, 4, 0): "attribute_BRIGHT_BLACK_OVER_GREEN",
    _mkattr(0, 1, 5, 0): "attribute_BRIGHT_BLACK_OVER_CYAN",
    _mkattr(0, 1, 6, 0): "attribute_BRIGHT_BLACK_OVER_YELLOW",
    _mkattr(0, 1, 7, 0): "attribute_BRIGHT_BLACK_OVER_WHITE",
    _mkattr(0, 0, 4, 0): "attribute_BLACK_OVER_GREEN",
    _mkattr(0, 0, 5, 0): "attribute_BLACK_OVER_CYAN",
    _mkattr(0, 0, 6, 0): "attribute_BLACK_OVER_YELLOW",
    _mkattr(0, 0, 7, 0): "attribute_BLACK_OVER_WHITE",
}


def byte_to_attr(b: int) -> str:
    """Return the attribute_* name for b, or MKATTR(...) if not in the enum."""
    if b in ATTRIBUTE_MAP:
        return ATTRIBUTE_MAP[b]
    f = (b >> 7) & 1
    br = (b >> 6) & 1
    p = (b >> 3) & 7
    i = b & 7
    return f"MKATTR({f}, {br}, {ATTR_COLOR_NAMES[p]}, {ATTR_COLOR_NAMES[i]})"


def extract_arg_from_comment(comment: str) -> Optional[str]:
    """Extract a C arg expression from an obj_t DEFW inline comment.

    Matches 'Argument - name' patterns; returns '&name' or None.
    """
    m = re.search(r"[Aa]rgument\s*[-–]\s*(\w+)", comment)
    if m:
        name = m.group(1)
        if name.lower() not in ("null", "none"):
            return f"&{name}"
    return None


# Face/mugshot layout constants (must match ChaseHQ/Engine/Internal.h)
FACE_ROW_BYTES = 4  # FACEROWBYTES  = FACEWIDTH / 8 = 32 / 8
FACE_BITMAP_ROWS = 40  # FACEHEIGHT
FACE_ATTR_ROWS = 5  # FACEATTRHEIGHT
FACE_BITMAP_BYTES = FACE_ROW_BYTES * FACE_BITMAP_ROWS  # 160
FACE_ATTR_BYTES = FACE_ROW_BYTES * FACE_ATTR_ROWS  # 20


def emit_face_mugshot_array(name: str, data: List[int], z80_addr: int) -> List[str]:
    """Emit a face/mugshot array: pixel names for the bitmap rows, attribute
    names for the final FACE_ATTR_ROWS rows."""
    lines = [
        f"// ${z80_addr:04X}",
        "// clang-format off",
        f"static const u8 {name}[{len(data)}] = {{",
    ]

    for i in range(0, FACE_BITMAP_BYTES, FACE_ROW_BYTES):
        chunk = data[i : i + FACE_ROW_BYTES]
        lines.append("  " + ", ".join(byte_to_pixel(b) for b in chunk) + ",")

    lines.append("")

    for i in range(
        FACE_BITMAP_BYTES, FACE_BITMAP_BYTES + FACE_ATTR_BYTES, FACE_ROW_BYTES
    ):
        chunk = data[i : i + FACE_ROW_BYTES]
        lines.append("  " + ", ".join(byte_to_attr(b) for b in chunk) + ",")

    lines.append("};")
    lines.append("// clang-format on")
    return lines


CHATTERCMD_NAMES = {
    0xFC: "CHATTERCMD_RANDOM",
    0xFE: "CHATTERCMD_PAUSE",
    0xFF: "CHATTERCMD_STOP",
}

# Fields of stage_t that come from perstage words 3..13.
PERSTAGE_PTR_FIELDS = [
    "addrof_hittable_objects",
    "addrof_right_hand_handlers",
    "addrof_right_hand_objects",
    "addrof_right_hand_short_pole_object",
    "addrof_left_hand_handlers",
    "addrof_left_hand_objects",
    "addrof_left_hand_short_pole_object",
    "addrof_perp_description",
    "addrof_arrest_messages",
    "addrof_helicopter_stuff_1",
    "addrof_helicopter_stuff_2",
]

# ── Skool parser ─────────────────────────────────────────────────────────────


def parse_hex(s: str) -> int:
    """Parse $XX or 0xXX or decimal."""
    s = s.strip()
    if s.startswith("$"):
        return int(s[1:], 16)
    if s.startswith("0x") or s.startswith("0X"):
        return int(s[2:], 16)
    return int(s)


class SkoolRecord:
    def __init__(
        self,
        addr: int,
        rtype: str,
        values: List[int],
        comment: str,
        annotations: List[int],
    ):
        self.addr = addr  # Z80 absolute address
        self.rtype = rtype  # 'B'=bytes, 'W'=words, 'C'=section label
        self.values = values  # list of ints (bytes or words)
        self.comment = comment  # inline comment text (stripped)
        self.annotations = annotations  # resolved addresses from [$XXXX] in comment


def parse_skool(path: str) -> Tuple[List[SkoolRecord], List[str], List[bool]]:
    """
    Return (records, section_comments, fresh_header_flags).
    section_comments[i] is the comment line immediately before records[i].
    fresh_header_flags[i] is True if a non-trivial header comment was seen
    immediately before records[i], even if its text is identical to the
    still-pending previous header (e.g. two consecutive "LOD table for
    'stretchy'" tables at different addresses) - see split_into_sections.
    """
    records: List[SkoolRecord] = []
    section_comments: List[str] = []
    fresh_header_flags: List[bool] = []
    pending_section = ""
    just_saw_header = False

    def _annots(cmt: str) -> List[int]:
        return [int(m, 16) for m in re.findall(r"\[\$([0-9A-Fa-f]+)\]", cmt)]

    with open(path, encoding="utf-8", errors="replace") as f:
        for line in f:
            line = line.rstrip()
            if not line:
                continue

            # Section header comment.
            # Update pending_section based on priority:
            #  - [Stage N] headers always win
            #  - Non-trivial comments that classify to a recognised type
            #    (not 'unknown') overwrite when type or text changes
            #  - Non-trivial comments set pending if it is currently empty
            if line.startswith("; "):
                txt = line[2:].strip()
                trivial = txt in ("", ".", "}", "{", ";", "LOD")
                is_stage_hdr = bool(re.match(r"\[Stage \d+\]", txt))
                if is_stage_hdr:
                    pending_section = txt
                    just_saw_header = True
                elif not trivial:
                    just_saw_header = True
                    if not pending_section:
                        pending_section = txt
                    else:
                        new_type = classify_section(txt)
                        cur_type = classify_section(pending_section)
                        is_subentry = new_type == "obj_defs_entry" and cur_type in (
                            "obj_defs_right",
                            "obj_defs_left",
                        )
                        if (
                            new_type != "unknown"
                            and (new_type != cur_type or txt != pending_section)
                            and not is_subentry
                        ):
                            pending_section = txt
                continue

            # Labelled byte data: b$ADDR DEFB ...
            m = re.match(r"^([bcw]?)\$([0-9A-Fa-f]+)\s+(DEFB|DEFW)\s+(.*)", line)
            if not m:
                # Continuation line: ' $ADDR DEFB ...' or ' $ADDR DEFW ...'
                m = re.match(r"^\s+\$([0-9A-Fa-f]+)\s+(DEFB|DEFW)\s+(.*)", line)
                if m:
                    addr = int(m.group(1), 16)
                    dtype = m.group(2)
                    rest = m.group(3)
                else:
                    continue
                prefix = ""
            else:
                prefix = m.group(1)
                addr = int(m.group(2), 16)
                dtype = m.group(3)
                rest = m.group(4)

            # Split rest into value list and comment
            if ";" in rest:
                val_part, cmt_part = rest.split(";", 1)
                cmt_part = cmt_part.strip()
            else:
                val_part = rest
                cmt_part = ""

            # Parse values
            values = []
            for tok in re.split(r"[,\s]+", val_part.strip()):
                if tok and tok.strip():
                    try:
                        values.append(parse_hex(tok))
                    except ValueError:
                        pass

            rtype = "W" if dtype == "DEFW" else "B"
            rec = SkoolRecord(addr, rtype, values, cmt_part, _annots(cmt_part))
            records.append(rec)
            section_comments.append(pending_section)
            fresh_header_flags.append(just_saw_header)
            just_saw_header = False
            if prefix:  # new labelled section resets pending comment
                pending_section = ""

    return coalesce_split_words(records, section_comments, fresh_header_flags)


def coalesce_split_words(
    records: List[SkoolRecord],
    section_comments: List[str],
    fresh_header_flags: List[bool],
) -> Tuple[List[SkoolRecord], List[str], List[bool]]:
    """Rejoin address operands that SkoolKit emitted as two DEFBs.

    Where the control file has no `W` directive over a pointer, SkoolKit writes
    it as a pair of DEFBs instead of one DEFW:

        $E904 DEFB $7D    ; [$E97D] Pre-shifted bitmap address
        $E905 DEFB $65

    Every decoder here expects pointers to arrive as `W` records -- the LOD,
    obj_t, stretchy and map-command parsers all pair a DEFB against the DEFW
    that follows it -- so an unrejoined pair does not merely lose that one
    pointer, it shifts every later field by a slot. In stage 2 that paired the
    LOD entries against the wrong bitmaps, dropped two obj_t entries and lost
    the SPLIT terminator off three map sections.

    The `[$XXXX]` annotation is the load-bearing signal: SkoolKit writes it
    only on an operand it resolved as an address, so ordinary byte tables are
    never caught by this. The follower must be unannotated and immediately
    adjacent, which is what distinguishes a split pointer from two neighbouring
    single-byte address operands.
    """
    out: List[SkoolRecord] = []
    out_comments: List[str] = []
    out_fresh: List[bool] = []
    i = 0
    while i < len(records):
        rec = records[i]
        nxt = records[i + 1] if i + 1 < len(records) else None
        if (
            rec.rtype == "B"
            and len(rec.values) == 1
            and rec.annotations
            and rec.annotations[0] >= 0
            and nxt is not None
            and nxt.rtype == "B"
            and len(nxt.values) == 1
            and nxt.addr == rec.addr + 1
            and not [a for a in nxt.annotations if a >= 0]
        ):
            word = (nxt.values[0] << 8) | rec.values[0]
            out.append(
                SkoolRecord(rec.addr, "W", [word], rec.comment, [rec.annotations[0]])
            )
            out_comments.append(section_comments[i])
            out_fresh.append(fresh_header_flags[i])
            i += 2
            continue
        out.append(rec)
        out_comments.append(section_comments[i])
        out_fresh.append(fresh_header_flags[i])
        i += 1
    return out, out_comments, out_fresh


def build_addr_map(records: List[SkoolRecord]) -> Dict[int, int]:
    """Build a flat address → byte value map."""
    m: Dict[int, int] = {}
    for rec in records:
        addr = rec.addr
        if rec.rtype == "W":
            for w in rec.values:
                m[addr] = w & 0xFF
                m[addr + 1] = (w >> 8) & 0xFF
                addr += 2
        else:
            for b in rec.values:
                m[addr] = b
                addr += 1
    return m


def compute_bank_offset(records: List[SkoolRecord]) -> int:
    """
    Determine the bank offset (abs_addr - raw_z80_addr) by looking at the
    first DEFW record with a [$XXXX] annotation that is > its raw value.
    """
    for rec in records:
        if rec.rtype == "W" and rec.annotations and rec.values:
            raw = rec.values[0]
            ann = rec.annotations[0]
            diff = ann - raw
            if 0 < diff < 0x10000:
                return diff
    return 0


def parse_defm_text(rest: str) -> str:
    """Convert a DEFM operand (e.g. "HI THERE",$CE) to a C string literal."""
    # Strip inline comment, respecting quotes
    in_q = False
    for i, ch in enumerate(rest):
        if ch == '"':
            in_q = not in_q
        elif ch == ";" and not in_q:
            rest = rest[:i]
            break
    rest = rest.strip()

    c_chars: List[str] = []
    i = 0
    while i < len(rest):
        if rest[i] == '"':
            i += 1
            while i < len(rest) and rest[i] != '"':
                c = rest[i]
                if c == "\\":
                    c_chars.append("\\\\")
                elif c == '"':
                    c_chars.append('\\"')
                else:
                    c_chars.append(c)
                i += 1
            i += 1  # skip closing quote
        elif rest[i] in (" ", ","):
            i += 1
        elif rest[i] == "$":
            b = int(rest[i + 1 : i + 3], 16)
            c_chars.append(f"\\x{b:02X}")
            i += 3
        elif rest[i].isdigit():
            j = i
            while j < len(rest) and rest[j].isdigit():
                j += 1
            c_chars.append(f"\\x{int(rest[i:j]):02X}")
            i = j
        else:
            i += 1
    return '"' + "".join(c_chars) + '"'


def _parse_defm_raw(rest: str) -> List[int]:
    """Parse a DEFM operand to raw byte values."""
    in_q = False
    for i, ch in enumerate(rest):
        if ch == '"':
            in_q = not in_q
        elif ch == ";" and not in_q:
            rest = rest[:i]
            break
    rest = rest.strip()
    result = []
    i = 0
    while i < len(rest):
        if rest[i] == '"':
            i += 1
            while i < len(rest) and rest[i] != '"':
                result.append(ord(rest[i]))
                i += 1
            i += 1
        elif rest[i] in (" ", ","):
            i += 1
        elif rest[i] == "$":
            result.append(int(rest[i + 1 : i + 3], 16))
            i += 3
        elif rest[i].isdigit():
            j = i
            while j < len(rest) and rest[j].isdigit():
                j += 1
            result.append(int(rest[i:j]))
            i = j
        else:
            i += 1
    return result


def bytes_to_c_string(bs: List[int]) -> str:
    """Render a byte run as a C string literal."""
    out: List[str] = []
    prev_was_hex_escape = False
    for b in bs:
        if 0x20 <= b < 0x7F:
            ch = chr(b)
            # A printable hex digit straight after a \xNN escape would be
            # swallowed into it ("\xC" "A" reads as \xCA), so close the literal
            # and reopen: adjacent literals concatenate.
            if prev_was_hex_escape and ch in "0123456789abcdefABCDEF":
                out.append('" "')
            if ch in ('"', "\\"):
                out.append("\\" + ch)
            else:
                out.append(ch)
            prev_was_hex_escape = False
        else:
            out.append("\\x%02X" % b)
            prev_was_hex_escape = True
    return '"' + "".join(out) + '"'


# A string run ends on the byte with bit 7 set; refuse to stitch beyond this
# many bytes rather than swallow the rest of the bank on a missing terminator.
_MAX_STRING_RUN = 256


def parse_defm_map(path: str) -> Tuple[Dict[int, str], Dict[int, List[int]]]:
    """Scan a skool file for text runs in a single pass.

    Returns (strings, bytemap) where strings maps addr → C string literal
    and bytemap maps addr → raw byte list.  Skool label addresses are
    already absolute so no bank offset is applied.

    A run starts at a DEFM but need not end there: where the control file has
    no `T` directive over the whole string, SkoolKit splits it into a short
    DEFM followed by DEFB bytes ($E145 in bank 1 is the worked example --
    `DEFM "THIS IS "` then eight DEFB lines carrying the rest). Unlabelled
    DEFB lines that continue on from the previous byte are therefore stitched
    back on until the terminator byte is seen, so the string comes out whole
    however the region happens to be controlled.
    """
    lines: List[Tuple[int, bool, bool, List[int]]] = []  # addr, is_defm, labelled, bytes
    with open(path, encoding="utf-8", errors="replace") as f:
        for line in f:
            line = line.rstrip()
            m = re.match(r"^([bcw]?)\$([0-9A-Fa-f]+)\s+(DEFM|DEFB)\s+(.*)", line)
            labelled = bool(m and m.group(1))
            if not m:
                m = re.match(r"^\s+\$([0-9A-Fa-f]+)\s+(DEFM|DEFB)\s+(.*)", line)
                if not m:
                    continue
                addr, dtype, rest = int(m.group(1), 16), m.group(2), m.group(3)
            else:
                addr, dtype, rest = int(m.group(2), 16), m.group(3), m.group(4)
            lines.append((addr, dtype == "DEFM", labelled, _parse_defm_raw(rest)))

    at = {addr: i for i, (addr, _, _, _) in enumerate(lines)}

    strings: Dict[int, str] = {}
    bytemap: Dict[int, List[int]] = {}
    for i, (addr, is_defm, _, bs) in enumerate(lines):
        if not is_defm:
            continue
        run = list(bs)
        next_addr = addr + len(bs)
        while run and not (run[-1] & 0x80) and len(run) < _MAX_STRING_RUN:
            j = at.get(next_addr)
            # Stop at a new DEFM or a labelled entry: that is the next string,
            # or the next table, not more of this one.
            if j is None or lines[j][1] or lines[j][2]:
                break
            run.extend(lines[j][3])
            next_addr += len(lines[j][3])
        strings[addr] = bytes_to_c_string(run)
        bytemap[addr] = run
    return strings, bytemap


# ── Map data decoders ─────────────────────────────────────────────────────────


def decode_curvature(stream: List[int]) -> List[str]:
    """Decode a curvature byte stream into MAP_CURVE_xxx macros."""
    out = []
    i = 0
    while i < len(stream):
        b = stream[i]
        if b == 0:
            out.append(None)  # ESC – handled by caller
            break
        count = b >> 4
        ttype = b & 0x0F
        name = CURVE_TYPES.get(ttype)
        if name and count > 0:
            out.append(f"MAP_CURVE_{name}({count}),")
        else:
            out.append(f"/* unknown curvature 0x{b:02X} */")
        i += 1
    return out


def decode_height(stream: List[int]) -> List[str]:
    """Decode a height byte stream into MAP_HEIGHT_xxx macros."""
    out = []
    i = 0
    while i < len(stream):
        b = stream[i]
        if b == 0:
            out.append(None)
            break
        count = b >> 4
        ttype = b & 0x0F
        name = HEIGHT_TYPES.get(ttype)
        if name and count > 0:
            out.append(f"MAP_HEIGHT_{name}({count}),")
        else:
            out.append(f"/* unknown height 0x{b:02X} */")
        i += 1
    return out


def decode_lanes(stream: List[int]) -> List[str]:
    """
    Decode a lanes byte stream (count, val pairs) into MAP_LANES_xxx macros.
    Each pair: stream[i]=count, stream[i+1]=lane_val.
    """
    out = []
    i = 0
    while i < len(stream) - 1:
        b = stream[i]
        if b == 0:
            out.append(None)
            break
        count = b
        val = stream[i + 1]
        name = LANE_VALS.get(val)
        if name:
            out.append(f"MAP_LANES_{name}({count}),")
        else:
            out.append(f"/* unknown lanes val=0x{val:02X} */ {count}, 0x{val:02X},")
        i += 2
    return out


def decode_hazards(stream: List[int]) -> List[str]:
    """Decode a hazard byte stream into MAP_HAZARD_WAIT / MAP_CMD_xxx."""
    out = []
    i = 0
    while i < len(stream):
        b = stream[i]
        if b != 0:
            out.append(f"MAP_HAZARD_WAIT({b}),")
            i += 1
        else:
            # ESC sequence
            if i + 1 >= len(stream):
                break
            cmd = stream[i + 1]
            macro = HAZARD_CMDS.get(cmd)
            if cmd == 0:  # GOTO/LOOP: needs DEFW
                out.append(None)
                break
            elif cmd == 1:
                out.append("MAP_CMD_FORK_END,")
                i += 2
            elif cmd == 2:  # SPLIT: needs 2×DEFW
                out.append(None)
                break
            elif macro:
                out.append(f"{macro},")
                i += 2
            else:
                out.append(f"MAP_ESC, ({cmd}),")
                i += 2
    return out


def decode_objects(stream: List[int], stage: int, obj_names: List[str]) -> List[str]:
    """Decode an object byte stream using stage-specific names."""
    out = []
    i = 0
    while i < len(stream):
        b = stream[i]
        if b == 0:
            out.append(None)
            break
        count = b >> 4
        otype = b & 0x0F
        if count > 0 and otype < len(obj_names):
            name = obj_names[otype]
            out.append(f"MAP_OBJ_S{stage}_{name}({count}),")
        else:
            out.append(f"/* obj 0x{b:02X} */")
        i += 1
    return out


# ── Section identification ────────────────────────────────────────────────────

MAP_SECTION_TYPES = {
    "curvature": decode_curvature,
    "height": decode_height,
    "lanes": decode_lanes,
    "hazards": decode_hazards,
    "left object": None,  # objects – set below
    "right object": None,
}


def classify_section(comment: str) -> Optional[str]:
    """
    Return a section type key from a section-header comment, or None.
    The comment is like 'Stage 2] Map curvature data'.
    """
    c = comment.lower()
    if "horizon graphic" in c or "backdrop" in c:
        return "backdrop"
    if "per-stage data" in c:
        return "perstage"
    if "per-stage difficulty" in c:
        return "difficulty"
    if "per-stage setup data" in c:
        return "setupdata"
    if "per-stage attract" in c:
        return "attractdata"
    if "table of addresses of lods" in c:
        return "lodaddrs"
    if "map curvature" in c:
        return "curvature"
    if "map height" in c:
        return "height"
    if "map lanes" in c:
        return "lanes"
    if "map hazards" in c:
        return "hazards"
    if "map left object" in c:
        return "leftobjs"
    if "map right object" in c:
        return "rightobjs"
    if "perp's mugshot" in c or "perp mugshot" in c:
        return "perp_mugshot"
    if "pilot's mugshot" in c or "pilot mugshot" in c:
        return "pilot_mugshot"
    if "lod table" in c:
        return "lod_table"
    if "hittable" in c:
        return "hittable_objects"
    if "helicopter data" in c:
        return "helicopter"
    if "object graphic definitions" in c:
        if "(right)" in c:
            return "obj_defs_right"
        if "(left)" in c:
            return "obj_defs_left"
        return "obj_defs_right"
    if "graphic definition for object" in c:
        return "obj_defs_entry"
    if "arrest messages" in c:
        return "arrest_msgs"
    if "nancy's perp description" in c or "perp description" in c:
        return "perp_desc"
    if "stretchy graphic" in c:
        return "stretchy"
    if "draw_object_left/right graphic data" in c:
        # Plain depthset_t blocks (2-byte bitmaps ptr + 10 depth/offset pairs)
        # used directly by non-stretchy draw_object_left/right, as opposed to
        # depthsets reached via a stretchy_t layer. Same 22-byte-block shape,
        # so it is handled by the same stretchy/depthset pipeline.
        return "stretchy"
    if "bitmap data" in c or "{bitmap" in c or "pointed to by helicopter" in c:
        return "bitmap"
    return "unknown"


# ── Section collector ─────────────────────────────────────────────────────────


class Section:
    def __init__(self, stype: str, start_addr: int, header_comment: str):
        self.stype = stype
        self.start_addr = start_addr
        self.header_comment = header_comment
        self.records: List[SkoolRecord] = []

    @property
    def bytes_flat(self) -> List[int]:
        """All bytes in address order, treating DEFW as 2 bytes LE."""
        result = []
        for rec in self.records:
            if rec.rtype == "W":
                for w in rec.values:
                    result.append(w & 0xFF)
                    result.append((w >> 8) & 0xFF)
            else:
                result.extend(rec.values)
        return result

    @property
    def words_with_annots(self) -> List[Tuple[int, int, int]]:
        """List of (raw_word, absolute_addr_or_-1, record_addr)."""
        result = []
        for rec in self.records:
            if rec.rtype == "W":
                for idx, w in enumerate(rec.values):
                    ann = rec.annotations[idx] if idx < len(rec.annotations) else -1
                    result.append((w, ann, rec.addr + idx * 2))
        return result


def split_into_sections(
    records: List[SkoolRecord],
    section_comments: List[str],
    fresh_header_flags: List[bool],
) -> List[Section]:
    """Group records by section, detecting section type from comments."""
    sections: List[Section] = []
    cur: Optional[Section] = None
    prev_type = None
    prev_cmt = None

    for rec, cmt, fresh in zip(records, section_comments, fresh_header_flags):
        stype = classify_section(cmt)
        # Sub-entries of an obj_defs container are absorbed into the parent section
        # rather than starting a new one; the right→left transition still splits
        # because obj_defs_left != obj_defs_right.
        is_obj_entry = stype == "obj_defs_entry" and prev_type in (
            "obj_defs_right",
            "obj_defs_left",
        )
        # Start a new section when the comment is non-empty AND either:
        #   - the type has changed, OR
        #   - the comment itself has changed (e.g. two consecutive lod_tables), OR
        #   - a new header comment was seen right here, even if its text is
        #     identical to the still-pending previous header (e.g. two
        #     consecutive "LOD table for 'stretchy'" tables at different
        #     addresses - without this, the second table's records get
        #     silently absorbed into the first table's section)
        if cmt and (stype != prev_type or cmt != prev_cmt or fresh) and not is_obj_entry:
            if cur is not None:
                sections.append(cur)
            cur = Section(stype or "unknown", rec.addr, cmt)
            prev_type = stype
            prev_cmt = cmt
        if cur is None:
            cur = Section("unknown", rec.addr, cmt)
        cur.records.append(rec)

    if cur is not None:
        sections.append(cur)
    return sections


# ── Code generators ───────────────────────────────────────────────────────────


def array_name(stage: int, stype: str, addr: int) -> str:
    """Generate a C array name from stage, section type and address."""
    prefixes = {
        "backdrop": f"stage{stage}_backdrop",
        "curvature": f"stage{stage}_map_curv_{addr:04X}",
        "height": f"stage{stage}_map_height_{addr:04X}",
        "lanes": f"stage{stage}_map_lanes_{addr:04X}",
        "hazards": f"stage{stage}_map_hazards_{addr:04X}",
        "leftobjs": f"stage{stage}_map_lobjs_{addr:04X}",
        "rightobjs": f"stage{stage}_map_robjs_{addr:04X}",
        "perp_mugshot": f"stage{stage}_perp_face",
        "pilot_mugshot": f"stage{stage}_pilot_mugshot",
        "bitmap": f"stage{stage}_bitmap_{addr:04X}",
        "lod_table": f"stage{stage}_lods_{addr:04X}",
        "hazard_lods": f"stage{stage}_hazard_lods_{addr:04X}",
        "arrest_msgs": f"stage{stage}_arrest_messages_{addr:04X}",
        "perp_desc": f"stage{stage}_perp_description",
        "obj_defs": f"stage{stage}_obj_defs_{addr:04X}",
        "obj_defs_right": f"stage{stage}_right_obj_defs_{addr:04X}",
        "obj_defs_left": f"stage{stage}_left_obj_defs_{addr:04X}",
        "hittable_objects": f"stage{stage}_hittable_objects_{addr:04X}",
        "helicopter": f"stage{stage}_helicopter_{addr:04X}",
        "lodaddrs": f"stage{stage}_lod_addrs_{addr:04X}",
        "stretchy": f"stage{stage}_stretchy_{addr:04X}",
    }
    return prefixes.get(stype, f"stage{stage}_data_{addr:04X}")


def byte_to_pixel(b: int) -> str:
    """Return the Pixels.h macro name for b (e.g. 0xA5 → 'X_X__X_X')."""
    return "".join("X" if (b >> (7 - i)) & 1 else "_" for i in range(8))


def sprite_layout(
    addr: int,
    size: int,
    shapes: Dict[int, Optional[Tuple[int, int, bool]]],
    fallback_per_row: int = 8,
) -> Tuple[str, int]:
    """How to lay out a sprite block: (length expression, elements per row).

    A sprite is width bytes across by height rows, so the array reads as the
    picture when it is written width per row and declared '6 * 31' rather than
    '186'. A masked sprite interleaves a mask byte with each pixel byte, giving
    'width * 2 * height' and twice as many elements per row.

    Falls back to the flat byte count when the shape is unknown or does not
    account for every byte of the block -- a block that a refused cut left
    holding two overlapping sprites is longer than either sprite's own extent,
    and claiming otherwise would understate the array.
    """
    shape = shapes.get(addr)
    if shape is None:
        return str(size), fallback_per_row
    width, height, masked = shape
    per_row = width * 2 if masked else width
    if per_row * height != size:
        return str(size), fallback_per_row
    return (
        f"{width} * 2 * {height}" if masked else f"{width} * {height}"
    ), per_row


def emit_raw_array(
    name: str,
    data: List[int],
    per_row: int = 8,
    z80_addr: int = 0,
    use_pixels: bool = False,
    length_expr: Optional[str] = None,
) -> List[str]:
    """Emit a 'static const u8 name[] = { ... };' array.

    When use_pixels is True the bytes are written using the Pixels.h macro
    names (e.g. X_X__X_X) instead of hex, and the block is wrapped in
    clang-format off/on so the visual rows are not reformatted.

    length_expr overrides the declared length, so a sprite can carry its shape
    ('6 * 31') instead of a bare byte count.
    """
    lines = []
    lines.append(f"// ${z80_addr:04X}")
    if use_pixels:
        lines.append("// clang-format off")
    lines.append(f"static const u8 {name}[{length_expr or len(data)}] = {{")
    for i in range(0, len(data), per_row):
        chunk = data[i : i + per_row]
        if use_pixels:
            row = ", ".join(byte_to_pixel(b) for b in chunk)
        else:
            row = ", ".join(f"0x{b:02X}" for b in chunk)
        lines.append(f"  {row},")
    lines.append("};")
    if use_pixels:
        lines.append("// clang-format on")
    return lines


def emit_map_section(
    stage: int,
    stype: str,
    sec: Section,
    obj_names: List[str],
    bank_offset: int,
    abs_to_name: Dict[int, str],
) -> Tuple[List[str], Dict[int, str]]:
    """
    Decode a map section into C macros.
    Returns (lines, goto_map) where goto_map maps raw_defw_value → array_name.
    Only GOTO/SPLIT targets are added to goto_map (not the section's own address).
    """
    name = array_name(stage, stype, sec.start_addr)
    goto_map: Dict[int, str] = {}

    raw = sec.bytes_flat
    # Collect DEFW records for GOTO/SPLIT extraction
    defw_seq: List[Tuple[int, int]] = []  # (raw_word, abs_addr_or_-1)
    for rec in sec.records:
        if rec.rtype == "W":
            for idx, w in enumerate(rec.values):
                ann = rec.annotations[idx] if idx < len(rec.annotations) else -1
                defw_seq.append((w, ann))

    # Choose decoder
    if stype == "curvature":
        decoded = decode_curvature(raw)
    elif stype == "height":
        decoded = decode_height(raw)
    elif stype == "lanes":
        decoded = decode_lanes(raw)
    elif stype == "hazards":
        decoded = decode_hazards(raw)
    elif stype in ("leftobjs", "rightobjs"):
        decoded = decode_objects(raw, stage, obj_names)
    else:
        decoded = [f"/* 0x{b:02X} */" for b in raw]

    content_lines = [l for l in decoded if l is not None]

    # Determine terminal command: find the LAST ESC+{0,1,2} in the stream.
    # For lanes (paired bytes), ESC is only valid at even positions.
    # For hazards, there may be multiple ESC sequences; the terminal is always last.
    terminal_cmd = None
    if stype == "lanes":
        for i in range(0, len(raw) - 1, 2):
            if raw[i] == 0 and raw[i + 1] in (0, 1, 2):
                terminal_cmd = raw[i + 1]
    else:
        for i in range(len(raw) - 1):
            if raw[i] == 0 and raw[i + 1] in (0, 1, 2):
                terminal_cmd = raw[i + 1]

    if terminal_cmd == 0 and defw_seq:
        # GOTO or LOOP — single DEFW target
        raw_tgt, abs_tgt = defw_seq[0]
        if abs_tgt < 0:
            abs_tgt = raw_tgt + bank_offset
        tgt_name = abs_to_name.get(abs_tgt)  # None if unresolved
        goto_map[raw_tgt] = tgt_name
        content_lines.append(f"MAP_CMD_GOTO(0x{raw_tgt:04X})")
    elif terminal_cmd == 2 and len(defw_seq) >= 2:
        # SPLIT — two DEFW targets
        raw_l, abs_l = defw_seq[0]
        raw_r, abs_r = defw_seq[1]
        if abs_l < 0:
            abs_l = raw_l + bank_offset
        if abs_r < 0:
            abs_r = raw_r + bank_offset
        name_l = abs_to_name.get(abs_l)  # None if unresolved
        name_r = abs_to_name.get(abs_r)  # None if unresolved
        goto_map[raw_l] = name_l
        goto_map[raw_r] = name_r
        content_lines.append(f"MAP_CMD_SPLIT(0x{raw_l:04X}, 0x{raw_r:04X})")
    elif terminal_cmd == 1:
        content_lines.append("MAP_CMD_FORK_END")

    lines = [f"// ${sec.start_addr:04X}"]
    lines.append(f"static const u8 {name}[] = {{")
    for cl in content_lines:
        lines.append(f"  {cl}")
    lines.append("};")
    return lines, goto_map


def resolve_section_ptr(
    abs_addr: int,
    abs_to_name: Dict[int, str],
    abs_to_depthset_name: Optional[Dict[int, str]] = None,
) -> Optional[str]:
    """Resolve abs_addr to &array[offset] using registered section starts.

    depthset_t structs are checked first and matched exactly (not by nearest
    preceding base + byte offset) since they are single structs, not arrays;
    a byte-offset match against them would index past the struct.
    """
    if abs_to_depthset_name and abs_addr in abs_to_depthset_name:
        return f"&{abs_to_depthset_name[abs_addr]}"
    if abs_addr in abs_to_name:
        return f"&{abs_to_name[abs_addr]}[0]"
    candidates = [(base, nm) for base, nm in abs_to_name.items() if base <= abs_addr]
    if candidates:
        base, nm = max(candidates, key=lambda x: x[0])
        return f"&{nm}[{abs_addr - base}]"
    return None


_OBJ_ENTRY_Z80_SIZE = 7  # 3 × DEFB + 2 × DEFW per obj_t in Z80
_OBJ_ARG_Z80_OFFSET = 3  # byte offset of the .arg DEFW within a Z80 obj entry


def resolve_obj_ptr(abs_addr: int, abs_to_name: Dict[int, str]) -> Optional[str]:
    """Resolve a Z80 address into a typed &obj_array[idx] or &obj_array[idx].arg.

    Handles the [-1] entry (one entry before the array start) that the Z80
    perstage data uses for the handlers/objects pointers.
    """
    obj_bases = [(b, nm) for b, nm in abs_to_name.items() if "_obj_defs_" in nm]
    best: Optional[Tuple[int, str, int]] = None
    best_dist = 0
    for base, nm in obj_bases:
        diff = abs_addr - base
        if diff < -_OBJ_ENTRY_Z80_SIZE or diff > 300:
            continue
        # Python % is always non-negative, matching floor division
        if diff % _OBJ_ENTRY_Z80_SIZE not in (0, _OBJ_ARG_Z80_OFFSET):
            continue
        dist = abs(diff)
        if best is None or dist < best_dist:
            best = (base, nm, diff)
            best_dist = dist
    if best is None:
        return None
    _, nm, diff = best
    entry_idx = diff // _OBJ_ENTRY_Z80_SIZE
    field_off = diff % _OBJ_ENTRY_Z80_SIZE
    if field_off == _OBJ_ARG_Z80_OFFSET:
        return f"&{nm}[{entry_idx}].arg"
    return f"&{nm}[{entry_idx}]"


def resolve_bitmap_ref(abs_addr: int, bitmap_names: Dict[int, str], stage: int) -> str:
    """Return a C expression (&array[offset]) for a bitmap at abs_addr."""
    if abs_addr in bitmap_names:
        return f"&{bitmap_names[abs_addr]}[0]"
    candidates = [
        (base, name) for base, name in bitmap_names.items() if base <= abs_addr
    ]
    if candidates:
        base, name = max(candidates, key=lambda x: x[0])
        return f"&{name}[{abs_addr - base}]"
    return f"&stage{stage}_bitmap_{abs_addr:04X}[0]"


def emit_perp_description(stage: int, sec: Section) -> List[str]:
    """Decode a perp_desc section into CHATTERCHR/CHATTERSTR/CHATTERCMD/CHATTERBLK."""
    data = sec.bytes_flat
    nm = f"stage{stage}_perp_description"

    # Expected layout (12 raw bytes):
    #   [0]      DEFB  char_id
    #   [1..8]   4 × DEFW  string pointers (replaced by CHATTERSTR_PERP_DESC_1..4)
    #   [9]      DEFB  command byte
    #   [10..11] DEFW  next-block address (always CHATTERBLK_HEROES_ACKNOWLEDGE)
    if len(data) < 12:
        return emit_raw_array(nm, data, 8, sec.start_addr)

    char_name = CHATTERCHR_NAMES.get(data[0], f"({data[0]})")
    cmd_name = CHATTERCMD_NAMES.get(data[9], f"0x{data[9]:02X}")

    lines = [f"// ${sec.start_addr:04X}"]
    lines.append(f"static const u8 {nm}[7] = {{")
    lines.append(f"  {char_name},")
    lines.append(f"  CHATTERSTR_PERP_DESC_1,")
    lines.append(f"  CHATTERSTR_PERP_DESC_2,")
    lines.append(f"  CHATTERSTR_PERP_DESC_3,")
    lines.append(f"  CHATTERSTR_PERP_DESC_4,")
    lines.append(f"  {cmd_name},")
    lines.append(f"  CHATTERBLK_HEROES_ACKNOWLEDGE")
    lines.append("};")
    return lines


def emit_hittable_array(
    stage: int, sec: Section, abs_to_name: Dict[int, str]
) -> Tuple[List[str], int]:
    """Emit a hittable_t array from alternating DEFB (width) + DEFW (bitmaps) records."""
    nm = array_name(stage, "hittable_objects", sec.start_addr)
    entries = []
    recs = sec.records
    i = 0
    while i + 1 < len(recs):
        if recs[i].rtype == "B" and recs[i + 1].rtype == "W":
            width = recs[i].values[0] if recs[i].values else 0
            w_rec = recs[i + 1]
            raw_w = w_rec.values[0] if w_rec.values else 0
            ann = w_rec.annotations[0] if w_rec.annotations else -1
            if ann < 0 and raw_w:
                ann = raw_w
            ref = resolve_section_ptr(ann, abs_to_name) if ann >= 0 else None
            # Only use ref when it points to a LOD table (const bitmap_t array).
            if ref and "_lods_" not in ref:
                ref = None
            bitmaps_str = ref if ref else f"NULL /* TODO: bitmaps ${ann:04X} */"
            entries.append(f"  {{ {width}, {bitmaps_str} }}")
        i += 2
    n = len(entries)
    lines = [f"// ${sec.start_addr:04X}", f"static const hittable_t {nm}[{n}] = {{"]
    for e in entries:
        lines.append(e + ",")
    lines.append("};")
    return lines, n


def emit_obj_array(
    stage: int,
    sec: Section,
    abs_to_name: Dict[int, str],
    abs_to_depthset_name: Dict[int, str],
) -> Tuple[List[str], int]:
    """Emit an obj_t array, one entry per _OBJ_ENTRY_Z80_SIZE bytes.

    Walking by byte stride rather than by record shape matters: a null argument
    pointer carries no `[$XXXX]` annotation for coalesce_split_words to latch
    onto, so stage 2's unused object 2 still arrives as `DEFB $00 / DEFB $00`
    where the others arrive as one DEFW. Matching on the record pattern
    B,B,B,W,W skipped that entry and then lost sync with the ones after it.
    """
    nm = array_name(stage, sec.stype, sec.start_addr)

    # Flatten records to bytes, keeping each word's comment and annotation on
    # its low byte so the pointer fields can still be resolved by name.
    bytes_: List[Tuple[int, str, int]] = []
    for rec in sec.records:
        if rec.rtype == "W":
            for idx, val in enumerate(rec.values):
                ann = rec.annotations[idx] if idx < len(rec.annotations) else -1
                bytes_.append((val & 0xFF, rec.comment if idx == 0 else "", ann))
                bytes_.append(((val >> 8) & 0xFF, "", -1))
        else:
            for val in rec.values:
                bytes_.append((val, rec.comment, -1))

    entries = []
    i = 0
    while i + _OBJ_ENTRY_Z80_SIZE <= len(bytes_):
        b0 = bytes_[i][0]
        b1 = bytes_[i + 1][0]
        b2 = bytes_[i + 2][0]
        arg_cmt, arg_ann = bytes_[i + 3][1], bytes_[i + 3][2]
        arg_val = bytes_[i + 3][0] | (bytes_[i + 4][0] << 8)
        hdl_val = bytes_[i + 5][0] | (bytes_[i + 6][0] << 8)

        if arg_val == 0:
            arg_str = "NULL"
        else:
            arg_str = extract_arg_from_comment(arg_cmt)
            if not arg_str:
                if arg_ann >= 0:
                    ref = resolve_section_ptr(
                        arg_ann, abs_to_name, abs_to_depthset_name
                    )
                    arg_str = ref if ref else f"NULL /* TODO: arg ${arg_ann:04X} */"
                else:
                    arg_str = f"NULL /* TODO: arg ${arg_val:04X} */"

        if hdl_val == 0:
            hdl_str = "NULL"
        else:
            hdl_str = HANDLER_ADDRESS_MAP.get(
                hdl_val, f"NULL /* TODO: handler ${hdl_val:04X} */"
            )

        entries.append((b0, b1, b2, arg_str, hdl_str))
        i += _OBJ_ENTRY_Z80_SIZE

    n = len(entries)
    lines = [f"// ${sec.start_addr:04X}", f"static const obj_t {nm}[{n}] = {{"]
    for b0, b1, b2, arg_str, hdl_str in entries:
        if b0 == 0 and b1 == 0 and b2 == 0 and arg_str == "NULL" and hdl_str == "NULL":
            lines.append("  { 0, 0, 0, NULL, NULL },")
        else:
            lines.append(f"  {{ {b0}, {b1}, {b2}, {arg_str}, {hdl_str} }},")
    lines.append("};")
    return lines, n


def _count_all_lods(data: List[int], n_annotated: int) -> int:
    """Count valid LOD entries in data.

    Starts from n_annotated (entries marked with 'Width (bytes)' comments),
    caps at the first entry whose flags byte is outside {0,1,2,3}, then
    auto-extends through any further consecutive valid 7-byte groups.  Stops
    when flags ∉ {0,1,2,3} or height == 0 (both indicate bitmap pixel data
    rather than a LOD descriptor).
    """
    n = 0
    for j in range(n_annotated):
        off = j * 7
        if off + 1 >= len(data) or data[off + 1] not in (0, 1, 2, 3):
            break
        n += 1
    while n * 7 + 7 <= len(data):
        off = n * 7
        width  = data[off]
        flags  = data[off + 1]
        height = data[off + 2]
        if flags not in (0, 1, 2, 3) or width == 0 or width > 8 or height == 0:
            break
        n += 1
    return n


def _lod_entry_count(sec: "Section") -> int:
    """Number of 7-byte bitmap_t records at the head of a lod_table section."""
    data = sec.bytes_flat
    n_ann = sum(1 for rec in sec.records if "width (bytes)" in rec.comment.lower())
    if n_ann == 0:
        n_ann = len(data) // 7 if len(data) % 7 == 0 else 0
    return _count_all_lods(data, n_ann)


def _lod_local_offset(sec: "Section", bank_offset: int) -> int:
    """Bank offset the section's own pointers use.

    Where a section's internal pointers use a different base than the global
    bank_offset (stage 2's data in bank 1 uses +$8400, not +$6400), an
    annotated DEFW reveals the correct one.
    """
    for raw_w, ann_w, _ in sec.words_with_annots:
        if ann_w >= 0 and raw_w > 0:
            candidate = ann_w - raw_w
            if candidate != bank_offset and 0 < candidate < 0x10000:
                return candidate
    return bank_offset


def lod_sprite_spans(
    sec: "Section",
    bank_offset: int,
    shapes: Optional[Dict[int, Optional[Tuple[int, int, bool]]]] = None,
) -> List[Tuple[int, Optional[int]]]:
    """Every sprite a LOD table points at, as (z80_addr, byte_size_or_None).

    Both pointers of each entry are reported: the pre-shifted one addresses a
    second copy of the sprite, usually further down the same run. The size is
    width * height, doubled when masked because a mask byte is interleaved with
    each pixel byte. It is None for flag values whose layout is not modelled
    here, so callers must not assume a size is always known.

    When `shapes` is given it collects z80_addr -> (width, height, masked) so
    the sprite's array can be emitted in its true shape. Two LOD entries that
    point at one address while disagreeing about its shape poison the entry to
    None: there is no way to tell which of them describes the layout.
    """
    data = sec.bytes_flat
    n_lods = _lod_entry_count(sec)
    local_offset = _lod_local_offset(sec, bank_offset)
    wwa = sec.words_with_annots

    spans: List[Tuple[int, Optional[int]]] = []
    for i in range(n_lods):
        off = i * 7
        if off + 6 >= len(data):
            break
        width, flags, height = data[off], data[off + 1], data[off + 2]
        if flags == BITMAP_FLAG_DEFAULT_VAL:
            size: Optional[int] = width * height
        elif flags == BITMAP_FLAG_MASKED_VAL:
            size = width * height * 2
        else:
            size = None
        for half in (0, 1):
            raw = (data[off + 4 + half * 2] << 8) | data[off + 3 + half * 2]
            ann = wwa[i * 2 + half][1] if i * 2 + half < len(wwa) else -1
            addr = ann if ann >= 0 else raw + local_offset
            spans.append((addr, size))
            if shapes is None or size is None:
                continue
            shape = (width, height, flags == BITMAP_FLAG_MASKED_VAL)
            shapes[addr] = shape if shapes.get(addr, shape) == shape else None
    return spans


def split_into_sprites(
    start: int, end: int, spans: List[Tuple[int, Optional[int]]]
) -> List[Tuple[int, int]]:
    """Cut the byte run [start, end) into one block per sprite it holds.

    In the Z80 a graphics run is undivided, but the LOD tables point at
    individual sprites inside it. Emitting the run as a single array leaves
    every entry indexing a shared blob -- `&stage2_bitmap_F768[338]` -- which
    records an offset but says nothing about where one sprite ends and the next
    begins, and renumbers every later sprite whenever an earlier one is
    re-measured. Cutting at each address pointed into the run gives one array
    per sprite, each named for its own Z80 address, so the entries read
    `&stage2_bitmap_F8AA[0]`.

    A cut is refused when it would fall inside a sprite that a LOD entry
    declares as reaching past it. Stage 5's entry at $D86C is 1x5 = 5 bytes but
    the next sprite starts 4 bytes later, so the two overlap in the original
    data. Indexing a shared blob absorbs that; separate arrays would not, since
    nothing guarantees how the compiler lays two arrays out. Such neighbours
    stay merged and resolve as an offset, exactly as before.

    Returns [(z80_addr, size), ...] covering [start, end) with no gaps, or an
    empty list when the run is empty.
    """
    if start >= end:
        return []

    cuts = {start}
    for addr, _ in spans:
        if start < addr < end:
            cuts.add(addr)

    # Drop cuts that fall strictly inside a sprite's declared extent.
    for addr, size in spans:
        if size is None or not (start <= addr < end):
            continue
        cuts -= {c for c in cuts if addr < c < addr + size}

    ordered = sorted(cuts)
    bounds = ordered + [end]
    return [(addr, bounds[i + 1] - addr) for i, addr in enumerate(ordered)]


def lod_bitmap_blocks(
    sec: "Section", spans: List[Tuple[int, Optional[int]]]
) -> Tuple[int, List[Tuple[int, int]]]:
    """Split the bitmap data trailing a LOD table into one block per sprite.

    `spans` must cover every LOD table in the stage, not just this section's:
    stage 2's $E953 run holds the sprites that the $E8FF and $E929 tables point
    at, so cutting it on its own table's pointers alone leaves those sprites
    merged into one 666-byte block.

    Returns (lod_end, blocks) where lod_end is the byte offset at which the
    descriptor records stop. See split_into_sprites for the cutting rule.
    """
    data = sec.bytes_flat
    lod_end = _lod_entry_count(sec) * 7
    if lod_end >= len(data):
        return lod_end, []
    return lod_end, split_into_sprites(
        sec.start_addr + lod_end, sec.start_addr + len(data), spans
    )


def emit_lod_table(
    stage: int,
    sec: Section,
    bank_offset: int,
    bitmap_names: Dict[int, str],
    spans: List[Tuple[int, Optional[int]]],
    shapes: Dict[int, Optional[Tuple[int, int, bool]]],
) -> Tuple[List[str], int]:
    """
    Decode a LOD table (7-byte bitmap_t records).
    Counts actual LOD entries from 'Width (bytes)' inline comments so that
    bitmap data appended to the same section is handled separately.  Any
    additional valid 7-byte groups that immediately follow the annotated
    entries are auto-detected and included (see _count_all_lods).

    The trailing bitmap data is emitted as one array per sprite rather than one
    blob for the whole run -- see lod_bitmap_blocks.

    Returns (C lines, n_lod_entries).
    """
    data = sec.bytes_flat
    n_lods = _lod_entry_count(sec)
    if n_lods == 0:
        return (
            emit_raw_array(
                array_name(stage, "lod_table", sec.start_addr),
                data,
                7,
                sec.start_addr,
            ),
            0,
        )

    # words_with_annots contains only the DEFW records (not DEFB).
    # Each LOD entry has exactly 2 DEFWs (bitmap addr + pre-shifted addr),
    # so LOD entry i uses wwa[i*2] and wwa[i*2+1].
    wwa = sec.words_with_annots
    local_offset = _lod_local_offset(sec, bank_offset)

    lod_end, blocks = lod_bitmap_blocks(sec, spans)

    # Register the sprite blocks before rendering any entry: resolve_bitmap_ref
    # picks the nearest base at or below the target, so an unregistered split
    # would silently resolve as an offset into the block before it.
    for addr, size in blocks:
        bitmap_names[addr] = f"stage{stage}_bitmap_{addr:04X}"

    name = array_name(stage, "lod_table", sec.start_addr)
    lines = [f"// ${sec.start_addr:04X}"]
    lines.append(f"static const bitmap_t {name}[{n_lods}] = {{")
    for i in range(n_lods):
        off = i * 7
        if off + 6 >= len(data):
            break
        width = data[off]
        flags = data[off + 1]
        height = data[off + 2]
        data_raw = (data[off + 4] << 8) | data[off + 3]
        shft_raw = (data[off + 6] << 8) | data[off + 5]
        d_ann = wwa[i * 2][1] if i * 2 < len(wwa) else -1
        s_ann = wwa[i * 2 + 1][1] if i * 2 + 1 < len(wwa) else -1
        data_abs = d_ann if d_ann >= 0 else (data_raw + local_offset)
        shft_abs = s_ann if s_ann >= 0 else (shft_raw + local_offset)
        flag_str = BITMAP_FLAGS.get(flags, f"0x{flags:02X}")
        d_ref = resolve_bitmap_ref(data_abs, bitmap_names, stage)
        s_ref = resolve_bitmap_ref(shft_abs, bitmap_names, stage)
        lines.append(
            f"  {{ {width}, {flag_str}, {height}," f" {d_ref}, {s_ref} }},  // [{i}]"
        )
    lines.append("};")

    # One array per sprite in the bitmap data that follows the descriptors, each
    # shaped as the picture it holds.
    for addr, size in blocks:
        start = addr - sec.start_addr
        length_expr, per_row = sprite_layout(addr, size, shapes)
        lines.append("")
        lines.extend(
            emit_raw_array(
                bitmap_names[addr],
                data[start : start + size],
                per_row,
                addr,
                use_pixels=True,
                length_expr=length_expr,
            )
        )

    return lines, n_lods


def emit_arrest_messages(
    stage: int, sec: Section, defm_bytes: Dict[int, List[int]]
) -> Tuple[List[str], int]:
    """Decode an arrest_msgs section using delay/DRAWCHARSTYLE/TWOBYTES/EOS macros.

    Returns (C lines, total byte count including embedded DEFM text).
    """
    nm = f"stage{stage}_arrest_messages_{sec.start_addr:04X}"
    recs = sec.records
    n = len(recs)
    content: List[str] = []
    total = 0

    def val(r: SkoolRecord) -> int:
        return r.values[0] if r.values else 0

    def emit_text(data: List[int]) -> None:
        nonlocal total
        parts = []
        for j, b in enumerate(data):
            if j == len(data) - 1:  # last byte carries EOS in top bit
                base = b & 0x7F
                lit = (
                    f"'{chr(base)}'"
                    if 0x20 <= base <= 0x7E and chr(base) not in ("'", "\\")
                    else f"0x{base:02X}"
                )
                parts.append(f"{lit} | EOS")
            else:
                parts.append(
                    f"'{chr(b)}'"
                    if 0x20 <= b <= 0x7E and chr(b) not in ("'", "\\")
                    else f"0x{b:02X}"
                )
        content.append("  " + ", ".join(parts) + ",")
        total += len(data)

    i = 0
    # Initial delay
    if i < n and recs[i].rtype == "B":
        content.append(f"  {val(recs[i])},  // initial delay")
        total += 1
        i += 1

    while i < n:
        # Message block: DEFB delay, DEFB flags, DEFB attr, DEFW backbuf, DEFW attr_addr
        if (
            i + 4 < n
            and recs[i].rtype == "B"
            and recs[i + 1].rtype == "B"
            and recs[i + 2].rtype == "B"
            and recs[i + 3].rtype == "W"
            and recs[i + 4].rtype == "W"
        ):
            style = DRAWCHARSTYLE_NAMES.get(val(recs[i + 1]), str(val(recs[i + 1])))
            defm_addr = recs[i + 4].addr + 2
            content.append("")
            content.append(f"  {val(recs[i])},  // delay")
            content.append(f"  {style},")
            content.append(f"  {val(recs[i + 2])},  // attribute")
            content.append(f"  TWOBYTES(0x{val(recs[i + 3]):04X}),  // backbuf")
            content.append(f"  TWOBYTES(0x{val(recs[i + 4]):04X}),  // attr")
            total += 7  # 3 × DEFB + 2 × DEFW
            i += 5
            if defm_addr in defm_bytes:
                run = defm_bytes[defm_addr]
                emit_text(run)
                # The DEFM line itself never reaches recs, but where the control
                # file split the string SkoolKit's DEFB continuation lines do.
                # Step over whatever the text run already covered, or they get
                # decoded a second time as the section's end markers.
                text_end = defm_addr + len(run)
                while i < n and recs[i].addr < text_end:
                    i += 1
        else:
            # End markers: first byte = TRANSITIONCONTROL, last = DRAWOVERLAY_STOP
            content.append("")
            end_bytes = [b for r in recs[i:] for b in r.values]
            for k, b in enumerate(end_bytes):
                if k < len(end_bytes) - 1:
                    name = TRANSITIONCONTROL_NAMES.get(b, str(b))
                    content.append(f"  {name},  // transition_control")
                else:
                    content.append(f"  DRAWOVERLAY_STOP")
                total += 1
            break

    lines = [
        f"// ${sec.start_addr:04X}",
        "// clang-format off",
        f"static const u8 {nm}[{total}] = {{",
    ]
    lines.extend(content)
    lines.append("};")
    lines.append("// clang-format on")
    return lines, total


# ── Stretchy / depthset decoders ─────────────────────────────────────────────


def _is_stretchy_entries(sec: "Section") -> bool:
    """True if the section holds stretchy_t entries (first record is DEFB)."""
    return bool(sec.records) and sec.records[0].rtype == "B"


def _parse_stretchy_entries(
    sec: "Section", bank_offset: int
) -> List[List[Tuple[int, Optional[int]]]]:
    """Parse a stretchy-entries section into one or two sub-arrays.

    Each sub-array is a list of (type_byte, abs_depthset_addr_or_None) tuples.
    The END entry (type=1) carries None as the address.
    Two sub-arrays arise when right and left entries are packed consecutively.
    """
    sub_arrays: List[List[Tuple[int, Optional[int]]]] = []
    current: List[Tuple[int, Optional[int]]] = []
    records = sec.records
    i = 0
    while i < len(records):
        rec = records[i]
        if rec.rtype == "B":
            type_byte = rec.values[0] if rec.values else 0
            if type_byte == 1:  # STRETCHY_TYPE_END
                current.append((1, None))
                sub_arrays.append(current)
                current = []
                i += 1
            elif i + 1 < len(records) and records[i + 1].rtype == "W":
                w_rec = records[i + 1]
                raw_ptr = w_rec.values[0] if w_rec.values else 0
                ann = w_rec.annotations[0] if w_rec.annotations else -1
                abs_ptr = ann if ann >= 0 else (raw_ptr + bank_offset)
                current.append((type_byte, abs_ptr))
                i += 2
            else:
                i += 1
        else:
            i += 1
    if current:
        sub_arrays.append(current)
    return sub_arrays


def register_stretchy_names(
    stage: int,
    sections: List["Section"],
    bank_offset: int,
    abs_to_name: Dict[int, str],
    abs_to_depthset_name: Dict[int, str],
) -> None:
    """First-pass: register stretchy_t array names and depthset_t struct names."""
    for sec in sections:
        if sec.stype != "stretchy":
            continue
        addr = sec.start_addr
        if _is_stretchy_entries(sec):
            sub_arrays = _parse_stretchy_entries(sec, bank_offset)
            n_sa = len(sub_arrays)
            if n_sa == 2:
                abs_to_name[addr] = f"stage{stage}_stretchy_{addr:04X}_right"
                # The right sub-array occupies (N_entries × 3 + 1) Z80 bytes.
                n_non_term = sum(1 for t, _ in sub_arrays[0] if t != 1)
                right_z80_size = n_non_term * 3 + 1
                abs_to_name[addr + right_z80_size] = (
                    f"stage{stage}_stretchy_{addr:04X}_left"
                )
            elif n_sa > 2 and n_sa % 2 == 0:
                # Multiple right/left pairs packed consecutively.
                # Register each sub-array's actual start address so that obj_defs
                # pointers into the middle of the block resolve correctly.
                offset = 0
                for pi in range(n_sa // 2):
                    letter = "ABCDEFGHIJ"[pi]
                    for side, entries in (
                        ("right", sub_arrays[pi * 2]),
                        ("left",  sub_arrays[pi * 2 + 1]),
                    ):
                        abs_to_name[addr + offset] = (
                            f"stage{stage}_stretchy_{addr:04X}_{letter}_{side}"
                        )
                        n_non_term = sum(1 for t, _ in entries if t != 1)
                        offset += n_non_term * 3 + 1
            else:
                # 1 sub-array (normal) or odd count (unsupported, falls back to raw u8)
                abs_to_name[addr] = f"stage{stage}_stretchy_{addr:04X}"
        else:
            data = sec.bytes_flat
            n = len(data) // 22
            for i in range(n):
                dep_addr = addr + i * 22
                abs_to_depthset_name[dep_addr] = f"stage{stage}_depthset_{dep_addr:04X}"


def emit_stretchy_typed(
    stage: int,
    sec: "Section",
    bank_offset: int,
    abs_to_name: Dict[int, str],
    abs_to_depthset_name: Dict[int, str],
) -> Tuple[List[str], List[str]]:
    """Emit stretchy_t arrays or depthset_t structs for a stretchy section.

    Returns (lines, fwd_decls).
    """
    lines: List[str] = [f"// ${sec.start_addr:04X}"]
    fwd_decls: List[str] = []

    if _is_stretchy_entries(sec):
        sub_arrays = _parse_stretchy_entries(sec, bank_offset)
        n_sa = len(sub_arrays)
        if n_sa == 1:
            suffix_entries = [("", sub_arrays[0])]
        elif n_sa == 2:
            suffix_entries = [("_right", sub_arrays[0]), ("_left", sub_arrays[1])]
        elif n_sa % 2 == 0:
            # Multiple right/left pairs: A_right, A_left, B_right, B_left, …
            suffix_entries = []
            for pi in range(n_sa // 2):
                letter = "ABCDEFGHIJ"[pi]
                suffix_entries.append((f"_{letter}_right", sub_arrays[pi * 2]))
                suffix_entries.append((f"_{letter}_left",  sub_arrays[pi * 2 + 1]))
        else:
            # Unexpected odd structure — fall back to raw bytes
            data = sec.bytes_flat
            nm = f"stage{stage}_stretchy_{sec.start_addr:04X}"
            lines.extend(emit_raw_array(nm, data, 8, sec.start_addr))
            fwd_decls.append(f"static const u8 {nm}[{len(data)}];")
            return lines, fwd_decls
        for suffix, entries in suffix_entries:
            nm = f"stage{stage}_stretchy_{sec.start_addr:04X}{suffix}"
            n = len(entries)
            lines.append(f"static const stretchy_t {nm}[{n}] = {{")
            for type_byte, abs_ptr in entries:
                type_name = STRETCHY_TYPE_NAMES.get(type_byte, f"({type_byte})")
                if type_byte == 1:
                    lines.append(f"  {{ {type_name}, NULL }},")
                else:
                    dep_nm = (
                        abs_to_depthset_name.get(abs_ptr)
                        if abs_ptr is not None
                        else None
                    )
                    if dep_nm is None and abs_ptr is not None:
                        dep_nm = COMMON_DEPTHSET_RAW_MAP.get(
                            abs_ptr - bank_offset
                        )
                    if dep_nm:
                        lines.append(f"  {{ {type_name}, &{dep_nm} }},")
                    else:
                        lines.append(
                            f"  {{ {type_name},"
                            f" NULL /* TODO: depthset ${abs_ptr:04X} */ }},"
                        )
            lines.append("};")
            lines.append("")
            fwd_decls.append(f"static const stretchy_t {nm}[{n}];")
    else:
        # Depthset block: groups of 22 bytes (2-byte bitmap ptr + 10 × 2-byte pairs).
        # Each depthset is 11 words; word 0 is the bitmaps ptr.
        data = sec.bytes_flat
        wwa = sec.words_with_annots  # (raw, ann, addr) per word
        n = len(data) // 22
        remainder = len(data) % 22
        for i in range(n):
            dep_addr = sec.start_addr + i * 22
            off = i * 22
            raw_ptr = data[off] | (data[off + 1] << 8)
            bm_wwa = wwa[i * 11] if i * 11 < len(wwa) else None
            ann = bm_wwa[1] if bm_wwa else -1
            abs_ptr = ann if ann >= 0 else (raw_ptr + bank_offset)
            bitmaps_nm = abs_to_name.get(abs_ptr)
            if bitmaps_nm:
                bitmaps_ref = f"&{bitmaps_nm}[0]"
            else:
                # abs_ptr may be an offset into an existing LOD table.
                # Find the nearest preceding lod_table entry and compute the
                # index as (abs_ptr - base) / 7 (7 bytes per Z80 LOD entry).
                lod_prefix = f"stage{stage}_lods_"
                lod_candidates = [
                    (base, nm) for base, nm in abs_to_name.items()
                    if nm.startswith(lod_prefix) and base <= abs_ptr
                ]
                if lod_candidates:
                    lod_base, lod_nm = max(lod_candidates, key=lambda x: x[0])
                    lod_idx = (abs_ptr - lod_base) // 7
                    bitmaps_ref = f"&{lod_nm}[{lod_idx}]"
                else:
                    bitmaps_ref = f"NULL /* TODO: bitmaps ${abs_ptr:04X} */"
            nm = f"stage{stage}_depthset_{dep_addr:04X}"
            lines.append(f"static const depthset_t {nm} = {{")
            lines.append(f"  {bitmaps_ref},")
            lines.append("  {")
            for j in range(10):
                depth = data[off + 2 + j * 2]
                offset_val = data[off + 3 + j * 2]
                lines.append(f"    0x{depth:02X}, 0x{offset_val:02X},")
            lines.append("  }")
            lines.append("};")
            lines.append("")
            fwd_decls.append(f"static const depthset_t {nm};")
        if remainder:
            # Stray bytes after last complete depthset — emit as raw.
            rem_addr = sec.start_addr + n * 22
            rem_nm = f"stage{stage}_stretchy_{rem_addr:04X}"
            rem_data = data[n * 22 :]
            lines.extend(emit_raw_array(rem_nm, rem_data, 8, rem_addr))
            lines.append("")
            fwd_decls.append(f"static const u8 {rem_nm}[{len(rem_data)}];")

    return lines, fwd_decls


# ── Stage struct emitter ──────────────────────────────────────────────────────


def emit_stage_struct(
    stage: int, sections: List[Section], abs_to_name: Dict[int, str], bank_offset: int
) -> List[str]:
    """Emit const stage_t stageN = { ... };"""

    def find_first(stype: str) -> Optional[Section]:
        return next((s for s in sections if s.stype == stype), None)

    backdrop_sec = find_first("backdrop")
    perstage_sec = find_first("perstage")
    difficulty_sec = find_first("difficulty")
    setupdata_sec = find_first("setupdata")
    attractdata_sec = find_first("attractdata")
    perp_sec = find_first("perp_mugshot")
    pilot_sec = find_first("pilot_mugshot")
    lodaddrs_sec = find_first("lodaddrs")

    lines: List[str] = []
    addr = backdrop_sec.start_addr if backdrop_sec else 0
    lines.append(f"// ${addr:04X}")
    lines.append(f"const stage_t stage{stage} = {{")

    # Backdrop – embedded inline
    if backdrop_sec:
        data = backdrop_sec.bytes_flat
        lines.append(f"  /* ${backdrop_sec.start_addr:04X} backdrop */")
        lines.append("  // clang-format off")
        lines.append("  {")
        for i in range(0, len(data), 10):
            row = data[i : i + 10]
            lines.append("    " + ", ".join(byte_to_pixel(b) for b in row) + ",")
        lines.append("  },")
        lines.append("  // clang-format on")
    else:
        lines.append("  { 0 },  /* TODO: backdrop */")

    # perstage: perp face attributes, pilot mugshot, ground colour
    perstage_addr = perstage_sec.start_addr if perstage_sec else 0
    lines.append(f"  /* ${perstage_addr:04X} perstage */")
    if perp_sec:
        lines.append(f"  &stage{stage}_perp_face[FACEBITMAPBYTES],")
    else:
        lines.append(f"  NULL,  /* no perp face on this stage */")
    if pilot_sec:
        lines.append(f"  &stage{stage}_pilot_mugshot[0],")
    else:
        lines.append(f"  NULL,  /* no pilot mugshot on this stage */")
    if perstage_sec:
        pws = perstage_sec.words_with_annots
        gc = pws[2][0] if len(pws) > 2 else 0
        lines.append(f"  0x{gc:04X},")
    else:
        lines.append("  0,  /* TODO: ground_colour */")

    # Pointer fields (perstage words 3..13)
    # Fields 1-6 point into obj_t arrays (possibly at [-1] or [-1].arg);
    # use resolve_obj_ptr for those and resolve_section_ptr for the rest.
    _OBJ_FIELDS = {1, 2, 3, 4, 5, 6}
    pws = perstage_sec.words_with_annots if perstage_sec else []
    _HELI_FIELDS = {"addrof_helicopter_stuff_1", "addrof_helicopter_stuff_2"}
    for i, field in enumerate(PERSTAGE_PTR_FIELDS):
        abs_a = pws[i + 3][1] if i + 3 < len(pws) else -1
        if field in _HELI_FIELDS:
            # Helicopter data is raw bytes, not yet decoded to heli_bitmap_t.
            # Always emit NULL to avoid pointer type mismatch; draw_helicopter
            # has a null guard for this.
            hint = f" (${abs_a:04X})" if abs_a >= 0 else ""
            lines.append(f"  NULL,  /* TODO: {field}{hint} - raw data, not decoded yet */")
            continue
        if abs_a >= 0:
            ref = (
                resolve_obj_ptr(abs_a, abs_to_name)
                if i in _OBJ_FIELDS
                else resolve_section_ptr(abs_a, abs_to_name)
            )
            if ref:
                lines.append(f"  {ref},  /* {field} */")
                continue
        hint = f" (${abs_a:04X})" if abs_a >= 0 else ""
        lines.append(f"  NULL,  /* TODO: {field}{hint} */")

    lines.append("")
    lws = lodaddrs_sec.words_with_annots if lodaddrs_sec else []

    def lod_ref(idx: int, as_array_ptr: bool = False) -> str:
        """Return a C expression for lodaddrs word idx.

        as_array_ptr=True: field type is const bitmap_t (*)[SPRITE_FRAMES] —
        emit &array (address of the whole array) rather than &array[N].
        """
        if idx >= len(lws):
            return "NULL"
        raw_w, abs_a, _ = lws[idx]
        if raw_w == 0:
            return "NULL"
        if abs_a < 0:
            abs_a = raw_w + bank_offset
        ref = resolve_section_ptr(abs_a, abs_to_name)
        # Only use ref if it points into a LOD table (const bitmap_t array).
        # Pointers into raw byte arrays (map_robjs, bitmap data) are
        # incompatible with const bitmap_t *.
        if ref and "_lods_" not in ref:
            ref = None
        if not ref:
            return f"NULL  /* TODO: ${abs_a:04X} */"
        if as_array_ptr:
            # Convert "&name[N]" → "(const bitmap_t (*)[SPRITE_FRAMES])&name[N]"
            ref = f"(const bitmap_t (*)[SPRITE_FRAMES]){ref}"
        return ref

    lines.append(f"  {lod_ref(0, as_array_ptr=True)},  /* bitmaps_stones */")
    lines.append(f"  {lod_ref(1, as_array_ptr=True)},  /* bitmaps_dust */")
    lines.append(f"  {lod_ref(2)},  /* bitmaps_perp_car */")
    vehicles = ", ".join(lod_ref(3 + j) for j in range(4))
    lines.append(f"  {{ {vehicles} }},  /* bitmaps_vehicles */")
    lines.append("")

    # Difficulty: car_spawn_delay, perp_lane_change_base, perp_approach_base
    if difficulty_sec:
        diff = difficulty_sec.bytes_flat
        lines.append(f"  /* ${difficulty_sec.start_addr:04X} difficulty */")
        lines.append(f"  {diff[0] if diff else 0},  /* car_spawn_delay */")
        lines.append(
            f"  {diff[1] if len(diff) > 1 else 0},  /* perp_lane_change_base */"
        )
        lines.append(f"  {diff[2] if len(diff) > 2 else 0},  /* perp_approach_base */")
    else:
        lines.append("  0,  /* TODO: car_spawn_delay */")
        lines.append("  0,  /* TODO: perp_lane_change_base */")
        lines.append("  0,  /* TODO: perp_approach_base */")
    lines.append("")

    def emit_scenedata(sec: Optional[Section], label: str) -> List[str]:
        if not sec:
            return [f"  {{ 0 }},  /* TODO: {label} */"]
        ws = sec.words_with_annots
        sl = [f"  /* ${sec.start_addr:04X} {label} */", "  {"]
        sl.append(f"    {ws[0][0] if ws else 0},")
        for idx in range(1, 7):
            if idx < len(ws):
                raw_w, abs_a, _ = ws[idx]
                if abs_a < 0:
                    abs_a = raw_w + bank_offset
                nm = abs_to_name.get(abs_a + 1)
                sl.append(
                    f"    &{nm}[-1],"
                    if nm
                    else f"    NULL,  /* TODO: ${abs_a + 1:04X} */"
                )
            else:
                sl.append(f"    NULL,  /* TODO: missing word {idx} */")
        sl.append("  },")
        return sl

    lines.extend(emit_scenedata(setupdata_sec, "setupdata"))
    lines.append("")
    lines.extend(emit_scenedata(attractdata_sec, "attractdata"))
    lines.append("")
    lines.append(f"  stage{stage}_chatter_strings")
    lines.append("};")
    return lines


# ── Main conversion ───────────────────────────────────────────────────────────


def convert(skool_path: str, stage: int, obj_names: List[str]) -> None:
    records, section_comments, fresh_header_flags = parse_skool(skool_path)
    bank_offset = compute_bank_offset(records)
    defm_strings, defm_bytes = parse_defm_map(skool_path)
    sections = split_into_sections(records, section_comments, fresh_header_flags)

    # When a bank contains multiple stages (e.g. bank 1 has stages 1+2, bank 6
    # has stages 3+4), keep only sections whose [Stage N] header matches the
    # requested stage. Untagged sections (no [Stage N] header) belong to
    # whichever tagged stage's address region they physically fall inside:
    # the region for stage T runs from T's own earliest tagged address up to
    # (but not including) the next tagged stage's earliest address. Without
    # this, untagged shared assets (car/hazard/stretchy sprites) get
    # duplicated into every stage sharing the same bank file.
    _stage_re = re.compile(r"\[Stage (\d+)\]")
    def _section_stage(sec: Section) -> int:
        m = _stage_re.match(sec.header_comment)
        return int(m.group(1)) if m else 0

    stage_starts = sorted(
        set((s.start_addr, _section_stage(s)) for s in sections if _section_stage(s) != 0)
    )

    def _region_stage(addr: int) -> int:
        region = 0
        for start_addr, stage_num in stage_starts:
            if addr >= start_addr:
                region = stage_num
            else:
                break
        return region

    sections = [
        s
        for s in sections
        if _section_stage(s) == stage
        or (_section_stage(s) == 0 and _region_stage(s.start_addr) == stage)
    ]

    # Every sprite any LOD table points at. A standalone bitmap section is
    # usually several sprites run together, so these are the addresses to cut it
    # at; collected across all the stage's LOD tables since a table may point
    # into a section other than its own.
    all_spans: List[Tuple[int, Optional[int]]] = []
    sprite_shapes: Dict[int, Optional[Tuple[int, int, bool]]] = {}
    for sec in sections:
        if sec.stype == "lod_table":
            all_spans.extend(lod_sprite_spans(sec, bank_offset, sprite_shapes))

    # First pass: name the blocks of every bitmap section so LOD tables can
    # reference them, splitting each section into its constituent sprites.
    bitmap_names: Dict[int, str] = {}
    bitmap_blocks: Dict[int, List[Tuple[int, int]]] = {}
    for sec in sections:
        if sec.stype != "bitmap":
            continue
        data = sec.bytes_flat
        blocks = split_into_sprites(
            sec.start_addr, sec.start_addr + len(data), all_spans
        ) or [(sec.start_addr, len(data))]
        bitmap_blocks[sec.start_addr] = blocks
        for addr, _ in blocks:
            bitmap_names[addr] = array_name(stage, "bitmap", addr)

    # Also pre-register the sprite blocks that lod_table sections will emit from
    # their trailing bitmap data, so that earlier LOD tables can resolve
    # pointers into blocks defined later in the file. Collect forward
    # declarations for them too.
    lod_remainder_fwd: List[str] = []
    for sec in sections:
        if sec.stype != "lod_table":
            continue
        for addr, size in lod_bitmap_blocks(sec, all_spans)[1]:
            name = f"stage{stage}_bitmap_{addr:04X}"
            bitmap_names[addr] = name
            length_expr, _ = sprite_layout(addr, size, sprite_shapes)
            lod_remainder_fwd.append(f"static const u8 {name}[{length_expr}];")

    # ── Header ───────────────────────────────────────────────────────────────
    print(f"/**")
    print(f" * Stage{stage}Data.c  (generated by convert_stage.py)")
    print(f" *")
    print(f" * Review all TODO comments before use.")
    print(f" */")
    print()
    print("#include <assert.h>")
    print("#include <stddef.h>")
    print()
    print('#include "C99/Types.h"')
    print('#include "ZXSpectrum/Pixels.h"')
    print('#include "ZXSpectrum/Spectrum.h"')
    print()
    print('#include "ChaseHQ/Engine/Types.h"')
    print('#include "CommonData.h"')
    print()
    print(f'#include "Stage{stage}Data.h"')
    print()
    print(
        "/* ----------------------------------------------------------------------- */"
    )
    print()

    # Object macros header
    print(f"/* Stage {stage} object type macros */")
    for i, oname in enumerate(obj_names):
        if oname:
            print(f"#define MAP_OBJ_S{stage}_{oname}_VAL  ({i})")
    print()
    for i, oname in enumerate(obj_names):
        if oname:
            print(
                f"#define MAP_OBJ_S{stage}_{oname}(D)   "
                f"(((D) << 4) | MAP_OBJ_S{stage}_{oname}_VAL)"
            )
    print()
    print(
        "/* ----------------------------------------------------------------------- */"
    )
    print()

    # ── First pass: build abs_addr → array_name map ──────────────────────────
    map_stypes = {"curvature", "height", "lanes", "hazards", "leftobjs", "rightobjs"}
    named_stypes = map_stypes | {
        "arrest_msgs",
        "perp_desc",
        "hazard_lods",
        "helicopter",
        "obj_defs",
        "obj_defs_right",
        "obj_defs_left",
        "hittable_objects",
        "lodaddrs",
        "lod_table",
    }
    abs_to_name: Dict[int, str] = {}
    for sec in sections:
        if sec.stype in named_stypes:
            abs_to_name[sec.start_addr] = array_name(stage, sec.stype, sec.start_addr)

    abs_to_depthset_name: Dict[int, str] = {}
    register_stretchy_names(
        stage, sections, bank_offset, abs_to_name, abs_to_depthset_name
    )

    # ── Generate code for each section ───────────────────────────────────────
    all_lines: List[str] = []
    goto_map: Dict[int, str] = {}  # raw_defw_value → array_name (for lookup_map_goto)
    fwd_decls: List[str] = []
    bitmap_sections = []

    for sec in sections:
        if sec.stype in map_stypes:
            lines, gm = emit_map_section(
                stage, sec.stype, sec, obj_names, bank_offset, abs_to_name
            )
            all_lines.extend(lines)
            all_lines.append("")
            goto_map.update(gm)
            nm = array_name(stage, sec.stype, sec.start_addr)
            # The macros re-encode the section's bytes one-for-one, so the C
            # array length is the section's byte span. Size the declaration:
            # C90 forbids a tentative definition with internal linkage and an
            # incomplete type.
            fwd_decls.append(f"static const u8 {nm}[{len(sec.bytes_flat)}];")

        elif sec.stype == "backdrop":
            fwd_decls.append("// backdrop declared inline in stage struct")

        elif sec.stype in ("perp_mugshot", "pilot_mugshot"):
            data = sec.bytes_flat
            nm = array_name(stage, sec.stype, sec.start_addr)
            all_lines.extend(emit_face_mugshot_array(nm, data, sec.start_addr))
            all_lines.append("")
            fwd_decls.append(f"static const u8 {nm}[{len(data)}];")

        elif sec.stype == "bitmap":
            data = sec.bytes_flat
            # Try to parse width×height from comment (e.g. "Bitmap data 5 bytes x 8")
            m = re.search(r"(\d+)\s+bytes?\s+x\s+(\d+)", sec.header_comment, re.I)
            per = (
                int(m.group(1)) * 2
                if m and "masked" in sec.header_comment.lower()
                else int(m.group(1)) if m else 8
            )
            # One array per sprite the LOD tables point at (see the first pass),
            # each shaped by the LOD entry that describes it. The section
            # comment's own width only stands in where no entry does.
            for addr, size in bitmap_blocks[sec.start_addr]:
                nm = bitmap_names[addr]
                start = addr - sec.start_addr
                length_expr, per_row = sprite_layout(addr, size, sprite_shapes, per)
                all_lines.extend(
                    emit_raw_array(
                        nm,
                        data[start : start + size],
                        per_row,
                        addr,
                        use_pixels=True,
                        length_expr=length_expr,
                    )
                )
                all_lines.append("")
                fwd_decls.append(f"static const u8 {nm}[{length_expr}];")
            bitmap_sections.append((nm, sec.start_addr))

        elif sec.stype == "lod_table":
            lines, n_lods = emit_lod_table(
                stage, sec, bank_offset, bitmap_names, all_spans, sprite_shapes
            )
            all_lines.extend(lines)
            all_lines.append("")
            nm = array_name(stage, "lod_table", sec.start_addr)
            if n_lods > 0:
                fwd_decls.append(f"static const bitmap_t {nm}[{n_lods}];")
            else:
                fwd_decls.append(f"static const u8 {nm}[{len(sec.bytes_flat)}];")

        elif sec.stype == "arrest_msgs":
            lines, total = emit_arrest_messages(stage, sec, defm_bytes)
            all_lines.extend(lines)
            all_lines.append("")
            nm = array_name(stage, "arrest_msgs", sec.start_addr)
            fwd_decls.append(f"static const u8 {nm}[{total}];")

        elif sec.stype == "perp_desc":
            all_lines.extend(emit_perp_description(stage, sec))
            all_lines.append("")
            fwd_decls.append(f"static const u8 stage{stage}_perp_description[7];")

            # The first four DEFW words in the header are pointers to the
            # chatter strings (DEFM lines).  Resolve each via defm_strings.
            ws = sec.words_with_annots
            strings = [
                defm_strings[w[1]] for w in ws[:4] if w[1] >= 0 and w[1] in defm_strings
            ]
            if strings:
                chat_nm = f"stage{stage}_chatter_strings"
                all_lines.append(f"static const char *{chat_nm}[{len(strings)}] = {{")
                for s in strings:
                    all_lines.append(f"  {s},")
                all_lines.append("};")
                all_lines.append("")
                fwd_decls.append(f"static const char *{chat_nm}[{len(strings)}];")

        elif sec.stype == "hittable_objects":
            lines, n = emit_hittable_array(stage, sec, abs_to_name)
            all_lines.extend(lines)
            all_lines.append("")
            nm = array_name(stage, "hittable_objects", sec.start_addr)
            fwd_decls.append(f"static const hittable_t {nm}[{n}];")

        elif sec.stype in ("obj_defs_right", "obj_defs_left"):
            lines, n = emit_obj_array(stage, sec, abs_to_name, abs_to_depthset_name)
            all_lines.extend(lines)
            all_lines.append("")
            nm = array_name(stage, sec.stype, sec.start_addr)
            fwd_decls.append(f"static const obj_t {nm}[{n}];")

        elif sec.stype == "hazard_lods":
            data = sec.bytes_flat
            nm = array_name(stage, "hazard_lods", sec.start_addr)
            all_lines.extend(emit_raw_array(nm, data, 3, sec.start_addr))
            all_lines.append("")
            fwd_decls.append(f"static const u8 {nm}[{len(data)}];")

        elif sec.stype == "helicopter":
            data = sec.bytes_flat
            nm = array_name(stage, "helicopter", sec.start_addr)
            all_lines.extend(emit_raw_array(nm, data, 8, sec.start_addr))
            all_lines.append("")
            fwd_decls.append(f"static const u8 {nm}[{len(data)}];")

        elif sec.stype == "obj_defs":
            data = sec.bytes_flat
            nm = array_name(stage, "obj_defs", sec.start_addr)
            all_lines.extend(emit_raw_array(nm, data, 7, sec.start_addr))
            all_lines.append("")
            fwd_decls.append(f"static const u8 {nm}[{len(data)}];")

        elif sec.stype == "lodaddrs":
            data = sec.bytes_flat
            nm = array_name(stage, "lodaddrs", sec.start_addr)
            all_lines.extend(emit_raw_array(nm, data, 2, sec.start_addr))
            all_lines.append("")
            fwd_decls.append(f"static const u8 {nm}[{len(data)}];")

        elif sec.stype == "stretchy":
            s_lines, s_fwd = emit_stretchy_typed(
                stage, sec, bank_offset, abs_to_name, abs_to_depthset_name
            )
            all_lines.extend(s_lines)
            all_lines.append("")
            fwd_decls.extend(s_fwd)

        elif sec.stype in ("perstage", "difficulty", "setupdata", "attractdata"):
            pass  # data inlined into stage_t struct by emit_stage_struct

        elif sec.stype == "unknown":
            all_lines.append(f"/* TODO: ${sec.start_addr:04X} [{sec.stype}]")
            all_lines.append(f"   {sec.header_comment}")
            # Emit raw hex as a comment
            data = sec.bytes_flat
            if data:
                hex_str = " ".join(f"0x{b:02X}" for b in data[:64])
                if len(data) > 64:
                    hex_str += f" ... ({len(data)} bytes total)"
                all_lines.append(f"   Raw: {hex_str}")
            # For word sections, show annotations
            wlist = sec.words_with_annots
            if wlist:
                for raw_w, abs_a, waddr in wlist[:16]:
                    ann = f"→ ${abs_a:04X}" if abs_a >= 0 else "(out-of-bounds)"
                    all_lines.append(f"   ${waddr:04X}: DEFW ${raw_w:04X}  {ann}")
            all_lines.append("*/")
            all_lines.append("")

    # ── Emit forward declarations ─────────────────────────────────────────────
    print("/* Forward declarations */")
    for d in fwd_decls:
        print(d)
    for d in lod_remainder_fwd:
        print(d)
    print()
    print(
        "/* ----------------------------------------------------------------------- */"
    )
    print()

    # ── Stage struct ──────────────────────────────────────────────────────────
    for line in emit_stage_struct(stage, sections, abs_to_name, bank_offset):
        print(line)
    print()
    print(
        "/* ----------------------------------------------------------------------- */"
    )
    print()

    # ── Data sections ─────────────────────────────────────────────────────────
    for line in all_lines:
        print(line)

    # ── lookup_map_goto ───────────────────────────────────────────────────────
    tname = f"stage{stage}_map_goto_table"
    print(f"static const struct {{ u16 z80; const void *ptr; }} {tname}[] = {{")
    for raw_addr, name in sorted(goto_map.items()):
        if name:
            print(f"  {{ 0x{raw_addr:04X}, &{name}[0] }},")
        else:
            raw = raw_addr - bank_offset if raw_addr > bank_offset else raw_addr
            print(
                f"  /* TODO: 0x{raw:04X} -> find array name for abs ${raw_addr:04X} */"
            )
    print("};")
    print()
    # Signature must match the declaration in Stage{N}Data.h, which takes the
    # address alone; the body never needed a state pointer.
    print(f"const void *stage{stage}_lookup_map_goto(u16 z80)")
    print("{")
    print("  int lo, hi, mid;")
    print()
    print(f"  lo  = 0;")
    print(f"  hi  = (int)NELEMS({tname}) - 1;")
    print(f"  while (lo <= hi) {{")
    print(f"    mid = lo + (hi - lo) / 2;")
    print(f"    if ({tname}[mid].z80 == z80) return {tname}[mid].ptr;")
    print(f"    if ({tname}[mid].z80 < z80)  lo = mid + 1;")
    print(f"    else                          hi = mid - 1;")
    print(f"  }}")
    print(f'  assert("Unknown Z80 address (stage {stage})" == NULL);')
    print("  return NULL;")
    print("}")


def main():
    parser = argparse.ArgumentParser(description="Convert skool file to C stage data.")
    parser.add_argument("skool", help="Input .skool file")
    parser.add_argument("stage", type=int, help="Stage number (2-5)")
    parser.add_argument(
        "--obj-names",
        default="NONE,TUNNEL_LIGHT,OBJ2,SHORT_POLE," "OBJ4,OBJ5,OBJ6,OBJ7,OBJ8",
        help="Comma-separated object type names for indices 0-8",
    )
    args = parser.parse_args()

    obj_names = [n.strip() for n in args.obj_names.split(",")]
    # Pad or trim to exactly 16 (max nibble value 0xF = 15)
    while len(obj_names) < 16:
        obj_names.append(f"OBJ{len(obj_names)}")

    convert(args.skool, args.stage, obj_names)


if __name__ == "__main__":
    main()
