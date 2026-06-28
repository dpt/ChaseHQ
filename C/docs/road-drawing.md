# Road Drawing Technique

## Overview

The road is drawn bottom-up, scanline by scanline, for the lower 128-pixel playfield. There is no per-frame trigonometry; all perspective is precomputed into tables each frame.

---

## The Road Buffer

`road_buffer[256]` is a circular 256-byte array (accessed via a wrapping offset pointer that advances one slot per frame). It stores a 21-slot history of map data, laid out in six parallel bands of 32 bytes each:

| Offset  | Contents              |
| ------- | --------------------- |
| 0–31    | Curvature             |
| 32–63   | Height                |
| 64–95   | Lane configuration    |
| 96–127  | Right-side object IDs |
| 128–159 | Left-side object IDs  |
| 160–191 | Hazard slots          |

Older slots = further distance. Each slot is one "row" of road as it scrolls toward the player.

---

## Per-Frame Preprocessing

### `build_height_table` → `table_e300[1..21]`

Reads 21 height bytes from the road buffer. Using `fast_counter` to index a row of `vertical_e600` (a 7×22 perspective scale table), it multiplies each height delta by a per-slot scale factor. The result is a 21-entry screen-Y position table — one Y coordinate per distance slot. A clamped copy (running minimum) goes into `table_e336`.

### `build_curve_table` → `table_ec00` (right edge) + `table_e800` (left edge)

Reads 20 curvature bytes from the road buffer. Accumulates them through `inward_bend_table` (a precomputed horizontal deflection curve) weighted by a `horizontal_e6b0` perspective row (again selected by `fast_counter`). Writes two 21-entry tables of 16-bit horizontal positions — one for each road edge — from the far end backwards using a Z80 SP-push trick. A second pass adds a fixed offset to produce the left-edge table. For a forked road the same routine is run twice with inverted curvature to generate both fork positions.

### `scroll_horizon`

Advances `dr_horizon_x_scroll` (0–19 cycles) and updates the vertical incline state, so the backdrop tiles at the correct speed.

---

## `draw_road`

Reads the lanes byte for the current slot to determine road configuration (2-lane, 3-lane, 4-lane, tunnel, dirt track, fork) and sets a chain of callback pointers and self-modifying state. Then iterates from the horizon down, one scanline at a time.

### For each scanline

1. Reads the right-edge and left-edge X positions from the prebuilt tables.
2. Divides the 32-byte scanline into three zones — left verge, road surface, right verge — expressed as widths in 2-byte units (0–15 each).
3. Fills those zones using fall-through `switch` statements emulating the Z80 SP-push trick: each case writes one word (2 bytes) and falls into the next, so entering at `case N` writes exactly `15-N` words. This fills the screen right-to-left in one pass without looping.
   - Verge: filled with an alternating `_X_X_X_X` / `________` pattern that toggles per scanline pair to produce road kerb stripes.
   - Road surface: filled with zero (blank tarmac).
4. Overlays road edge markings and lane dashes by AND-OR masking from small prebuilt bitmaps in the `$E8xx`/`$ECxx` tables. The edge thickness thins with distance via a self-modifying counter (`dr_SM_C6D8`).

### At the horizon transition

- Fills the backdrop using either the raw stage bitmap or the nibble-pre-shifted copy (selected by whether `dr_horizon_x_scroll` is odd or even), with a jump-table cutting into a fixed 21-byte copy loop to apply the scroll offset.
- Fills sky rows above the backdrop with zero (or `$FFFF` all-white in a tunnel).

---

## Object Pass (`draw_everything_else`)

After the road is painted, a second pass over the same 21 distance slots:

- Increments `table_e300` and `table_e336` by 32 each (advancing the frame).
- For each slot, reads the right-side and left-side object IDs from the road buffer and dispatches to a stage-specific handler (tree, bush, lamp, sign, tunnel light, …) with the slot's Y position from `table_e300` and X position from `table_ea00`.
- Also triggers hazard arrows, smoke/dust, tunnel overlay, and helicopter.

---

## Key Design Insight

The Spectrum couldn't afford per-pixel perspective division at draw time. Instead, the game pre-scales everything into Y-position and X-position tables once per frame using integer multiply via bit-shift accumulation (6 conditional adds). The rasteriser then just does table lookups and SP-push fills — no maths at all during the actual scanline rendering.
