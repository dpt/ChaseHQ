# Translation Analysis: build_height_table

## Function Overview

`build_height_table` (`$CD3A`, in `Main.c`) builds the per-frame perspective
height lookup table used by `draw_road`. It reads height bytes from the road
buffer, scales each one by a perspective factor for the current distance slot,
and writes the results into `height_table[1..21]`. It also derives
`clamped_heights[]` (a running-minimum clamp of the same 21 entries) and updates
`horizon_attr[0..1]` for the sky/ground boundary scroll (see CLAUDE.md's
"$E34B–$E34D horizon attribute scroll" section).

## Key Components

### 1. Variable declarations

Locals follow the project's `RegisterName_description` convention (see
CLAUDE.md), one per line, ordered by first use:

- `IY_roadbuf` (was IY) — road buffer height-channel pointer
- `C_heightbyte` (was C) — height byte read on entry; input to the initial scale
  call
- `A_counter` (was A/B) — `fast_counter & 0xE0`; perspective row selector and
  scale-call argument
- `HL_pvtab` (was HL) — pointer into the perspective Y-scale table
  `persp_y_scale` (Z80 `$E6xx`)
- `C_min` (was C) — incline accumulator in phase 1; becomes the running minimum
  in phase 2
- `Bdash_iters` (was B') — phase 1 loop counter, 21 iterations
- `DE_phtab` (was DE') — pointer walking `height_table[1..21]`
- `DE_v` (was DE) — perspective scale entry × 2; input to the per-slot
  shift-multiply
- `A_height` (was A) — scaled pixel-row height written to `height_table`
- `HL_dst` (was HL) — walks `clamped_heights[0..20]` then `horizon_attr[0..1]`
  in phases 2–3
- `DE_src` (was DE) — source pointer walking `height_table` in phase 2
- `B_iters` (was B) — phase 2 loop counter, 21 iterations

### 2. Algorithm steps

#### Phase 1: height table construction (`bht_loop`, $CD63–$CDB6)

1. Read the initial height byte from the road buffer and call
   `scale_curvature_or_height(A_counter, C_heightbyte)` once to seed `C_min`.
2. Loop 21 times (`Bdash_iters`), each iteration:
   - Read the next perspective scale entry (`*HL_pvtab`) and double it into
     `DE_v`.
   - Accumulate the next road-buffer height byte into `C_min`.
   - If non-zero, negate as needed and apply an inline shift-multiply —
     `A_height = ((A_height & 0x7F) * DE_v) >> 7` — the direct C equivalent of
     the Z80 shift-and-add sequence at `$CD84-$CDA9`.
   - Add the perspective baseline (`*HL_pvtab`) and store into `height_table`.
3. Write the `0xA0` sentinel one past the last entry.

#### Phase 2: clamping (`bht_loop2`, $CDB7–$CDCA)

Copies `height_table[1..21]` into `clamped_heights[0..20]`, tracking a running
minimum seeded at 96.

#### Phase 3: horizon delta ($CDCB onward)

Rounds the running minimum to a multiple of 8 and writes it to
`horizon_attr[0]`; the difference from the previous frame's value goes to
`horizon_attr[1]`.

### 3. `scale_curvature_or_height` ($CDD6)

A separate helper, used here only for the phase-1 seed value (not in the
per-slot loop, which now does its shift-multiply inline). It replicates the
Z80's hardware-free multiply: three iterations of `RL E` / conditional `ADD A,C`
/ `ADD A,A` extract the top three bits of the multiplier and accumulate their
contribution, followed by four rounding right shifts. Only the top three bits of
the multiplier are examined, which is sufficient because callers always pass a
multiple of `$20`.

### 4. Addressing and memory operations

- Uses `ROADBUF_FWD2PTR(ROADBUF_HEIGHT_OFFSET)` to access the road buffer.
- Uses `WRAP_INCREMENT_ASSIGN` for the road buffer's circular wrap.
- Uses `FAST_COUNTER_PERSP_ROW` to map `fast_counter` to a `persp_y_scale` row.

### 5. SM field usage

`fast_counter` is an SM field modified elsewhere at runtime; this function only
reads it. `height_table` and `clamped_heights` are plain data arrays, not
self-modified instructions.

## Correctness assessment

The translation matches the Z80 shift-and-add multiply exactly (verified against
the disassembly at `$CD84-$CDA9`), uses `s8`/`u8` correctly for the signed
height-delta arithmetic, and all existing tests pass.
