# graphics_png.py

`graphics_png.py` (`C/scripts/`) exports every graphic bitmap in the C port to a
single PNG sheet, and imports a sheet's pixels back into the C sources.

## Usage

```
python3 graphics_png.py export sheet.png
python3 graphics_png.py import sheet.png
```

Requires Pillow (`pip install pillow`).

## What's on the sheet

`export` scans `CommonData.c` and `Stage{1-5}Data.c` for every
`static const u8 name[...] = { ... };` array written entirely in `Pixels.h`
bit-pattern macros (see the project's bitmap convention), plus each stage's
`backdrop[]` block, and draws one cell per graphic, stacked in a single column.
Cells are as wide as the largest graphic found, but each keeps its own height
(no wasted grey space padding small graphics out to the tallest one). The top
block is `CommonData.c`
graphics (streetlamps, tunnel lights, helicopter parts, arrow, ...); the
following blocks are stage1..stage5, in order. `stage6` is a port-added test
level (only built under `CHQ_ENABLE_TEST_STAGE`, default OFF) and is exempt
from the sheet.

The manifest is not hand-maintained — it is re-discovered from the current
source on every run, so the column always matches whatever is currently
committed. This also means `import` must be run against a sheet produced by
`export` from the _same_ revision of the sources: cell positions are derived
from array names/sizes, so editing an array's declared dimensions between
export and import will desync the column.

`import` reads each cell back to bytes (a pixel darker than 50% grey is ink,
lighter is paper), converts each byte to its `Pixels.h` macro name, and replaces
the corresponding array's body in place, preserving the file's existing row
width and indentation.

## Orientation

Sprites and backdrops are drawn on the sheet flipped vertically (source row 0 at
the bottom of the cell): array row order in the C source runs bottom-to-top for
these graphics, so flipping gives the sheet the right way up. Face mugshots are
stored top-to-bottom already and are not flipped.

## Masked sprites

Some `bitmap_t` sprites carry `BITMAPFLAG_MASKED` data: instead of one plain
pixel byte per column, the source array interleaves a mask byte and a data byte
per column (`AND mask, OR data` against the back buffer). These render with mask
bit 1 (see-through) drawn as opaque magenta (`255, 0, 255`), and mask bit 0 as
an opaque ink/paper pixel from the data byte. Magenta is deliberately distinct
from the canvas grey, ink and paper, so a genuine edit can't be mistaken for a
transparent pixel. Import reverses this: a pixel matching that magenta exactly
is read back as masked out (mask=1), everything else is thresholded into the
data bit as normal.

Only arrays wholly dedicated to masked data (every `BITMAPFLAG_MASKED` reference
to them starts at offset 0) get this treatment. A handful of arrays — e.g.
`bitmap_helicopter[]`, whose rotor/body parts are sliced out at various non-zero
offsets, some masked and some not — can't be split into "masked" vs "plain" on
one sheet, so they render as plain monochrome instead.

## Colour

Most sprites (`bitmap_t` graphics — vehicles, dust, streetlamps, ...) render
monochrome (black ink / white paper): they carry no attribute byte of their own,
so there's no colour to source. Face mugshots and backdrops do have real colour
and are rendered with it:

- Faces: `bitmap_faces[]` interleaves 40 bitmap rows with 5 real attribute rows
  per face; those attribute bytes (`attribute_*` / `MKATTR(...)`) are decoded to
  ink/paper RGB per 8×8 cell.
- Backdrops: tinted using each stage's `ground_colour` field for the bottom 11
  rows and a fixed sky colour (`attribute_BRIGHT_BLACK_OVER_CYAN`,
  `Main.c:15690`) for the rest — an approximation of the real per-row sky
  banding in `set_playfield_attrs`, not a pixel-exact match.

Colour is export-only. `import` always reconstructs plain ink/paper bits from
pixel darkness — attribute bytes are never edited, so recolouring a face or
backdrop cell in an image editor has no effect on import.

A `width * height + N` size expr (e.g. `2 * 2 * 4 + 7`) is a real image
followed by `N` scaffold bytes the source itself marks as unreached (a
"further bytes, not reached by any LOD entry" comment): the real
`width * height` block renders normally — masked, if the array qualifies —
and the trailing `N` bytes render as extra plain monochrome rows appended
below it, since they aren't real mask/data pairs. These leftover bytes get
their own row width (capped like the `MAX_ROW_BYTES` blobs below), independent
of the real image's width — a narrow sprite's leftover bytes would otherwise
stretch into an oddly tall, skinny noise strip instead of a squarish blob.

Arrays whose C size expression can't be dimensioned at all (a few
scaffold/undimensioned blobs, e.g. leftover `bitmap_<hexaddr>` tables) are
reshaped to a fixed 32-byte row width purely to keep the sheet a sane size; this
changes their on-sheet row wrapping but not the underlying byte order, so
round-tripping is still lossless.

## Frame-major tables: font and transition masks

`font[]` and the four `*_transition_frames[]` tables (`spiral`, `circle`,
`square`, `diamond` — the growing wipe/reveal masks used for scene
transitions) are declared as `N * H`, but unlike a real `width * height`
sprite this means N frames/glyphs of H rows each, stored frame-by-frame (all
of frame 0's rows, then all of frame 1's, ...), not one row-major image N
bytes wide. These five names are special-cased (`FRAME_MAJOR_ARRAYS`) to tile
the N frames side by side on the sheet instead of reading the array as a
single flat bitmap.

## Verifying a round trip

Running `import` on a sheet produced by `export`, with no edits, should be a
content no-op:

```
python3 graphics_png.py export /tmp/sheet.png
python3 graphics_png.py import /tmp/sheet.png
git diff --stat libraries/ChaseHQ/Data/   # only cosmetic row-wrap diffs, if any
git checkout -- libraries/ChaseHQ/Data/
```
