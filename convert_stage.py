#!/usr/bin/env python3
"""
convert_stage.py

Converts a Chase H.Q. skool file to a C stage data skeleton.

Usage:
  python3 convert_stage.py <skool_file> <stage_num> [options] > ChaseHQ-StageNData.c

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
    0: 'STRAIGHT', 1: 'RIGHT', 2: 'RIGHT_HARD', 3: 'RIGHT_VERY_HARD',
    9: 'LEFT', 10: 'LEFT_HARD', 11: 'LEFT_VERY_HARD',
}

HEIGHT_TYPES = {
    1: 'UP7', 3: 'UP5', 5: 'UP3', 7: 'UP1',
    8: 'LEVEL', 9: 'DOWN1', 11: 'DOWN3', 13: 'DOWN5', 15: 'DOWN7',
}

LANE_VALS = {
    0x00: '4',      0x81: '3L',     0x82: '3R',
    0x01: '2L',     0x02: '2M',     0x03: '2R',
    0xBD: '4TO3L',  0x8E: '4TO3R',  0xAD: '3TO4L',  0x9E: '3TO4R',
    0x06: '3TO2L',  0x0F: '3TO2R',  0x2D: '2TO3L',  0x1F: '2TO3R',
    0x45: 'TUNNEL', 0x59: 'TUNNEL_EXIT',
    0xC1: 'DIRTTRACK', 0xED: 'FORKED',
}

# ESC command byte → macro name (None = needs special handling)
HAZARD_CMDS = {
    0:  None,                    # GOTO/LOOP
    1:  'MAP_CMD_FORK_END',
    2:  None,                    # SPLIT
    3:  'MAP_CMD_STOP_BARRIERS',
    4:  'MAP_ESC, (4)',          # HAZARD1_L (Stage 2+) / START_BARRIERS_L (S1)
    5:  'MAP_ESC, (5)',          # HAZARD1_R
    6:  'MAP_CMD_UNKNOWN_HAZARD_6',
    7:  'MAP_CMD_START_BARRIERS_L',
    8:  'MAP_CMD_START_BARRIERS_R',
    9:  'MAP_CMD_START_TWO_BARRIERS',
    10: 'MAP_CMD_ARROW_OFF',
    11: 'MAP_CMD_ARROW_L',
    12: 'MAP_CMD_ARROW_R',
    13: 'MAP_CMD_START_CARS',
    14: 'MAP_CMD_STOP_CARS',
    15: 'MAP_ESC, (15)',         # stop helicopter
    17: 'MAP_ESC, (17)',         # start heli left
}

BITMAP_FLAGS = {0: 'BITMAPFLAG_DEFAULT', 1: 'BITMAPFLAG_MASKED',
                2: 'BITMAPFLAG_FLIPPED', 3: 'BITMAPFLAG_MASKED|BITMAPFLAG_FLIPPED'}

CHATTERCHR_NAMES = {
    0: 'CHATTERCHR_PILOT', 1: 'CHATTERCHR_NANCY',
    2: 'CHATTERCHR_RAYMOND', 3: 'CHATTERCHR_TONY',
}

DRAWCHARSTYLE_NAMES = {
    1: 'DRAWCHARSTYLE_GENERIC',
    2: 'DRAWCHARSTYLE_SINGLE',
    3: 'DRAWCHARSTYLE_DOUBLE',
    4: 'DRAWCHARSTYLE_SINGLE_INVERTED',
    5: 'DRAWCHARSTYLE_DOUBLE_INVERTED',
}

TRANSITIONCONTROL_NAMES = {
    0: 'TRANSITIONCONTROL_STOP',
    1: 'TRANSITIONCONTROL_DRAW_MUGSHOTS',
    2: 'TRANSITIONCONTROL_OVERLAY_MESSAGES',
    3: 'TRANSITIONCONTROL_FILL_ATTRIBUTES',
    4: 'TRANSITIONCONTROL_FADE',
}

ATTR_COLOR_NAMES = {
    0: 'ATTR_BLACK', 1: 'ATTR_BLUE',    2: 'ATTR_RED',    3: 'ATTR_MAGENTA',
    4: 'ATTR_GREEN', 5: 'ATTR_CYAN',    6: 'ATTR_YELLOW', 7: 'ATTR_WHITE',
}

def _mkattr(f: int, b: int, p: int, i: int) -> int:
    return (f << 7) | (b << 6) | (p << 3) | i

# Maps the byte values of every named enum entry in ZXSpectrum/Spectrum.h
ATTRIBUTE_MAP: Dict[int, str] = {
    _mkattr(0, 0, 0, 0): 'attribute_BLACK_OVER_BLACK',
    _mkattr(0, 0, 0, 1): 'attribute_BLUE_OVER_BLACK',
    _mkattr(0, 0, 0, 2): 'attribute_RED_OVER_BLACK',
    _mkattr(0, 0, 0, 3): 'attribute_MAGENTA_OVER_BLACK',
    _mkattr(0, 0, 0, 4): 'attribute_GREEN_OVER_BLACK',
    _mkattr(0, 0, 0, 5): 'attribute_CYAN_OVER_BLACK',
    _mkattr(0, 0, 0, 6): 'attribute_YELLOW_OVER_BLACK',
    _mkattr(0, 0, 0, 7): 'attribute_WHITE_OVER_BLACK',
    _mkattr(0, 1, 0, 1): 'attribute_BRIGHT_BLUE_OVER_BLACK',
    _mkattr(0, 1, 0, 2): 'attribute_BRIGHT_RED_OVER_BLACK',
    _mkattr(0, 1, 0, 3): 'attribute_BRIGHT_MAGENTA_OVER_BLACK',
    _mkattr(0, 1, 0, 4): 'attribute_BRIGHT_GREEN_OVER_BLACK',
    _mkattr(0, 1, 0, 5): 'attribute_BRIGHT_CYAN_OVER_BLACK',
    _mkattr(0, 1, 0, 6): 'attribute_BRIGHT_YELLOW_OVER_BLACK',
    _mkattr(0, 1, 0, 7): 'attribute_BRIGHT_WHITE_OVER_BLACK',
    _mkattr(0, 1, 1, 0): 'attribute_BRIGHT_BLACK_OVER_BLUE',
    _mkattr(0, 1, 2, 0): 'attribute_BRIGHT_BLACK_OVER_RED',
    _mkattr(0, 1, 3, 0): 'attribute_BRIGHT_BLACK_OVER_MAGENTA',
    _mkattr(0, 1, 4, 0): 'attribute_BRIGHT_BLACK_OVER_GREEN',
    _mkattr(0, 1, 5, 0): 'attribute_BRIGHT_BLACK_OVER_CYAN',
    _mkattr(0, 1, 6, 0): 'attribute_BRIGHT_BLACK_OVER_YELLOW',
    _mkattr(0, 1, 7, 0): 'attribute_BRIGHT_BLACK_OVER_WHITE',
    _mkattr(0, 0, 4, 0): 'attribute_BLACK_OVER_GREEN',
    _mkattr(0, 0, 5, 0): 'attribute_BLACK_OVER_CYAN',
    _mkattr(0, 0, 6, 0): 'attribute_BLACK_OVER_YELLOW',
    _mkattr(0, 0, 7, 0): 'attribute_BLACK_OVER_WHITE',
}


def byte_to_attr(b: int) -> str:
    """Return the attribute_* name for b, or MKATTR(...) if not in the enum."""
    if b in ATTRIBUTE_MAP:
        return ATTRIBUTE_MAP[b]
    f  = (b >> 7) & 1
    br = (b >> 6) & 1
    p  = (b >> 3) & 7
    i  = b & 7
    return f'MKATTR({f}, {br}, {ATTR_COLOR_NAMES[p]}, {ATTR_COLOR_NAMES[i]})'


# Face/mugshot layout constants (must match ChaseHQ-Internal.h)
FACE_ROW_BYTES   = 4   # FACEROWBYTES  = FACEWIDTH / 8 = 32 / 8
FACE_BITMAP_ROWS = 40  # FACEHEIGHT
FACE_ATTR_ROWS   = 5   # FACEATTRHEIGHT
FACE_BITMAP_BYTES = FACE_ROW_BYTES * FACE_BITMAP_ROWS  # 160
FACE_ATTR_BYTES   = FACE_ROW_BYTES * FACE_ATTR_ROWS    # 20


def emit_face_mugshot_array(name: str, data: List[int],
                            z80_addr: int) -> List[str]:
    """Emit a face/mugshot array: pixel names for the bitmap rows, attribute
    names for the final FACE_ATTR_ROWS rows."""
    lines = [f'// ${z80_addr:04X}', '// clang-format off',
             f'static const u8 {name}[{len(data)}] = {{']

    for i in range(0, FACE_BITMAP_BYTES, FACE_ROW_BYTES):
        chunk = data[i:i + FACE_ROW_BYTES]
        lines.append('  ' + ', '.join(byte_to_pixel(b) for b in chunk) + ',')

    lines.append('')

    for i in range(FACE_BITMAP_BYTES, FACE_BITMAP_BYTES + FACE_ATTR_BYTES,
                   FACE_ROW_BYTES):
        chunk = data[i:i + FACE_ROW_BYTES]
        lines.append('  ' + ', '.join(byte_to_attr(b) for b in chunk) + ',')

    lines.append('};')
    lines.append('// clang-format on')
    return lines


CHATTERCMD_NAMES = {
    0xFC: 'CHATTERCMD_RANDOM',
    0xFE: 'CHATTERCMD_PAUSE',
    0xFF: 'CHATTERCMD_STOP',
}

# Fields of stage_t that come from perstage words 3..13.
PERSTAGE_PTR_FIELDS = [
    'addrof_hittable_objects',
    'addrof_right_hand_handlers',
    'addrof_right_hand_objects',
    'addrof_right_hand_short_pole_object',
    'addrof_left_hand_handlers',
    'addrof_left_hand_objects',
    'addrof_left_hand_short_pole_object',
    'addrof_perp_description',
    'addrof_arrest_messages',
    'addrof_helicopter_stuff_1',
    'addrof_helicopter_stuff_2',
]

# ── Skool parser ─────────────────────────────────────────────────────────────

def parse_hex(s: str) -> int:
    """Parse $XX or 0xXX or decimal."""
    s = s.strip()
    if s.startswith('$'):
        return int(s[1:], 16)
    if s.startswith('0x') or s.startswith('0X'):
        return int(s[2:], 16)
    return int(s)


class SkoolRecord:
    def __init__(self, addr: int, rtype: str, values: List[int], comment: str,
                 annotations: List[int]):
        self.addr = addr          # Z80 absolute address
        self.rtype = rtype        # 'B'=bytes, 'W'=words, 'C'=section label
        self.values = values      # list of ints (bytes or words)
        self.comment = comment    # inline comment text (stripped)
        self.annotations = annotations  # resolved addresses from [$XXXX] in comment


def parse_skool(path: str) -> Tuple[List[SkoolRecord], List[str]]:
    """
    Return (records, section_comments).
    section_comments[i] is the comment line immediately before records[i].
    """
    records: List[SkoolRecord] = []
    section_comments: List[str] = []
    pending_section = ''

    def _annots(cmt: str) -> List[int]:
        return [int(m, 16) for m in re.findall(r'\[\$([0-9A-Fa-f]+)\]', cmt)]

    with open(path, encoding='utf-8', errors='replace') as f:
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
            if line.startswith('; '):
                txt = line[2:].strip()
                trivial = txt in ('', '.', '}', '{', ';', 'LOD')
                is_stage_hdr = bool(re.match(r'\[Stage \d+\]', txt))
                if is_stage_hdr:
                    pending_section = txt
                elif not trivial:
                    if not pending_section:
                        pending_section = txt
                    else:
                        new_type = classify_section(txt)
                        cur_type = classify_section(pending_section)
                        if (new_type != 'unknown' and
                                (new_type != cur_type or txt != pending_section)):
                            pending_section = txt
                continue

            # Labelled byte data: b$ADDR DEFB ...
            m = re.match(r'^([bcw]?)\$([0-9A-Fa-f]+)\s+(DEFB|DEFW)\s+(.*)', line)
            if not m:
                # Continuation line: ' $ADDR DEFB ...' or ' $ADDR DEFW ...'
                m = re.match(r'^\s+\$([0-9A-Fa-f]+)\s+(DEFB|DEFW)\s+(.*)', line)
                if m:
                    addr = int(m.group(1), 16)
                    dtype = m.group(2)
                    rest = m.group(3)
                else:
                    continue
                prefix = ''
            else:
                prefix = m.group(1)
                addr = int(m.group(2), 16)
                dtype = m.group(3)
                rest = m.group(4)

            # Split rest into value list and comment
            if ';' in rest:
                val_part, cmt_part = rest.split(';', 1)
                cmt_part = cmt_part.strip()
            else:
                val_part = rest
                cmt_part = ''

            # Parse values
            values = []
            for tok in re.split(r'[,\s]+', val_part.strip()):
                if tok and tok.strip():
                    try:
                        values.append(parse_hex(tok))
                    except ValueError:
                        pass

            rtype = 'W' if dtype == 'DEFW' else 'B'
            rec = SkoolRecord(addr, rtype, values, cmt_part, _annots(cmt_part))
            records.append(rec)
            section_comments.append(pending_section)
            if prefix:  # new labelled section resets pending comment
                pending_section = ''

    return records, section_comments


def build_addr_map(records: List[SkoolRecord]) -> Dict[int, int]:
    """Build a flat address → byte value map."""
    m: Dict[int, int] = {}
    for rec in records:
        addr = rec.addr
        if rec.rtype == 'W':
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
        if rec.rtype == 'W' and rec.annotations and rec.values:
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
        elif ch == ';' and not in_q:
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
                if c == '\\':
                    c_chars.append('\\\\')
                elif c == '"':
                    c_chars.append('\\"')
                else:
                    c_chars.append(c)
                i += 1
            i += 1  # skip closing quote
        elif rest[i] in (' ', ','):
            i += 1
        elif rest[i] == '$':
            b = int(rest[i + 1:i + 3], 16)
            c_chars.append(f'\\x{b:02X}')
            i += 3
        elif rest[i].isdigit():
            j = i
            while j < len(rest) and rest[j].isdigit():
                j += 1
            c_chars.append(f'\\x{int(rest[i:j]):02X}')
            i = j
        else:
            i += 1
    return '"' + ''.join(c_chars) + '"'


def _parse_defm_raw(rest: str) -> List[int]:
    """Parse a DEFM operand to raw byte values."""
    in_q = False
    for i, ch in enumerate(rest):
        if ch == '"':
            in_q = not in_q
        elif ch == ';' and not in_q:
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
        elif rest[i] in (' ', ','):
            i += 1
        elif rest[i] == '$':
            result.append(int(rest[i + 1:i + 3], 16))
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


def parse_defm_map(path: str) -> Tuple[Dict[int, str], Dict[int, List[int]]]:
    """Scan a skool file for DEFM lines in a single pass.

    Returns (strings, bytemap) where strings maps addr → C string literal
    and bytemap maps addr → raw byte list.  Skool label addresses are
    already absolute so no bank offset is applied.
    """
    strings:  Dict[int, str]       = {}
    bytemap:  Dict[int, List[int]] = {}
    with open(path, encoding='utf-8', errors='replace') as f:
        for line in f:
            line = line.rstrip()
            m = re.match(r'^[bcw]?\$([0-9A-Fa-f]+)\s+DEFM\s+(.*)', line)
            if not m:
                m = re.match(r'^\s+\$([0-9A-Fa-f]+)\s+DEFM\s+(.*)', line)
            if m:
                addr = int(m.group(1), 16)
                rest = m.group(2)
                strings[addr] = parse_defm_text(rest)
                bytemap[addr] = _parse_defm_raw(rest)
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
            out.append(f'MAP_CURVE_{name}({count}),')
        else:
            out.append(f'/* unknown curvature 0x{b:02X} */')
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
            out.append(f'MAP_HEIGHT_{name}({count}),')
        else:
            out.append(f'/* unknown height 0x{b:02X} */')
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
            out.append(f'MAP_LANES_{name}({count}),')
        else:
            out.append(f'/* unknown lanes val=0x{val:02X} */ {count}, 0x{val:02X},')
        i += 2
    return out


def decode_hazards(stream: List[int]) -> List[str]:
    """Decode a hazard byte stream into MAP_HAZARD_WAIT / MAP_CMD_xxx."""
    out = []
    i = 0
    while i < len(stream):
        b = stream[i]
        if b != 0:
            out.append(f'MAP_HAZARD_WAIT({b}),')
            i += 1
        else:
            # ESC sequence
            if i + 1 >= len(stream):
                break
            cmd = stream[i + 1]
            macro = HAZARD_CMDS.get(cmd)
            if cmd == 0:   # GOTO/LOOP: needs DEFW
                out.append(None)
                break
            elif cmd == 1:
                out.append('MAP_CMD_FORK_END,')
                i += 2
            elif cmd == 2:  # SPLIT: needs 2×DEFW
                out.append(None)
                break
            elif macro:
                out.append(f'{macro},')
                i += 2
            else:
                out.append(f'MAP_ESC, ({cmd}),')
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
            out.append(f'MAP_OBJ_S{stage}_{name}({count}),')
        else:
            out.append(f'/* obj 0x{b:02X} */')
        i += 1
    return out


# ── Section identification ────────────────────────────────────────────────────

MAP_SECTION_TYPES = {
    'curvature': decode_curvature,
    'height':    decode_height,
    'lanes':     decode_lanes,
    'hazards':   decode_hazards,
    'left object': None,   # objects – set below
    'right object': None,
}

def classify_section(comment: str) -> Optional[str]:
    """
    Return a section type key from a section-header comment, or None.
    The comment is like 'Stage 2] Map curvature data'.
    """
    c = comment.lower()
    if 'horizon graphic' in c or 'backdrop' in c:
        return 'backdrop'
    if 'per-stage data' in c:
        return 'perstage'
    if 'per-stage difficulty' in c:
        return 'difficulty'
    if 'per-stage setup data' in c:
        return 'setupdata'
    if 'per-stage attract' in c:
        return 'attractdata'
    if 'table of addresses of lods' in c:
        return 'lodaddrs'
    if 'map curvature' in c:
        return 'curvature'
    if 'map height' in c:
        return 'height'
    if 'map lanes' in c:
        return 'lanes'
    if 'map hazards' in c:
        return 'hazards'
    if 'map left object' in c:
        return 'leftobjs'
    if 'map right object' in c:
        return 'rightobjs'
    if "perp's mugshot" in c or "perp mugshot" in c:
        return 'perp_mugshot'
    if "pilot's mugshot" in c or "pilot mugshot" in c:
        return 'pilot_mugshot'
    if 'lod table' in c:
        return 'lod_table'
    if 'hittable hazard' in c:
        return 'hazard_lods'
    if 'helicopter data' in c:
        return 'helicopter'
    if 'object graphic definitions' in c or 'graphic definition for object' in c:
        return 'obj_defs'
    if 'hittable hazards' in c:
        return 'hittable'
    if 'arrest messages' in c:
        return 'arrest_msgs'
    if "nancy's perp description" in c or "perp description" in c:
        return 'perp_desc'
    if 'stretchy graphic' in c:
        return 'stretchy'
    if 'lod table' in c:
        return 'lod_table'
    if 'bitmap data' in c or '{bitmap' in c or 'pointed to by helicopter' in c:
        return 'bitmap'
    return 'unknown'


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
            if rec.rtype == 'W':
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
            if rec.rtype == 'W':
                for idx, w in enumerate(rec.values):
                    ann = rec.annotations[idx] if idx < len(rec.annotations) else -1
                    result.append((w, ann, rec.addr + idx * 2))
        return result


def split_into_sections(records: List[SkoolRecord],
                         section_comments: List[str]) -> List[Section]:
    """Group records by section, detecting section type from comments."""
    sections: List[Section] = []
    cur: Optional[Section] = None
    prev_type = None
    prev_cmt = None

    for rec, cmt in zip(records, section_comments):
        stype = classify_section(cmt)
        # Start a new section when the comment is non-empty AND either:
        #   - the type has changed, OR
        #   - the comment itself has changed (e.g. two consecutive lod_tables)
        if cmt and (stype != prev_type or cmt != prev_cmt):
            if cur is not None:
                sections.append(cur)
            cur = Section(stype or 'unknown', rec.addr, cmt)
            prev_type = stype
            prev_cmt = cmt
        if cur is None:
            cur = Section('unknown', rec.addr, cmt)
        cur.records.append(rec)

    if cur is not None:
        sections.append(cur)
    return sections


# ── Code generators ───────────────────────────────────────────────────────────

def array_name(stage: int, stype: str, addr: int) -> str:
    """Generate a C array name from stage, section type and address."""
    prefixes = {
        'backdrop':    f'stage{stage}_backdrop',
        'curvature':   f'stage{stage}_map_curv_{addr:04X}',
        'height':      f'stage{stage}_map_height_{addr:04X}',
        'lanes':       f'stage{stage}_map_lanes_{addr:04X}',
        'hazards':     f'stage{stage}_map_hazards_{addr:04X}',
        'leftobjs':    f'stage{stage}_map_lobjs_{addr:04X}',
        'rightobjs':   f'stage{stage}_map_robjs_{addr:04X}',
        'perp_mugshot': f'stage{stage}_perp_face',
        'pilot_mugshot': f'stage{stage}_pilot_mugshot',
        'bitmap':       f'stage{stage}_bitmap_{addr:04X}',
        'lod_table':    f'stage{stage}_lods_{addr:04X}',
        'hazard_lods':  f'stage{stage}_hazard_lods_{addr:04X}',
        'arrest_msgs':  f'stage{stage}_arrest_messages_{addr:04X}',
        'perp_desc':    f'stage{stage}_perp_description',
        'obj_defs':     f'stage{stage}_obj_defs_{addr:04X}',
        'helicopter':   f'stage{stage}_helicopter_{addr:04X}',
        'lodaddrs':     f'stage{stage}_lod_addrs_{addr:04X}',
        'stretchy':     f'stage{stage}_stretchy_{addr:04X}',
    }
    return prefixes.get(stype, f'stage{stage}_data_{addr:04X}')


def byte_to_pixel(b: int) -> str:
    """Return the Pixels.h macro name for b (e.g. 0xA5 → 'X_X__X_X')."""
    return ''.join('X' if (b >> (7 - i)) & 1 else '_' for i in range(8))


def emit_raw_array(name: str, data: List[int], per_row: int = 8,
                   z80_addr: int = 0, use_pixels: bool = False) -> List[str]:
    """Emit a 'static const u8 name[] = { ... };' array.

    When use_pixels is True the bytes are written using the Pixels.h macro
    names (e.g. X_X__X_X) instead of hex, and the block is wrapped in
    clang-format off/on so the visual rows are not reformatted.
    """
    lines = []
    lines.append(f'// ${z80_addr:04X}')
    if use_pixels:
        lines.append('// clang-format off')
    lines.append(f'static const u8 {name}[{len(data)}] = {{')
    for i in range(0, len(data), per_row):
        chunk = data[i:i + per_row]
        if use_pixels:
            row = ', '.join(byte_to_pixel(b) for b in chunk)
        else:
            row = ', '.join(f'0x{b:02X}' for b in chunk)
        lines.append(f'  {row},')
    lines.append('};')
    if use_pixels:
        lines.append('// clang-format on')
    return lines


def emit_map_section(stage: int, stype: str, sec: Section,
                     obj_names: List[str], bank_offset: int,
                     abs_to_name: Dict[int, str]) -> Tuple[List[str], Dict[int, str]]:
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
        if rec.rtype == 'W':
            for idx, w in enumerate(rec.values):
                ann = rec.annotations[idx] if idx < len(rec.annotations) else -1
                defw_seq.append((w, ann))

    # Choose decoder
    if stype == 'curvature':
        decoded = decode_curvature(raw)
    elif stype == 'height':
        decoded = decode_height(raw)
    elif stype == 'lanes':
        decoded = decode_lanes(raw)
    elif stype == 'hazards':
        decoded = decode_hazards(raw)
    elif stype in ('leftobjs', 'rightobjs'):
        decoded = decode_objects(raw, stage, obj_names)
    else:
        decoded = [f'/* 0x{b:02X} */' for b in raw]

    content_lines = [l for l in decoded if l is not None]

    # Determine terminal command: find the LAST ESC+{0,1,2} in the stream.
    # For lanes (paired bytes), ESC is only valid at even positions.
    # For hazards, there may be multiple ESC sequences; the terminal is always last.
    terminal_cmd = None
    if stype == 'lanes':
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
        tgt_name = abs_to_name.get(abs_tgt, f'/* ${abs_tgt:04X} */')
        goto_map[raw_tgt] = tgt_name
        content_lines.append(f'MAP_CMD_GOTO(0x{raw_tgt:04X})')
    elif terminal_cmd == 2 and len(defw_seq) >= 2:
        # SPLIT — two DEFW targets
        raw_l, abs_l = defw_seq[0]
        raw_r, abs_r = defw_seq[1]
        if abs_l < 0:
            abs_l = raw_l + bank_offset
        if abs_r < 0:
            abs_r = raw_r + bank_offset
        name_l = abs_to_name.get(abs_l, f'/* ${abs_l:04X} */')
        name_r = abs_to_name.get(abs_r, f'/* ${abs_r:04X} */')
        goto_map[raw_l] = name_l
        goto_map[raw_r] = name_r
        content_lines.append(f'MAP_CMD_SPLIT(0x{raw_l:04X}, 0x{raw_r:04X})')
    elif terminal_cmd == 1:
        content_lines.append('MAP_CMD_FORK_END')

    lines = [f'// ${sec.start_addr:04X}']
    lines.append(f'static const u8 {name}[] = {{')
    for cl in content_lines:
        lines.append(f'  {cl}')
    lines.append('};')
    return lines, goto_map


def resolve_section_ptr(abs_addr: int,
                        abs_to_name: Dict[int, str]) -> Optional[str]:
    """Resolve abs_addr to &array[offset] using registered section starts."""
    if abs_addr in abs_to_name:
        return f'&{abs_to_name[abs_addr]}[0]'
    candidates = [(base, nm) for base, nm in abs_to_name.items()
                  if base <= abs_addr]
    if candidates:
        base, nm = max(candidates, key=lambda x: x[0])
        return f'&{nm}[{abs_addr - base}]'
    return None


def resolve_bitmap_ref(abs_addr: int, bitmap_names: Dict[int, str],
                       stage: int) -> str:
    """Return a C expression (&array[offset]) for a bitmap at abs_addr."""
    if abs_addr in bitmap_names:
        return f'&{bitmap_names[abs_addr]}[0]'
    candidates = [(base, name) for base, name in bitmap_names.items()
                  if base <= abs_addr]
    if candidates:
        base, name = max(candidates, key=lambda x: x[0])
        return f'&{name}[{abs_addr - base}]'
    return f'&stage{stage}_bitmap_{abs_addr:04X}[0]'


def emit_perp_description(stage: int, sec: Section) -> List[str]:
    """Decode a perp_desc section into CHATTERCHR/CHATTERSTR/CHATTERCMD/CHATTERBLK."""
    data = sec.bytes_flat
    nm = f'stage{stage}_perp_description'

    # Expected layout (12 raw bytes):
    #   [0]      DEFB  char_id
    #   [1..8]   4 × DEFW  string pointers (replaced by CHATTERSTR_PERP_DESC_1..4)
    #   [9]      DEFB  command byte
    #   [10..11] DEFW  next-block address (always CHATTERBLK_HEROES_ACKNOWLEDGE)
    if len(data) < 12:
        return emit_raw_array(nm, data, 8, sec.start_addr)

    char_name = CHATTERCHR_NAMES.get(data[0], f'({data[0]})')
    cmd_name  = CHATTERCMD_NAMES.get(data[9], f'0x{data[9]:02X}')

    lines = [f'// ${sec.start_addr:04X}']
    lines.append(f'static const u8 {nm}[7] = {{')
    lines.append(f'  {char_name},')
    lines.append(f'  CHATTERSTR_PERP_DESC_1,')
    lines.append(f'  CHATTERSTR_PERP_DESC_2,')
    lines.append(f'  CHATTERSTR_PERP_DESC_3,')
    lines.append(f'  CHATTERSTR_PERP_DESC_4,')
    lines.append(f'  {cmd_name},')
    lines.append(f'  CHATTERBLK_HEROES_ACKNOWLEDGE')
    lines.append('};')
    return lines


def emit_lod_table(stage: int, sec: Section, bank_offset: int,
                   bitmap_names: Dict[int, str]) -> Tuple[List[str], int]:
    """
    Decode a LOD table (7-byte bitmap_t records).
    Counts actual LOD entries from 'Width (bytes)' inline comments so that
    bitmap data appended to the same section is handled separately.
    Returns (C lines, n_lod_entries).
    """
    # Count LOD entries: each entry's first byte has comment 'Width (bytes)'
    n_lods = sum(1 for rec in sec.records
                 if 'width (bytes)' in rec.comment.lower())

    data = sec.bytes_flat
    if n_lods == 0:
        # Fallback: divide by 7 if cleanly possible
        if len(data) % 7 == 0:
            n_lods = len(data) // 7
        else:
            return emit_raw_array(array_name(stage, 'lod_table', sec.start_addr),
                                  data, 7, sec.start_addr), 0

    # Cap at first entry with invalid flags (valid: 0=default, 1=masked,
    # 2=flipped, 3=both). Bitmap bytes mixed into the section can have
    # spurious Width(bytes) annotations, so we stop at garbage entries.
    valid_lods = 0
    for j in range(n_lods):
        off = j * 7
        if off + 1 >= len(data) or data[off + 1] not in (0, 1, 2, 3):
            break
        valid_lods += 1
    n_lods = valid_lods

    lod_end = n_lods * 7
    name = array_name(stage, 'lod_table', sec.start_addr)
    lines = [f'// ${sec.start_addr:04X}']
    lines.append(f'static const bitmap_t {name}[{n_lods}] = {{')
    for i in range(n_lods):
        off = i * 7
        if off + 6 >= len(data):
            break
        width = data[off]
        flags = data[off + 1]
        height = data[off + 2]
        data_raw = (data[off + 4] << 8) | data[off + 3]
        shft_raw = (data[off + 6] << 8) | data[off + 5]
        data_abs = data_raw + bank_offset
        shft_abs = shft_raw + bank_offset
        flag_str = BITMAP_FLAGS.get(flags, f'0x{flags:02X}')
        d_ref = resolve_bitmap_ref(data_abs, bitmap_names, stage)
        s_ref = resolve_bitmap_ref(shft_abs, bitmap_names, stage)
        lines.append(f'  {{ {width}, {flag_str}, {height},'
                     f' {d_ref}, {s_ref} }},  // [{i}]')
    lines.append('};')

    # Emit any remaining data (bitmap bytes that follow the LOD entries)
    if lod_end < len(data):
        remainder = data[lod_end:]
        rem_addr = sec.start_addr + lod_end
        rem_name = f'stage{stage}_bitmap_{rem_addr:04X}'
        lines.append('')
        lines.extend(emit_raw_array(rem_name, remainder, 8, rem_addr,
                                    use_pixels=True))
        bitmap_names[rem_addr] = rem_name  # register for cross-references

    return lines, n_lods


def emit_arrest_messages(stage: int, sec: Section,
                         defm_bytes: Dict[int, List[int]]) -> Tuple[List[str], int]:
    """Decode an arrest_msgs section using delay/DRAWCHARSTYLE/TWOBYTES/EOS macros.

    Returns (C lines, total byte count including embedded DEFM text).
    """
    nm = f'stage{stage}_arrest_messages_{sec.start_addr:04X}'
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
            if j == len(data) - 1:           # last byte carries EOS in top bit
                base = b & 0x7F
                lit = f"'{chr(base)}'" if 0x20 <= base <= 0x7E and chr(base) not in ("'", '\\') \
                      else f'0x{base:02X}'
                parts.append(f'{lit} | EOS')
            else:
                parts.append(f"'{chr(b)}'" if 0x20 <= b <= 0x7E and chr(b) not in ("'", '\\')
                             else f'0x{b:02X}')
        content.append('  ' + ', '.join(parts) + ',')
        total += len(data)

    i = 0
    # Initial delay
    if i < n and recs[i].rtype == 'B':
        content.append(f'  {val(recs[i])},  // initial delay')
        total += 1
        i += 1

    while i < n:
        # Message block: DEFB delay, DEFB flags, DEFB attr, DEFW backbuf, DEFW attr_addr
        if (i + 4 < n
                and recs[i].rtype == 'B'
                and recs[i + 1].rtype == 'B'
                and recs[i + 2].rtype == 'B'
                and recs[i + 3].rtype == 'W'
                and recs[i + 4].rtype == 'W'):
            style = DRAWCHARSTYLE_NAMES.get(val(recs[i + 1]), str(val(recs[i + 1])))
            defm_addr = recs[i + 4].addr + 2
            content.append('')
            content.append(f'  {val(recs[i])},  // delay')
            content.append(f'  {style},')
            content.append(f'  {val(recs[i + 2])},  // attribute')
            content.append(f'  TWOBYTES(0x{val(recs[i + 3]):04X}),  // backbuf')
            content.append(f'  TWOBYTES(0x{val(recs[i + 4]):04X}),  // attr')
            total += 7   # 3 × DEFB + 2 × DEFW
            if defm_addr in defm_bytes:
                emit_text(defm_bytes[defm_addr])
            i += 5
        else:
            # End markers: first byte = TRANSITIONCONTROL, last = DRAWOVERLAY_STOP
            content.append('')
            end_bytes = [b for r in recs[i:] for b in r.values]
            for k, b in enumerate(end_bytes):
                if k < len(end_bytes) - 1:
                    name = TRANSITIONCONTROL_NAMES.get(b, str(b))
                    content.append(f'  {name},  // transition_control')
                else:
                    content.append(f'  DRAWOVERLAY_STOP')
                total += 1
            break

    lines = [f'// ${sec.start_addr:04X}',
             '// clang-format off',
             f'static const u8 {nm}[{total}] = {{']
    lines.extend(content)
    lines.append('};')
    lines.append('// clang-format on')
    return lines, total


# ── Stage struct emitter ──────────────────────────────────────────────────────

def emit_stage_struct(stage: int, sections: List[Section],
                      abs_to_name: Dict[int, str],
                      bank_offset: int) -> List[str]:
    """Emit const stage_t stageN = { ... };"""

    def find_first(stype: str) -> Optional[Section]:
        return next((s for s in sections if s.stype == stype), None)

    backdrop_sec    = find_first('backdrop')
    perstage_sec    = find_first('perstage')
    difficulty_sec  = find_first('difficulty')
    setupdata_sec   = find_first('setupdata')
    attractdata_sec = find_first('attractdata')
    pilot_sec       = find_first('pilot_mugshot')
    lodaddrs_sec    = find_first('lodaddrs')

    lines: List[str] = []
    addr = backdrop_sec.start_addr if backdrop_sec else 0
    lines.append(f'// ${addr:04X}')
    lines.append(f'const stage_t stage{stage} = {{')

    # Backdrop – embedded inline
    if backdrop_sec:
        data = backdrop_sec.bytes_flat
        lines.append(f'  /* ${backdrop_sec.start_addr:04X} backdrop */')
        lines.append('  // clang-format off')
        lines.append('  {')
        for i in range(0, len(data), 10):
            row = data[i:i + 10]
            lines.append('    ' + ', '.join(byte_to_pixel(b) for b in row) + ',')
        lines.append('  },')
        lines.append('  // clang-format on')
    else:
        lines.append('  { 0 },  /* TODO: backdrop */')

    # perstage: perp face attributes, pilot mugshot, ground colour
    perstage_addr = perstage_sec.start_addr if perstage_sec else 0
    lines.append(f'  /* ${perstage_addr:04X} perstage */')
    lines.append(f'  &stage{stage}_perp_face[FACEBITMAPBYTES],')
    if pilot_sec:
        lines.append(f'  &stage{stage}_pilot_mugshot[0],')
    else:
        lines.append(f'  NULL,  /* no pilot mugshot on this stage */')
    if perstage_sec:
        pws = perstage_sec.words_with_annots
        gc = pws[2][0] if len(pws) > 2 else 0
        lines.append(f'  0x{gc:04X},')
    else:
        lines.append('  0,  /* TODO: ground_colour */')

    # Pointer fields (perstage words 3..13)
    pws = perstage_sec.words_with_annots if perstage_sec else []
    for i, field in enumerate(PERSTAGE_PTR_FIELDS):
        abs_a = pws[i + 3][1] if i + 3 < len(pws) else -1
        if abs_a >= 0:
            ref = resolve_section_ptr(abs_a, abs_to_name)
            if ref:
                lines.append(f'  {ref},  /* {field} */')
                continue
        hint = f' (${abs_a:04X})' if abs_a >= 0 else ''
        lines.append(f'  NULL,  /* TODO: {field}{hint} */')

    lines.append('')
    lws = lodaddrs_sec.words_with_annots if lodaddrs_sec else []

    def lod_ref(idx: int) -> str:
        if idx >= len(lws):
            return 'NULL'
        raw_w, abs_a, _ = lws[idx]
        if raw_w == 0:
            return 'NULL'
        if abs_a < 0:
            abs_a = raw_w + bank_offset
        ref = resolve_section_ptr(abs_a, abs_to_name)
        return ref if ref else f'NULL  /* TODO: ${abs_a:04X} */'

    lines.append(f'  {lod_ref(0)},  /* bitmaps_stones */')
    lines.append(f'  {lod_ref(1)},  /* bitmaps_dust */')
    lines.append(f'  {lod_ref(2)},  /* bitmaps_perp_car */')
    vehicles = ', '.join(lod_ref(3 + j) for j in range(4))
    lines.append(f'  {{ {vehicles} }},  /* bitmaps_vehicles */')
    lines.append('')

    # Difficulty: car_spawn_delay, perp_lane_change_base, perp_approach_base
    if difficulty_sec:
        diff = difficulty_sec.bytes_flat
        lines.append(f'  /* ${difficulty_sec.start_addr:04X} difficulty */')
        lines.append(f'  {diff[0] if diff else 0},  /* car_spawn_delay */')
        lines.append(f'  {diff[1] if len(diff) > 1 else 0},  /* perp_lane_change_base */')
        lines.append(f'  {diff[2] if len(diff) > 2 else 0},  /* perp_approach_base */')
    else:
        lines.append('  0,  /* TODO: car_spawn_delay */')
        lines.append('  0,  /* TODO: perp_lane_change_base */')
        lines.append('  0,  /* TODO: perp_approach_base */')
    lines.append('')

    def emit_scenedata(sec: Optional[Section], label: str) -> List[str]:
        if not sec:
            return [f'  {{ 0 }},  /* TODO: {label} */']
        ws = sec.words_with_annots
        sl = [f'  /* ${sec.start_addr:04X} {label} */', '  {']
        sl.append(f'    {ws[0][0] if ws else 0},')
        for idx in range(1, 7):
            if idx < len(ws):
                raw_w, abs_a, _ = ws[idx]
                if abs_a < 0:
                    abs_a = raw_w + bank_offset
                nm = abs_to_name.get(abs_a + 1)
                sl.append(f'    &{nm}[-1],' if nm
                          else f'    NULL,  /* TODO: ${abs_a + 1:04X} */')
            else:
                sl.append(f'    NULL,  /* TODO: missing word {idx} */')
        sl.append('  },')
        return sl

    lines.extend(emit_scenedata(setupdata_sec, 'setupdata'))
    lines.append('')
    lines.extend(emit_scenedata(attractdata_sec, 'attractdata'))
    lines.append('')
    lines.append(f'  stage{stage}_chatter_strings')
    lines.append('};')
    return lines


# ── Main conversion ───────────────────────────────────────────────────────────

def convert(skool_path: str, stage: int, obj_names: List[str]) -> None:
    records, section_comments = parse_skool(skool_path)
    bank_offset = compute_bank_offset(records)
    defm_strings, defm_bytes = parse_defm_map(skool_path)
    sections = split_into_sections(records, section_comments)

    # First pass: collect all bitmap section names so LOD tables can reference them
    bitmap_names: Dict[int, str] = {}
    for sec in sections:
        if sec.stype == 'bitmap':
            bname = array_name(stage, 'bitmap', sec.start_addr)
            bitmap_names[sec.start_addr] = bname

    # Also pre-register the remainder blobs that lod_table sections will emit,
    # so that earlier LOD tables can resolve offsets into later-defined blobs.
    # Collect forward declarations for those blobs too.
    lod_remainder_fwd: List[str] = []
    for sec in sections:
        if sec.stype != 'lod_table':
            continue
        data = sec.bytes_flat
        n = sum(1 for rec in sec.records
                if 'width (bytes)' in rec.comment.lower())
        if n == 0:
            n = len(data) // 7 if len(data) % 7 == 0 else 0
        valid = 0
        for j in range(n):
            off = j * 7
            if off + 1 >= len(data) or data[off + 1] not in (0, 1, 2, 3):
                break
            valid += 1
        lod_end = valid * 7
        if lod_end < len(data):
            rem_addr = sec.start_addr + lod_end
            rem_name = f'stage{stage}_bitmap_{rem_addr:04X}'
            rem_size = len(data) - lod_end
            bitmap_names[rem_addr] = rem_name
            lod_remainder_fwd.append(f'static const u8 {rem_name}[{rem_size}];')

    # ── Header ───────────────────────────────────────────────────────────────
    print(f'/**')
    print(f' * ChaseHQ-Stage{stage}Data.c  (generated by convert_stage.py)')
    print(f' *')
    print(f' * Review all TODO comments before use.')
    print(f' */')
    print()
    print('#include <assert.h>')
    print('#include <stddef.h>')
    print()
    print('#include "C99/Types.h"')
    print('#include "ZXSpectrum/Pixels.h"')
    print('#include "ZXSpectrum/Spectrum.h"')
    print()
    print('#include "../ChaseHQ.h"')
    print('#include "ChaseHQ-CommonData.h"')
    print()
    print(f'#include "ChaseHQ-Stage{stage}Data.h"')
    print()
    print('/* ----------------------------------------------------------------------- */')
    print()

    # Object macros header
    print(f'/* Stage {stage} object type macros */')
    for i, oname in enumerate(obj_names):
        if oname:
            print(f'#define MAP_OBJ_S{stage}_{oname}_VAL  ({i})')
    print()
    for i, oname in enumerate(obj_names):
        if oname:
            print(f'#define MAP_OBJ_S{stage}_{oname}(D)   '
                  f'(((D) << 4) | MAP_OBJ_S{stage}_{oname}_VAL)')
    print()
    print('/* ----------------------------------------------------------------------- */')
    print()

    # ── First pass: build abs_addr → array_name map ──────────────────────────
    map_stypes = {'curvature', 'height', 'lanes', 'hazards', 'leftobjs', 'rightobjs'}
    named_stypes = map_stypes | {
        'arrest_msgs', 'perp_desc', 'hazard_lods',
        'helicopter', 'obj_defs', 'lodaddrs', 'lod_table',
    }
    abs_to_name: Dict[int, str] = {}
    for sec in sections:
        if sec.stype in named_stypes:
            abs_to_name[sec.start_addr] = array_name(stage, sec.stype, sec.start_addr)

    # ── Generate code for each section ───────────────────────────────────────
    all_lines: List[str] = []
    goto_map: Dict[int, str] = {}  # raw_defw_value → array_name (for lookup_map_goto)
    fwd_decls: List[str] = []
    bitmap_sections = []

    for sec in sections:
        if sec.stype in map_stypes:
            lines, gm = emit_map_section(stage, sec.stype, sec, obj_names,
                                         bank_offset, abs_to_name)
            all_lines.extend(lines)
            all_lines.append('')
            goto_map.update(gm)
            nm = array_name(stage, sec.stype, sec.start_addr)
            fwd_decls.append(f'static const u8 {nm}[];')

        elif sec.stype == 'backdrop':
            fwd_decls.append('// backdrop declared inline in stage struct')

        elif sec.stype in ('perp_mugshot', 'pilot_mugshot'):
            data = sec.bytes_flat
            nm = array_name(stage, sec.stype, sec.start_addr)
            all_lines.extend(emit_face_mugshot_array(nm, data, sec.start_addr))
            all_lines.append('')
            fwd_decls.append(f'static const u8 {nm}[{len(data)}];')

        elif sec.stype == 'bitmap':
            data = sec.bytes_flat
            nm = array_name(stage, 'bitmap', sec.start_addr)
            # Try to parse width×height from comment (e.g. "Bitmap data 5 bytes x 8")
            m = re.search(r'(\d+)\s+bytes?\s+x\s+(\d+)', sec.header_comment, re.I)
            per = int(m.group(1)) * 2 if m and 'masked' in sec.header_comment.lower() else \
                  int(m.group(1)) if m else 8
            all_lines.extend(emit_raw_array(nm, data, per, sec.start_addr,
                                            use_pixels=True))
            all_lines.append('')
            fwd_decls.append(f'static const u8 {nm}[{len(data)}];')
            bitmap_sections.append((nm, sec.start_addr))

        elif sec.stype == 'lod_table':
            lines, n_lods = emit_lod_table(stage, sec, bank_offset, bitmap_names)
            all_lines.extend(lines)
            all_lines.append('')
            nm = array_name(stage, 'lod_table', sec.start_addr)
            if n_lods > 0:
                fwd_decls.append(f'static const bitmap_t {nm}[{n_lods}];')
            else:
                fwd_decls.append(f'static const u8 {nm}[{len(sec.bytes_flat)}];')

        elif sec.stype == 'arrest_msgs':
            lines, total = emit_arrest_messages(stage, sec, defm_bytes)
            all_lines.extend(lines)
            all_lines.append('')
            nm = array_name(stage, 'arrest_msgs', sec.start_addr)
            fwd_decls.append(f'static const u8 {nm}[{total}];')

        elif sec.stype == 'perp_desc':
            all_lines.extend(emit_perp_description(stage, sec))
            all_lines.append('')
            fwd_decls.append(f'static const u8 stage{stage}_perp_description[7];')

            # The first four DEFW words in the header are pointers to the
            # chatter strings (DEFM lines).  Resolve each via defm_strings.
            ws = sec.words_with_annots
            strings = [defm_strings[w[1]]
                       for w in ws[:4] if w[1] >= 0 and w[1] in defm_strings]
            if strings:
                chat_nm = f'stage{stage}_chatter_strings'
                all_lines.append(f'static const char *{chat_nm}[{len(strings)}] = {{')
                for s in strings:
                    all_lines.append(f'  {s},')
                all_lines.append('};')
                all_lines.append('')
                fwd_decls.append(f'static const char *{chat_nm}[{len(strings)}];')

        elif sec.stype == 'hazard_lods':
            data = sec.bytes_flat
            nm = array_name(stage, 'hazard_lods', sec.start_addr)
            all_lines.extend(emit_raw_array(nm, data, 3, sec.start_addr))
            all_lines.append('')
            fwd_decls.append(f'static const u8 {nm}[{len(data)}];')

        elif sec.stype == 'helicopter':
            data = sec.bytes_flat
            nm = array_name(stage, 'helicopter', sec.start_addr)
            all_lines.extend(emit_raw_array(nm, data, 8, sec.start_addr))
            all_lines.append('')
            fwd_decls.append(f'static const u8 {nm}[{len(data)}];')

        elif sec.stype == 'obj_defs':
            data = sec.bytes_flat
            nm = array_name(stage, 'obj_defs', sec.start_addr)
            all_lines.extend(emit_raw_array(nm, data, 7, sec.start_addr))
            all_lines.append('')
            fwd_decls.append(f'static const u8 {nm}[{len(data)}];')

        elif sec.stype == 'lodaddrs':
            data = sec.bytes_flat
            nm = array_name(stage, 'lodaddrs', sec.start_addr)
            all_lines.extend(emit_raw_array(nm, data, 2, sec.start_addr))
            all_lines.append('')
            fwd_decls.append(f'static const u8 {nm}[{len(data)}];')

        elif sec.stype == 'stretchy':
            data = sec.bytes_flat
            nm = array_name(stage, 'stretchy', sec.start_addr)
            all_lines.extend(emit_raw_array(nm, data, 8, sec.start_addr))
            all_lines.append('')
            fwd_decls.append(f'static const u8 {nm}[{len(data)}];')

        elif sec.stype in ('perstage', 'difficulty', 'setupdata', 'attractdata'):
            pass  # data inlined into stage_t struct by emit_stage_struct

        elif sec.stype in ('hittable', 'unknown'):
            all_lines.append(f'/* TODO: ${sec.start_addr:04X} [{sec.stype}]')
            all_lines.append(f'   {sec.header_comment}')
            # Emit raw hex as a comment
            data = sec.bytes_flat
            if data:
                hex_str = ' '.join(f'0x{b:02X}' for b in data[:64])
                if len(data) > 64:
                    hex_str += f' ... ({len(data)} bytes total)'
                all_lines.append(f'   Raw: {hex_str}')
            # For word sections, show annotations
            wlist = sec.words_with_annots
            if wlist:
                for raw_w, abs_a, waddr in wlist[:16]:
                    ann = f'→ ${abs_a:04X}' if abs_a >= 0 else '(out-of-bounds)'
                    all_lines.append(f'   ${waddr:04X}: DEFW ${raw_w:04X}  {ann}')
            all_lines.append('*/')
            all_lines.append('')

    # ── Emit forward declarations ─────────────────────────────────────────────
    print('/* Forward declarations */')
    for d in fwd_decls:
        print(d)
    for d in lod_remainder_fwd:
        print(d)
    print()
    print('/* ----------------------------------------------------------------------- */')
    print()

    # ── Stage struct ──────────────────────────────────────────────────────────
    for line in emit_stage_struct(stage, sections, abs_to_name, bank_offset):
        print(line)
    print()
    print('/* ----------------------------------------------------------------------- */')
    print()

    # ── Data sections ─────────────────────────────────────────────────────────
    for line in all_lines:
        print(line)

    # ── lookup_map_goto ───────────────────────────────────────────────────────
    tname = f'stage{stage}_map_goto_table'
    print(f'static const struct {{ u16 z80; const void *ptr; }} {tname}[] = {{')
    for raw_addr, name in sorted(goto_map.items()):
        if name:
            print(f'  {{ 0x{raw_addr:04X}, &{name}[0] }},')
        else:
            raw = raw_addr - bank_offset if raw_addr > bank_offset else raw_addr
            print(f'  /* TODO: 0x{raw:04X} -> find array name for abs ${raw_addr:04X} */')
    print('};')
    print()
    print(f'const void *stage{stage}_lookup_map_goto(chqstate_t *state, u16 z80)')
    print('{')
    print('  int lo, hi, mid;')
    print()
    print(f'  lo  = 0;')
    print(f'  hi  = (int)(sizeof({tname}) / sizeof({tname}[0])) - 1;')
    print(f'  while (lo <= hi) {{')
    print(f'    mid = lo + (hi - lo) / 2;')
    print(f'    if ({tname}[mid].z80 == z80) return {tname}[mid].ptr;')
    print(f'    if ({tname}[mid].z80 < z80)  lo = mid + 1;')
    print(f'    else                          hi = mid - 1;')
    print(f'  }}')
    print(f'  assert("Unknown Z80 address (stage {stage})" == NULL);')
    print('  return NULL;')
    print('}')


def main():
    parser = argparse.ArgumentParser(description='Convert skool file to C stage data.')
    parser.add_argument('skool', help='Input .skool file')
    parser.add_argument('stage', type=int, help='Stage number (2-5)')
    parser.add_argument('--obj-names', default='NONE,TUNNEL_LIGHT,OBJ2,SHORT_POLE,'
                        'OBJ4,OBJ5,OBJ6,OBJ7,OBJ8',
                        help='Comma-separated object type names for indices 0-8')
    args = parser.parse_args()

    obj_names = [n.strip() for n in args.obj_names.split(',')]
    # Pad or trim to exactly 16 (max nibble value 0xF = 15)
    while len(obj_names) < 16:
        obj_names.append(f'OBJ{len(obj_names)}')

    convert(args.skool, args.stage, obj_names)


if __name__ == '__main__':
    main()
