# convert_stage.py

`convert_stage.py` (project root) converts a Chase H.Q. skool file to a C stage data skeleton for one stage. It is the starting point for every stage data file beyond stage 1 (which is hand-crafted).

## Usage

```
python3 convert_stage.py <skool_file> <stage_num> [options] > ChaseHQ-StageNData.c
```

`skool_file` is one of the 128K bank skool files. A bank file can contain more than one stage ([Stage N] sections); `convert_stage.py` filters to the requested stage number and ignores sections belonging to other stages.

| Stage | Skool file                  |
| ----- | --------------------------- |
| 2     | `ChaseHQ-128K-bank-1.skool` |
| 3     | `ChaseHQ-128K-bank-6.skool` |
| 4     | `ChaseHQ-128K-bank-6.skool` |
| 5     | `ChaseHQ-128K-bank-7.skool` |

The CMake `convert_stages` target (`cmake --build <dir> --target convert_stages`) runs all four invocations automatically.

### Options

| Flag | Default | Effect |
| --- | --- | --- |
| `--obj-names A,B,...` | `NONE,TUNNEL_LIGHT,OBJ2,SHORT_POLE,OBJ4,...` | Names for all object type indices 0–15 |

Per-stage invocations (also encoded in `CMakeLists.txt`):

```bash
python3 convert_stage.py ChaseHQ-128K-bank-1.skool 2 \
    --obj-names NONE,TUNNEL_LIGHT,OBJ2,SHORT_POLE,HUGE_ROCK,PALM_TREE,LEAVES,DOUBLE_LAMP,OBJ8 \
    > C/ChaseHQ/Data/Stage2Data.c

python3 convert_stage.py ChaseHQ-128K-bank-6.skool 3 \
    --obj-names NONE,TUNNEL_LIGHT,OVERHEAD_BRIDGE,SHORT_POLE,TOWER_BLOCK,SPEED_LIMIT_SIGN,TELEGRAPH_POLE,OBJ7 \
    > C/ChaseHQ/Data/Stage3Data.c

python3 convert_stage.py ChaseHQ-128K-bank-6.skool 4 \
    --obj-names NONE,TUNNEL_LIGHT,OBJ2,SHORT_POLE,NEAR_COLUMN,FAR_COLUMN,PILE_OF_ROCKS,STREET_LAMP,TURN_SIGN_POINTING_LEFT,TURN_SIGN_POINTING_RIGHT \
    > C/ChaseHQ/Data/Stage4Data.c

python3 convert_stage.py ChaseHQ-128K-bank-7.skool 5 \
    --obj-names NONE,TUNNEL_LIGHT,OVERHEAD_BRIDGE,OBJ3,CACTUS,DOUBLE_STREET_LAMP,HUGE_ROCK,TELEGRAPH_POLE \
    > C/ChaseHQ/Data/Stage5Data.c
```

## What the script generates

| Section | Output type | Notes |
| --- | --- | --- |
| Backdrop | `u8[]` inline in `stage_t` | Pixel macro names from `Pixels.h` |
| Map data (curvature/height/lanes/hazards/left-objs/right-objs) | `u8[]` of `MAP_*` macros | GOTO/SPLIT targets resolved |
| Vehicle/hazard bitmap data | `u8[]` | Pixel macro names |
| LOD table entries | `bitmap_t[]` | See LOD auto-detection below |
| Mugshot/face data | `u8[]` | Pixel + attribute macro names |
| Perp description, arrest messages | `u8[]` | `CHATTERCHR_*`, `DRAWCHARSTYLE_*` macros |
| Helicopter data, object definitions, hazard LODs | `u8[]` | Raw hex |
| LOD address table | `u8[]` | Raw hex |
| Stretchy graphic data | `stretchy_t[]` / `depthset_t` | Typed structs |
| Forward declarations | — | For all generated arrays |
| `const stage_t stageN` | Struct initialiser | Partially filled |
| `stageN_lookup_map_goto()` | `switch` body | All GOTO/SPLIT targets |

## What requires manual completion

The following items cannot be decoded automatically and are left as `/* TODO */` comments or raw `u8[]` placeholders:

- `obj_t` and `hittable_t` graphic definition arrays (typed structs)
- `stretchy_t` / `depthset_t` tables when pointer targets are unresolvable
- Handler pointers not in `HANDLER_ADDRESS_MAP`

## LOD auto-detection

Each LOD (Level of Detail) entry is 7 bytes in the Z80 format: width, flags, height, data-ptr-lo, data-ptr-hi, shifted-ptr-lo, shifted-ptr-hi. The skool annotates entries it knows about with an inline `Width (bytes)` comment.

The script counts those annotated entries, then extends the count by consuming additional consecutive 7-byte groups that pass a validity check:

- `flags ∈ {0, 1, 2, 3}` (matches `BITMAPFLAG_*` enum values)
- `width ∈ 1..8`
- `height ≠ 0`

This auto-extension is necessary for stages where the skool only annotates the first few LODs but the table continues with unannotated entries (e.g. stage 5 has 5 annotated entries but 15 in total).

Any bytes that follow the last valid LOD entry are emitted as a separate `stageN_bitmap_XXXX` pixel array, which is then used as the `data`/`shifted` base for the LODs that reference it.

## Bank offset

The script auto-detects a global bank offset (the difference between Z80 absolute addresses stored as `DEFW` and their raw values) from the first annotated pointer in the file. For all stage skool files the global offset is `$6400`.

Some LOD table sections use a different local pointer base (stage 2's stretchy graphic data uses `$8400` rather than `$6400`). When annotated `DEFW` records within a section imply a different offset, the script derives a per-section `local_offset` and uses it as the fallback for un-annotated entries within that section.

## Stage 1

Stage 1 data (`Stage1Data.c`) is hand-crafted and should not be regenerated with this script.
