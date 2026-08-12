#!/usr/bin/env python3
"""
graphics_png.py - export/import Chase H.Q. C port graphics as a single PNG
sheet.

Usage:
    graphics_png.py export sheet.png
    graphics_png.py import sheet.png

Every `static const u8 name[...] = { ... };` array in CommonData.c and
Stage{1-6}Data.c whose body is written entirely in Pixels.h bit-pattern
macros (X_X__X_X etc.) is treated as one graphic, plus each stage's
backdrop[] block. Entries are auto-discovered by scanning the source, not
hand-listed, so the manifest always matches whatever is currently in the
repo. See docs/graphics-png-format.md.

Most sprites render monochrome (ink=black, paper=white) -- most bitmap_t
sprites carry no attribute byte of their own (colour comes from whatever
ATTR cell the game has already painted that frame, not from the bitmap
data). Face mugshots and backdrops DO have known colour: mugshots carry
real attribute bytes alongside their bitmap, and backdrops are tinted with
each stage's ground_colour plus a fixed sky colour, both rendered here.
Colour is export-only preview; import always reconstructs monochrome
ink/paper bits, never edits attribute bytes.
"""
import argparse
import re
import sys
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parent.parent
DATA_DIR = ROOT / "libraries" / "ChaseHQ" / "Data"
PIXELS_H = ROOT / "include" / "ZXSpectrum" / "Pixels.h"
SPECTRUM_H = ROOT / "include" / "ZXSpectrum" / "Spectrum.h"

# stage6 is a port-added test level, only built under CHQ_ENABLE_TEST_STAGE
# (default OFF) and not part of the original game; exempt from the sheet.
DATA_FILES = ["CommonData.c"] + [f"Stage{n}Data.c" for n in range(1, 6)]

CELL_GAP = 2
COLS = 20
MAX_ROW_BYTES = 32  # cap on width_bytes for reshaping oversized/flat blobs
INK = (0, 0, 0, 255)
TRANSPARENT = (255, 0, 255, 255)  # masked-out pixel; opaque magenta, distinct from canvas grey/ink/paper
PAPER = (255, 255, 255, 255)
SECTION_GAP_ROWS = 1

FACEBYTES = 180
FACEBITMAPBYTES = 160  # 4 bytes/row * 40 rows
FACEROWBYTES = 4
FACEHEIGHT = 40
FACEATTRBYTES = 20  # 4 bytes/row * 5 rows
NFACES = 3

BACKDROP_GROUND_ROWS = 11  # Main.c:15775 -- bottom rows painted with ground_colour
SKY_ATTR_EXPR = "attribute_BRIGHT_BLACK_OVER_CYAN"  # Main.c:15690

# ZX Spectrum ink/paper palette (normal, bright), indexed 0-7 by ATTR_*.
NORMAL_PALETTE = [
    (0, 0, 0), (0, 0, 197), (197, 0, 0), (197, 0, 197),
    (0, 197, 0), (0, 197, 197), (197, 197, 0), (197, 197, 197),
]
BRIGHT_PALETTE = [
    (0, 0, 0), (0, 0, 255), (255, 0, 0), (255, 0, 255),
    (0, 255, 0), (0, 255, 255), (255, 255, 0), (255, 255, 255),
]


def default_colour_fn(px_row, px_col):
    return INK, PAPER


def load_attribute_table():
    """Maps ATTR_* names to 0-7 and attribute_* enum names to (bright, paper_idx, ink_idx)."""
    text = SPECTRUM_H.read_text()
    attr_idx = {
        name: int(value)
        for name, value in re.findall(r"#define\s+(ATTR_\w+)\s+\((\d+)\)", text)
    }
    attributes = {}
    for name, f, b, paper, ink in re.findall(
        r"(attribute_\w+)\s*=\s*MKATTR\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\w+)\s*,\s*(\w+)\s*\)",
        text,
    ):
        attributes[name] = (int(b), attr_idx.get(paper, _try_int(paper)), attr_idx.get(ink, _try_int(ink)))
    return attr_idx, attributes


