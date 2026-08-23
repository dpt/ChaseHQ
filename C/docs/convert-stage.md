# convert_stage.py

`convert_stage.py` (`C/scripts/`) converts a Chase H.Q. skool file to a C stage
data skeleton for one stage. It is the starting point for every stage data file
beyond stage 1 (which is hand-crafted).

> **The output is a skeleton, not a build product.** It compiles, but the
> committed `Stage{2-5}Data.c` files contain hand-work the script cannot
> reproduce — decoded `overhead_span_t` tables, resolved helicopter pointers,
> named `STAGE{N}_MAP_*_ADDR` macros, a Doxygen prologue on every table.
> Regenerating over a committed file discards all of it and reintroduces the
> `/* TODO */` placeholders. Use this to scaffold a _new_ stage; to change an
> existing one, edit the committed `.c` by hand. See
> [What requires manual completion](#what-requires-manual-completion).

## Usage

```
python3 convert_stage.py <skool_file> <stage_num> [options] > StageNData.c
```

`skool_file` is one of the 128K bank skool files. A bank file can contain more
than one stage ([Stage N] sections); `convert_stage.py` filters to the requested
stage number and ignores sections belonging to other stages.

| Stage | Skool file                  |
| ----- | --------------------------- |
| 2     | `ChaseHQ-128K-bank-1.skool` |
| 3     | `ChaseHQ-128K-bank-6.skool` |
| 4     | `ChaseHQ-128K-bank-6.skool` |
| 5     | `ChaseHQ-128K-bank-7.skool` |

The CMake `convert_stages` target
(`cmake --build <dir> --target convert_stages`) runs all four invocations
automatically. It **overwrites the committed `Stage{2-5}Data.c` files with
skeletons that do not compile** — it is a scaffolding tool kept for reference,
not part of the normal build. Commit or stash your work before running it, and
`git checkout` the `Data/` directory afterwards unless you intend to redo the
manual completion by hand.

### Options

| Flag                  | Default                                      | Effect                            |
| --------------------- | -------------------------------------------- | --------------------------------- |
| `--obj-names A,B,...` | `NONE,TUNNEL_LIGHT,OBJ2,SHORT_POLE,OBJ4,...` | Names for object type indices 0–8 |

Per-stage invocations (also encoded in `CMakeLists.txt`), run from the
repository root:

```bash
python3 C/scripts/convert_stage.py Speccy/ChaseHQ-128K-bank-1.skool 2 \
    --obj-names NONE,TUNNEL_LIGHT,OBJ2,SHORT_POLE,HUGE_ROCK,PALM_TREE,LEAVES,DOUBLE_LAMP,OBJ8 \
    > C/libraries/ChaseHQ/Data/Stage2Data.c

python3 C/scripts/convert_stage.py Speccy/ChaseHQ-128K-bank-6.skool 3 \
    --obj-names NONE,TUNNEL_LIGHT,OVERHEAD_BRIDGE,SHORT_POLE,TOWER_BLOCK,SPEED_LIMIT_SIGN,TELEGRAPH_POLE,OBJ7 \
    > C/libraries/ChaseHQ/Data/Stage3Data.c

python3 C/scripts/convert_stage.py Speccy/ChaseHQ-128K-bank-6.skool 4 \
    --obj-names NONE,TUNNEL_LIGHT,OBJ2,SHORT_POLE,NEAR_COLUMN,FAR_COLUMN,PILE_OF_ROCKS,STREET_LAMP,TURN_SIGN_POINTING_LEFT,TURN_SIGN_POINTING_RIGHT \
    > C/libraries/ChaseHQ/Data/Stage4Data.c

python3 C/scripts/convert_stage.py Speccy/ChaseHQ-128K-bank-7.skool 5 \
    --obj-names NONE,TUNNEL_LIGHT,OVERHEAD_BRIDGE,OBJ3,CACTUS,DOUBLE_STREET_LAMP,HUGE_ROCK,TELEGRAPH_POLE \
    > C/libraries/ChaseHQ/Data/Stage5Data.c
```

## What the script generates

| Section                                                        | Output type                   | Notes                                    |
| -------------------------------------------------------------- | ----------------------------- | ---------------------------------------- |
| Backdrop                                                       | `u8[]` inline in `stage_t`    | Pixel macro names from `Pixels.h`        |
| Map data (curvature/height/lanes/hazards/left-objs/right-objs) | `u8[]` of `MAP_*` macros      | GOTO/SPLIT targets resolved              |
| Vehicle/hazard bitmap data                                     | `u8[]`                        | Pixel macro names                        |
| LOD table entries                                              | `bitmap_t[]`                  | See LOD auto-detection below             |
| Mugshot/face data                                              | `u8[]`                        | Pixel + attribute macro names            |
| Perp description, arrest messages                              | `u8[]`                        | `CHATTERCHR_*`, `DRAWCHARSTYLE_*` macros |
| Helicopter data, object definitions, hazard LODs               | `u8[]`                        | Raw hex                                  |
| LOD address table                                              | `u8[]`                        | Raw hex                                  |
| Stretchy graphic data                                          | `stretchy_t[]` / `depthset_t` | Typed structs                            |
| Forward declarations                                           | —                             | For all generated arrays                 |
| `const stage_t stageN`                                         | Struct initialiser            | Partially filled                         |
| `stageN_lookup_map_goto()`                                     | `switch` body                 | All GOTO/SPLIT targets                   |

## What requires manual completion

The output compiles as generated, but the following items cannot be decoded
automatically and are left as `/* TODO */` comments or raw `u8[]` placeholders:

- `overhead_span_t` tables, emitted as raw `stageN_bitmap_XXXX` byte arrays
- `stretchy_t` / `depthset_t` tables when pointer targets are unresolvable
- Handler pointers not in `HANDLER_ADDRESS_MAP`
- Helicopter data pointers, left `NULL` with a TODO
- The `stageN_map_*` section addresses, which the committed files hoist into
  named `STAGE{N}_MAP_*_ADDR` defines; the script emits bare literals
- `stageN_lod_addrs_XXXX`, a raw table the committed files delete once the typed
  `bitmap_t` LODs supersede it
- A Doxygen `/** $XXXX: name */` prologue on each table; the script emits a bare
  `// $XXXX`
- Pointers the committed files hand-craft, such as stage 2's
  `&stage2_left_obj_defs_E25B[-1].arg` — the script emits the array element,
  which warns on the pointer type

For stages 2-5 that is roughly 90 `_ADDR` macros, 280 prologues and 2
`overhead_span_t` decodes; no TODO placeholders remain in the committed files.

## Reading a re-controlled skool

Parts of the bank skool files have lost their `T` and `W` control directives, so
SkoolKit writes text and pointers in pieces:

```
 $E145 DEFM "THIS IS "     ; the rest of the string follows as DEFBs
 $E14D DEFB $4E
 ...
 $E904 DEFB $7D            ; [$E97D] one DEFW pointer, written as two DEFBs
 $E905 DEFB $65
```

The converter rejoins both forms — `parse_defm_map` stitches DEFB continuation
lines onto a text run until the bit-7 terminator, and `coalesce_split_words`
turns an annotated DEFB pair back into the `DEFW` record every decoder expects.
Without the second of those a split pointer does not merely go missing, it
shifts every later field of the table by a slot; that is how stage 2's LOD
entries came out paired against the wrong bitmaps and two `obj_t` entries
vanished. Decoders that walk a fixed-stride table (`emit_obj_array`) step by
byte offset rather than by record shape for the same reason — a _null_ pointer
carries no `[$XXXX]` annotation for the coalescing pass to recognise.

`C/scripts/check_stage_converter.py` guards this: it runs all four stages and
asserts every `MAP_*` macro emitted is defined in `Stages.h` and that each
stage's four chatter lines decode whole and terminated.

## LOD auto-detection

Each LOD (Level of Detail) entry is 7 bytes in the Z80 format: width, flags,
height, data-ptr-lo, data-ptr-hi, shifted-ptr-lo, shifted-ptr-hi. The skool
annotates entries it knows about with an inline `Width (bytes)` comment.

The script counts those annotated entries, then extends the count by consuming
additional consecutive 7-byte groups that pass a validity check:

- `flags ∈ {0, 1, 2, 3}` (matches `BITMAPFLAG_*` enum values)
- `width ∈ 1..8`
- `height ≠ 0`

This auto-extension is necessary for stages where the skool only annotates the
first few LODs but the table continues with unannotated entries (e.g. stage 5
has 5 annotated entries but 15 in total).

## One array per sprite

A graphics run is undivided in the Z80, but the LOD tables point at individual
sprites inside it. Emitting the run as one array leaves every entry indexing a
shared blob:

```c
{ 2, BITMAPFLAG_MASKED, 1, &stage2_bitmap_F768[338], &stage2_bitmap_F768[390] },
```

That records an offset but says nothing about where one sprite ends and the next
begins, and renumbers every later sprite whenever an earlier one is re-measured.
`split_into_sprites` cuts each run at every address a LOD entry points at, so
each sprite becomes its own array named for its own Z80 address:

```c
{ 2, BITMAPFLAG_MASKED, 1, &stage2_bitmap_F8AA[0], &stage2_bitmap_F8DE[0] },
```

Pre-shifted pointers are cut on too — they address a second copy of the sprite
further down the same run. The cut points are gathered from _every_ LOD table in
the stage, not just the one a run happens to trail: stage 2's `$E953` run holds
the sprites that the `$E8FF` and `$E929` tables point at.

Both the bytes that follow a LOD table and standalone bitmap sections are split
this way. Across stages 2-5 this takes the number of offset-into-blob references
from 690 to 2.

**One cut is refused.** Where two sprites overlap in the original data the cut
between them cannot be made: stage 5's `$D86C` is 1×5 = 5 bytes but the next
sprite starts 4 bytes later. Indexing one shared blob absorbs that overlap;
separate arrays would not, since nothing guarantees how the compiler lays two
arrays out. Such neighbours stay merged and resolve as an offset, which is where
the remaining 2 references come from. `check_stage_converter.py` asserts no
sprite overruns the array it points into.

## Sprite arrays carry their shape

Once a sprite has its own array the LOD entry describing it gives the array a
shape, so it is written one row per line and declares that shape rather than a
byte count:

```c
static const u8 stage4_bitmap_E870[6 * 30] = {
  ____XXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXX_____,
  _XXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXXXX, XXXXXX__,
  ...
```

A masked sprite interleaves a mask byte with each pixel byte, so it is twice as
wide on the page and declares `width * 2 * height` — the same convention the
hand-written `Stage1Data.c` uses (`stage1_bitmap_tree_top_48x10[6 * 2 * 10]`).
Forward declarations repeat the expression.

`sprite_layout` falls back to a flat byte count and 8 per row when the shape
does not account for every byte of the block. That covers the refused cut above
and the sprites whose stored extent exceeds `width * height` — a 4×4 at stage
2's `$F3D4` occupies 21 bytes, and the `BITMAPFLAG_FLIPPED` layout that explains
it is not modelled here. Declaring `4 * 4` for a 21-byte block would be a lie
about the data, so those stay flat: 334 of 360 arrays are shaped.

## Bank offset

The script auto-detects a global bank offset (the difference between Z80
absolute addresses stored as `DEFW` and their raw values) from the first
annotated pointer in the file. For all stage skool files the global offset is
`$6400`.

Some LOD table sections use a different local pointer base (stage 2's stretchy
graphic data uses `$8400` rather than `$6400`). When annotated `DEFW` records
within a section imply a different offset, the script derives a per-section
`local_offset` and uses it as the fallback for un-annotated entries within that
section.

## Stage 1

Stage 1 data (`Stage1Data.c`) is hand-crafted and should not be regenerated with
this script.
