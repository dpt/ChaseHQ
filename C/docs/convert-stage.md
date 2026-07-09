# convert_stage.py

`convert_stage.py` (project root) converts a Chase H.Q. skool file to a C stage data skeleton for one stage. It is the starting point for every stage data file beyond stage 1 (which is hand-crafted).

## Usage

```
python3 convert_stage.py <skool_file> <stage_num> [options] > ChaseHQ-StageNData.c
```

`skool_file` is typically one of the bank files (`ChaseHQ-128K-bank-3.skool`, etc.). `stage_num` is 2–5.

### Options

| Flag | Default | Effect |
| --- | --- | --- |
| `--obj-names A,B,C,D,E` | `OBJ4,OBJ5,OBJ6,OBJ7,OBJ8` | Names for object types 4–8 (beyond NONE/LIGHT/UNUSED/SHORT_POLE) |
| `--turn-signs` | off | Stage has turn signs (objects 8 and 9) |
| `--no-turn-signs` | on | Stage has no turn signs |

Per-stage invocations used for stages 2–5:

```bash
python3 convert_stage.py ChaseHQ-128K-bank-3.skool 2 \
    --obj-names TREE,LIGHT_RIGHT,LIGHT_LEFT,BUSH,BILLBOARD \
    > C/ChaseHQ/Data/ChaseHQ-Stage2Data.c

python3 convert_stage.py ChaseHQ-128K-bank-4.skool 3 \
    --obj-names TREE,LIGHT,CONE,BARRIER,SIGN --turn-signs \
    > C/ChaseHQ/Data/ChaseHQ-Stage3Data.c

python3 convert_stage.py ChaseHQ-128K-bank-6.skool 4 \
    --obj-names PALM,LIGHT_RIGHT,LIGHT_LEFT,CONE,BARRIER \
    > C/ChaseHQ/Data/ChaseHQ-Stage4Data.c

python3 convert_stage.py ChaseHQ-128K-bank-7.skool 5 \
    --obj-names TREE,LIGHT_RIGHT,LIGHT_LEFT,CONE,BARRIER \
    > C/ChaseHQ/Data/ChaseHQ-Stage5Data.c
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

The script auto-detects the bank offset (the difference between Z80 absolute addresses stored as `DEFW` and their raw values) from the first annotated pointer in the file. Stages 2–5 live in RAM banks 3/4/6/7 so the offset is `$6400` for all of them.

## Stage 1

Stage 1 data (`ChaseHQ-Stage1Data.c`) is hand-crafted and should not be regenerated with this script.