def _try_int(s):
    return int(s) if s.isdigit() else 0


MKATTR_CALL_RE = re.compile(r"MKATTR\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\w+)\s*,\s*(\w+)\s*\)")


def attr_expr_to_ink_paper(expr, attr_idx, attributes):
    """Decodes an 'attribute_NAME' or 'MKATTR(F,B,P,I)' source expression to (ink_rgb, paper_rgb)."""
    m = MKATTR_CALL_RE.search(expr)
    if m:
        _flash, bright, paper, ink = m.groups()
        bright = int(bright)
        paper_idx = attr_idx.get(paper, _try_int(paper))
        ink_idx = attr_idx.get(ink, _try_int(ink))
    else:
        m = re.search(r"attribute_\w+", expr)
        if not m or m.group(0) not in attributes:
            sys.exit(f"unrecognised attribute expression: {expr!r}")
        bright, paper_idx, ink_idx = attributes[m.group(0)]
    palette = BRIGHT_PALETTE if bright else NORMAL_PALETTE
    ink_rgb = palette[ink_idx] + (255,)
    paper_rgb = palette[paper_idx] + (255,)
    return ink_rgb, paper_rgb


def section_of(filename):
    if filename == "CommonData.c":
        return "common"
    m = re.match(r"Stage(\d)Data\.c", filename)
    return f"stage{m.group(1)}"


def load_macro_table():
    text = PIXELS_H.read_text()
    name_to_value = {}
    for name, value in re.findall(r"#define\s+([X_]{8})\s+\((\d+)\)", text):
        name_to_value[name] = int(value)
    if len(name_to_value) != 256:
        sys.exit(f"expected 256 Pixels.h macros, found {len(name_to_value)}")
    value_to_name = {v: k for k, v in name_to_value.items()}
    return name_to_value, value_to_name


TOKEN_RE = re.compile(r"[X_]{8}")


def height_of(size_expr, total_bytes):
    if "*" not in size_expr:
        return 1
    factors = [f.strip() for f in size_expr.split("*")]
    if factors and factors[-1].isdigit():
        height = int(factors[-1])
        if height > 0 and total_bytes % height == 0:
            return height
    return 1


def line_indent(text, offset):
    """Leading whitespace of the source line containing `offset`."""
    line_start = text.rfind("\n", 0, offset) + 1
    return re.match(r"[ \t]*", text[line_start:offset]).group(0)


class Entry:
    def __init__(
        self, name, filename, span, width_bytes, height, total_bytes=None,
        colour_fn=None, indent="  ", trailing="\n", flip_v=True, masked=False,
    ):
        self.name = name
        self.filename = filename
        self.span = span  # (start, end) offsets of the array body in the file
        self.width_bytes = width_bytes  # raw row width; for masked entries this is 2x the visual width (mask byte + data byte per column)
        self.height = height
        self.total_bytes = total_bytes if total_bytes is not None else width_bytes * height
        self.section = section_of(filename)
        self.colour_fn = colour_fn or default_colour_fn
        self.indent = indent
        self.trailing = trailing  # text appended after the last row (closing brace's indent, or nothing)
        self.flip_v = flip_v  # sheet row order vs. source row order; mugshots read the right way up already
        self.masked = masked  # BITMAPFLAG_MASKED data: interleaved (mask byte, data byte) pairs per column

    @property
    def px_w(self):
        if self.masked:
            return (self.width_bytes // 2) * 8
        return self.width_bytes * 8

    @property
    def px_h(self):
        return self.height


ARRAY_RE = re.compile(
    r"(?:static )?const pixel_t (\w+)\[([^\]]*)\]\s*=\s*\{(.*?)\};", re.DOTALL
)


def discover_arrays(filename, text, name_to_value):
    entries = []
    for m in ARRAY_RE.finditer(text):
        name, size_expr, body = m.groups()
        body_nocomments = re.sub(r"//[^\n]*", "", body)
        tokens = [t.strip() for t in body_nocomments.replace("\n", " ").split(",")]
        tokens = [t for t in tokens if t]
        if not tokens or not all(t in name_to_value for t in tokens):
            continue
        total = len(tokens)
        height = height_of(size_expr, total)
        width_bytes = total // height
        # ponytail: reshape oversized/undimensioned blobs (e.g. unsplit
        # scaffold data) to a bounded row width so the sheet stays a sane
        # size; byte order is unaffected, so the round trip is unaffected.
        if width_bytes > MAX_ROW_BYTES:
            width_bytes = MAX_ROW_BYTES
            height = -(-total // width_bytes)  # ceil division
        body_start, body_end = m.start(3), m.end(3)
        first_token = re.search(r"\S", text[body_start:body_end])
        indent = line_indent(text, body_start + first_token.start()) if first_token else "  "
        trailing = "\n" + (indent[:-2] if len(indent) >= 2 else "")
        entries.append(
            Entry(name, filename, (body_start, body_end), width_bytes, height, total, indent=indent, trailing=trailing)
        )
    return entries


def find_backdrop(filename, text):
    stage_n = filename[len("Stage") : filename.index("Data.c")]
    marker = re.search(rf"const stage_t stage{stage_n} = \{{", text)
    if not marker:
        return None
    depth = 1  # already inside the outer '{' the regex matched
    inner_start = None
    for i in range(marker.end(), len(text)):
        c = text[i]
        if c == "{":
            depth += 1
            if depth == 2:
                inner_start = i + 1
        elif c == "}":
            if depth == 2:
                first_token = re.search(r"\S", text[inner_start:i])
                indent = line_indent(text, inner_start + first_token.start()) if first_token else "    "
                trailing = "\n" + (indent[:-2] if len(indent) >= 2 else "")
                return Entry(
                    f"stage{stage_n}.backdrop", filename, (inner_start, i), 10, 24, indent=indent, trailing=trailing
                )
            depth -= 1
    return None


TOKEN_OR_ATTR_RE = re.compile(r"[X_]{8}|attribute_\w+|MKATTR\([^)]*\)")


def _face_entry(name, filename, bitmap_matches, attr_matches, attr_idx, attributes):
    """Builds one coloured face Entry from bitmap+attribute token match lists."""
    if len(bitmap_matches) < FACEBITMAPBYTES or len(attr_matches) < FACEATTRBYTES:
        return None
    text_ref = bitmap_matches[0].string
    indent = line_indent(text_ref, bitmap_matches[0].start())
    span_start = text_ref.rfind("\n", 0, bitmap_matches[0].start())  # include the row's leading newline+indent
    span_end = bitmap_matches[-1].end()
    if text_ref[span_end : span_end + 1] == ",":
        span_end += 1  # fold the row's trailing comma into the span so it isn't duplicated
    span = (span_start, span_end)
    colour_grid = [attr_expr_to_ink_paper(m.group(0), attr_idx, attributes) for m in attr_matches]

    def colour_fn(px_row, px_col, grid=colour_grid):
        return grid[(px_row // 8) * FACEROWBYTES + px_col]

    return Entry(
        name, filename, span, FACEROWBYTES, FACEHEIGHT, FACEBITMAPBYTES,
        colour_fn, indent=indent, trailing="", flip_v=False,
    )


def find_faces(text, attr_idx, attributes):
    """Splits the shared bitmap_faces[] array into NFACES coloured face entries."""
    marker = re.search(r"const pixel_t bitmap_faces\[FACEBYTES \* NFACES\]\s*=\s*\{", text)
    if not marker:
        return []
    body_end = text.index("};", marker.end())
    body_start = marker.end()
    matches = list(TOKEN_OR_ATTR_RE.finditer(text, body_start, body_end))

    entries = []
    pos = 0
    for face_i in range(NFACES):
        bitmap_matches = matches[pos : pos + FACEBITMAPBYTES]
        attr_matches = matches[pos + FACEBITMAPBYTES : pos + FACEBITMAPBYTES + FACEATTRBYTES]
        pos += FACEBITMAPBYTES + FACEATTRBYTES
        entry = _face_entry(f"face_{face_i}", "CommonData.c", bitmap_matches, attr_matches, attr_idx, attributes)
        if entry is None:
            break
        entries.append(entry)
    return entries


SINGLE_FACE_RE = re.compile(r"(?:static )?const pixel_t (\w+)\[(?:FACEBYTES|180)\]\s*=\s*\{")


def find_single_faces(filename, text, attr_idx, attributes):
    """Per-stage standalone face/mugshot arrays (stage1_perp_face,
    stage2_pilot_mugshot, ...) -- same bitmap+attribute layout as
    bitmap_faces but one face per array, not part of the shared blob."""
    entries = []
    for m in SINGLE_FACE_RE.finditer(text):
        name = m.group(1)
        if name == "bitmap_faces":
            continue
        body_end = text.index("};", m.end())
        matches = list(TOKEN_OR_ATTR_RE.finditer(text, m.end(), body_end))
        bitmap_matches = matches[:FACEBITMAPBYTES]
        attr_matches = matches[FACEBITMAPBYTES : FACEBITMAPBYTES + FACEATTRBYTES]
        entry = _face_entry(name, filename, bitmap_matches, attr_matches, attr_idx, attributes)
        if entry is not None:
            entries.append(entry)
    return entries


GROUND_COLOUR_RE = re.compile(r"\}\s*,(.*?),(.*?),(.*?),", re.DOTALL)


def find_ground_colour_fn(filename, text, attr_idx, attributes):
    stage_n = filename[len("Stage") : filename.index("Data.c")]
    marker = re.search(rf"const stage_t stage{stage_n} = \{{", text)
    if not marker:
        return None
    m = GROUND_COLOUR_RE.search(text, marker.end())
    if not m:
        return None
    ground_ink, ground_paper = attr_expr_to_ink_paper(m.group(3), attr_idx, attributes)
    sky_ink, sky_paper = attr_expr_to_ink_paper(SKY_ATTR_EXPR, attr_idx, attributes)

    def colour_fn(px_row, px_col):
        if px_row >= 24 - BACKDROP_GROUND_ROWS:
            return ground_ink, ground_paper
        return sky_ink, sky_paper

    return colour_fn


BITMAP_T_RE = re.compile(
    r"\{\s*\d+\s*,\s*BITMAPFLAG_MASKED\s*,\s*\d+\s*,"
    r"\s*&(\w+)\[(\d+)\]\s*,\s*&(\w+)\[(\d+)\]\s*\}"
)


def find_masked_array_names(text):
    """Array names wholly given over to BITMAPFLAG_MASKED (mask byte, data
    byte) column pairs -- i.e. every masked reference to that array starts
    at offset 0. Arrays sliced at a non-zero offset (heli parts sharing one
    big bitmap_helicopter[] blob, some at masked offsets, some not) are left
    alone and rendered as plain monochrome, since a single array can't carry
    two different pixel encodings on the sheet.
    """
    names, excluded = set(), set()
    for data_name, data_off, shifted_name, shifted_off in BITMAP_T_RE.findall(text):
        for name, off in ((data_name, data_off), (shifted_name, shifted_off)):
            (names if off == "0" else excluded).add(name)
    return names - excluded


MAIN_C = ROOT / "libraries" / "ChaseHQ" / "Engine" / "Main.c"

# Main.c references shared data arrays by bare name (plus pointer arithmetic
# for slices), not CommonData.c's &name[N] form -- e.g. `bitmap_arrow` itself
# is declared in CommonData.c but only ever used in a BITMAPFLAG_MASKED
# bitmap_t literal here, so its maskedness is invisible to BITMAP_T_RE.
MAIN_BITMAP_T_RE = re.compile(
    r"\{\s*\d+\s*,\s*BITMAPFLAG_MASKED\b[^,]*,\s*\d+\s*,"
    r"\s*(\w+)(\s*\+\s*\d+)?\s*,\s*(\w+)(\s*\+\s*\d+)?\s*\}"
)


def find_masked_array_names_in_main():
    """Same offset-0-only rule as find_masked_array_names, applied to Main.c's
    bare-name bitmap_t literals instead of CommonData.c's &name[N] form."""
    text = MAIN_C.read_text()
    names, excluded = set(), set()
    for data_name, data_off, shifted_name, shifted_off in MAIN_BITMAP_T_RE.findall(text):
        for name, off in ((data_name, data_off), (shifted_name, shifted_off)):
            if name == "NULL":
                continue
            (excluded if off else names).add(name)
    return names - excluded


def build_manifest(name_to_value):
    attr_idx, attributes = load_attribute_table()
    manifest = []
    main_masked_names = find_masked_array_names_in_main()
    for filename in DATA_FILES:
        text = (DATA_DIR / filename).read_text()
        entries = discover_arrays(filename, text, name_to_value)
        masked_names = find_masked_array_names(text) | main_masked_names
        for e in entries:
            if e.name in masked_names and e.width_bytes % 2 == 0:
                e.masked = True
        backdrop = find_backdrop(filename, text)
        if backdrop is not None:
            colour_fn = find_ground_colour_fn(filename, text, attr_idx, attributes)
            if colour_fn is not None:
                backdrop.colour_fn = colour_fn
            entries.append(backdrop)
        if filename == "CommonData.c":
            entries.extend(find_faces(text, attr_idx, attributes))
        entries.extend(find_single_faces(filename, text, attr_idx, attributes))
        entries.sort(key=lambda e: e.name)
        manifest.extend(entries)
    return manifest


def layout(manifest, cell_w, cell_h):
    """Assign (col, row) grid positions, grouped by section, wrapped at COLS."""
    positions = {}
    row = 0
    for section in ["common"] + [f"stage{n}" for n in range(1, 7)]:
        section_entries = [e for e in manifest if e.section == section]
        if not section_entries:
            continue
        col = 0
        for e in section_entries:
            positions[id(e)] = (col, row)
            col += 1
            if col == COLS:
                col = 0
                row += 1
        if col != 0:
            row += 1
        row += SECTION_GAP_ROWS
    nrows = row
    return positions, nrows


def do_export(png_path):
    name_to_value, _ = load_macro_table()
    manifest = build_manifest(name_to_value)
    cell_w = max(e.px_w for e in manifest)
    cell_h = max(e.px_h for e in manifest)
    positions, nrows = layout(manifest, cell_w, cell_h)

    sheet_w = COLS * (cell_w + CELL_GAP) + CELL_GAP
    sheet_h = nrows * (cell_h + CELL_GAP) + CELL_GAP
    image = Image.new("RGBA", (sheet_w, sheet_h), (128, 128, 128, 255))

    for filename in DATA_FILES:
        text = (DATA_DIR / filename).read_text()
        for e in [m for m in manifest if m.filename == filename]:
            body = text[e.span[0] : e.span[1]]
            tokens = [t for t in TOKEN_RE.findall(body)]
            col, row = positions[id(e)]
            x0 = CELL_GAP + col * (cell_w + CELL_GAP)
            y0 = CELL_GAP + row * (cell_h + CELL_GAP)
            if e.masked:
                visual_width = e.width_bytes // 2
                for pair_i in range(0, len(tokens) // 2):
                    mask_tok, data_tok = tokens[pair_i * 2], tokens[pair_i * 2 + 1]
                    px_row, px_col = pair_i // visual_width, pair_i % visual_width
                    sheet_row = e.px_h - 1 - px_row if e.flip_v else px_row
                    for bit in range(8):
                        x, y = x0 + px_col * 8 + bit, y0 + sheet_row
                        # mask bit 1 = transparent (background shows through the AND);
                        # mask bit 0 = opaque, coloured by the data bit
                        pixel = TRANSPARENT if mask_tok[bit] == "X" else (INK if data_tok[bit] == "X" else PAPER)
                        image.putpixel((x, y), pixel)
                continue
            for byte_i, token in enumerate(tokens):
                px_row = byte_i // e.width_bytes
                px_col = byte_i % e.width_bytes
                ink, paper = e.colour_fn(px_row, px_col)
                sheet_row = e.px_h - 1 - px_row if e.flip_v else px_row
                for bit in range(8):
                    x = x0 + px_col * 8 + bit
                    y = y0 + sheet_row
                    image.putpixel((x, y), ink if token[bit] == "X" else paper)

    image.save(png_path)
    print(f"wrote {png_path}: {len(manifest)} graphics, {sheet_w}x{sheet_h}px")


def do_import(png_path):
    name_to_value, value_to_name = load_macro_table()
    manifest = build_manifest(name_to_value)
    cell_w = max(e.px_w for e in manifest)
    cell_h = max(e.px_h for e in manifest)
    positions, _ = layout(manifest, cell_w, cell_h)

    image = Image.open(png_path).convert("RGBA")

    for filename in DATA_FILES:
        path = DATA_DIR / filename
        text = path.read_text()
        edits = []  # (start, end, replacement)
        for e in [m for m in manifest if m.filename == filename]:
            col, row = positions[id(e)]
            x0 = CELL_GAP + col * (cell_w + CELL_GAP)
            y0 = CELL_GAP + row * (cell_h + CELL_GAP)
            tokens = []
            if e.masked:
                visual_width = e.width_bytes // 2
                for pair_i in range(e.total_bytes // 2):
                    px_row, px_col = pair_i // visual_width, pair_i % visual_width
                    sheet_row = e.px_h - 1 - px_row if e.flip_v else px_row
                    mask_bits, data_bits = "", ""
                    for bit in range(8):
                        r, g, b, a = image.getpixel((x0 + px_col * 8 + bit, y0 + sheet_row))
                        transparent = (r, g, b) == TRANSPARENT[:3]
                        mask_bits += "X" if transparent else "_"
                        data_bits += "_" if transparent else ("X" if (r + g + b) / 3 < 128 else "_")
                    tokens.append(mask_bits)
                    tokens.append(data_bits)
            else:
                for byte_i in range(e.total_bytes):
                    px_row = byte_i // e.width_bytes
                    px_col = byte_i % e.width_bytes
                    sheet_row = e.px_h - 1 - px_row if e.flip_v else px_row
                    bits = ""
                    for bit in range(8):
                        r, g, b, a = image.getpixel((x0 + px_col * 8 + bit, y0 + sheet_row))
                        lum = (r + g + b) / 3
                        bits += "X" if lum < 128 else "_"
                    tokens.append(bits)
            rows = [tokens[i : i + e.width_bytes] for i in range(0, len(tokens), e.width_bytes)]
            row_lines = [e.indent + ", ".join(row) + "," for row in rows]
            if not text[e.span[0] : e.span[1]].rstrip().endswith(","):
                row_lines[-1] = row_lines[-1].rstrip(",")
            new_body = "\n" + "\n".join(row_lines) + e.trailing
            edits.append((e.span[0], e.span[1], new_body))
        if not edits:
            continue
        edits.sort(key=lambda t: t[0], reverse=True)
        for start, end, replacement in edits:
            text = text[:start] + replacement + text[end:]
        path.write_text(text)

    print(f"updated {len(manifest)} graphics across {len(DATA_FILES)} files")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    export_p = sub.add_parser("export", help="write a PNG sheet from the current C sources")
    export_p.add_argument("png")
    import_p = sub.add_parser("import", help="write a PNG sheet's pixels back into the C sources")
    import_p.add_argument("png")
    args = parser.parse_args()

    if args.command == "export":
        do_export(args.png)
    else:
        do_import(args.png)


if __name__ == "__main__":
    main()
