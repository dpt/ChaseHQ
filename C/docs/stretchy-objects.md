# Stretchy Objects — Depth-Scaled Composite Sprites

This document describes how Chase H.Q. draws trees, lamp posts, telegraph poles and other scenery using a technique called _stretchy objects_: composite bitmaps made of multiple segments, each drawn at a perspective-dependent height so that the whole sprite appears to stretch or shrink with distance.

## Overview

Road-side objects in Chase H.Q. are not single bitmaps. Each one is an array of `stretchy_t` entries, and each entry represents one horizontal band of the final sprite. The segments are stacked from bottom to top on screen:

- A fixed-height base segment (the tree trunk or pole foot).
- One or more perspective-scaled middle segments (foliage bands that compress at distance).
- A fixed-height top segment (the canopy or lamp head).
- A terminator marking the end of the array.

When the renderer iterates over the array, it calculates a vertical scale for each non-FIXED entry from the `persp_y_scale` table and draws that segment at its scaled height. Segments drawn in sequence stack to form the full object.

The key design decision is that each segment's bitmap is **drawn once** at its calculated height — not repeated pixel-by-pixel. The "stretching" effect comes from varying how tall each segment's bitmap appears, combined with stacking multiple distinct segments to create one taller composite graphic. If a bitmap is shorter than the target height, the drawing code clips and draws only what fits.

## Data Representation

### `stretchy_t`

```c
typedef struct stretchy {
  u8                type;
  const depthset_t *set;
} stretchy_t;
```

Each entry has a `type` byte that determines how its height is computed, and a pointer to a `depthset_t` (see below). The type field uses the following values:

| Type | Behaviour |
| --- | --- |
| `STRETCHY_TYPE_FIXED` | Height equals the bitmap's own `height` field. No perspective scaling applied. Used for top and bottom bands that must maintain a constant visual weight at all distances. |
| `STRETCHY_TYPE_25PC` through `STRETCHY_TYPE_200PC` | Height is a percentage of the base value from `persp_y_scale`, with the exact percentage determined by the type byte. A larger percentage (e.g. 200%) means the segment stretches more at close range, creating the foreshortening effect. |
| `STRETCHY_TYPE_END` | Terminator; stops the loop. No `set` pointer is required. |

All percentage values are relative to a base scale read from `persp_y_scale` — see _Drawing Pipeline_ below for details.

### `depthset_t` and `depthset_pair_t`

Each stretchy entry's `depthset_t` describes the bitmap data at that position in the sprite:

```c
typedef struct depthset {
  const bitmap_t         *bitmaps;            // array of bitmap variants
  depthset_pair_t         pairs[DEPTHSET_MAX]; // maps depths → bitmaps
} depthset_t;
```

The `pairs` array maps a depth index (which object is nearest, next-nearest, etc.) to an offset into the `bitmaps` array. The renderer uses the current depth to select which bitmap to draw. A typical depthset has 10 pairs (one per visible depth level), though some have fewer.

The `bitmap_t` struct describes one horizontal band:

```c
typedef struct bitmap {
  u8        width_bytes;    // row stride in bytes
  u8        flags;          // BITMAPFLAG_MASKED, BITMAPFLAG_FLIPPED
  u8        height;         // pixel rows in this bitmap
  const u8 *data;           // normal variant
  const u8 *shifted;        // mirrored variant (same data if not flipped)
} bitmap_t;
```

The `width_bytes` and `height` fields double as the bitmap's own height reference, used by fixed-type entries as the draw height. The `flags` field controls whether the bitmap is masked (semi-transparent) or horizontally flipped. Masked bitmaps use an AND mask against the road background; flipped bitmaps are the mirrored version for the opposite side of the road.

### Example: a telegraph pole on stage 1

The left-hand telegraph pole uses five entries:

```
STRETCHY_TYPE_FIXED   → bottom segment   (depthset selected by current depth)
STRETCHY_TYPE_38PC    → lower-middle     (perspective-scaled to 38% of base)
STRETCHY_TYPE_FIXED   → middle           (constant height regardless of depth)
STRETCHY_TYPE_113PC   → upper-middle     (scaled to 113% at close range, compressing further back)
STRETCHY_TYPE_FIXED   → top segment      (constant height)
STRETCHY_TYPE_END     → terminator
```

At close range the `38PC` and `113PC` segments produce taller bands. At distance they shrink, making the pole appear to recede. The fixed entries provide visual anchors that stay recognisable regardless of position.

## Drawing Pipeline

The entry point for stretchy objects is `draw_stretchy_object_left()` or `draw_stretchy_object_right()`, which delegate to `draw_stretchy_object_common()`. The common function runs a loop over the `stretchy_t[]` array:

1. **Read the type byte.** If it equals `STRETCHY_TYPE_END`, exit the loop.
2. **Select the depthset** from the entry's `set` field and choose the bitmap variant for the current depth index.
3. **Dispatch based on type:**
   - `STRETCHY_TYPE_FIXED`: falls through to the scaling ladder with no modification — the height stays at the bitmap's own value.
   - A percentage type enters a switch/case that shifts or accumulates from the base scale. The scaled result becomes the draw height for this segment.
4. **Calculate the column offset** (the cumulative width of preceding segments) and use it to position this band horizontally relative to the others.
5. **Invoke the callback.** Fixed entries call `SM_91CD_callback` to draw their bitmap at full height; percentage entries call `SM_9244_callback` which combines the scaled height with an additional column-pass routine to produce the final composite row.

### `persp_y_scale` lookup

The base scale for each segment comes from a 2D table:

```c
const u8 persp_y_scale[8][22];
```

Eight animation frames (for the scrolling road's perspective cycle) × 22 depth levels. For a given frame and depth, the entry gives a base height in pixels. Percentage entries multiply this by their factor (25% through 200%). Fixed entries ignore the table entirely.

### Column offset and stacking

Each segment carries a running column total (`C_total`), which represents the cumulative width of all preceding segments. This value is negated into `doc_col_pos` to clip each band's left edge against the screen. The last byte of the previous segment's bitmap determines where the next one starts, creating a tight stack with no gap between bands.

## Callback Mechanism

The `SM_91CD_callback` and `SM_9244_callback` pointers are stored as self-modified fields in `chqstate_t`. They allow the main loop to dispatch through function pointers without hard-coding draw logic, keeping the stretchy loop generic while each segment's callback provides its own specialised rendering.

For standard sprites (`doc_plot_mode == 0`), the callback chain goes through `plot_sprite()` or `plot_sprite_flipped()`, which copy bitmap rows from source to back-buffer one row at a time, advancing backward through the buffer for each scanline. Masked bitmaps use an AND mask; flipped ones read from the `shifted` pointer instead of `data`.

For column-mode drawing (`doc_plot_mode == 2`, used by percentage entries), the callback sets up a two-phase render: the first phase draws fixed-width top and bottom bands, then the middle section repeats its bitmap row across the required height. The repeat loop advances through the back buffer row-by-row, wrapping the source pointer back to the start of the middle bitmap after each pass.

## Clipping

If any part of a segment lies outside the visible screen area (below y = 191 or above the top edge, or beyond the left/right road boundaries), that segment is skipped entirely. The height calculation `IYheight[53 + offset] - IYheight[offset]` gives the available vertical space on screen for the object at that position; if it is less than one pixel the segment exits immediately. For percentage entries the scaled height may exceed the available space, in which case only the top portion of the bitmap is drawn and the rest is clipped.

## Key Design Patterns

**Composite over single.** Each object is an array rather than one bitmap. This avoids storing dozens of full-size bitmaps for every depth level — instead each segment's depthset holds ten variants, and they stack into a composite sprite at render time. The Z80 memory savings are significant: five segments of roughly 32 bytes each beat one 128-byte bitmap comfortably.

**Fixed anchors + variable fill.** Fixed-type segments provide visual constants (the trunk base, the lamp head) that stay recognisable at all depths. Percentage segments fill in between with perspective-accurate scaling. The result is a sprite whose proportions change with distance without losing its identifying features.

**Depth-driven selection.** Each depth level maps to one `depthset_pair_t`, which selects a bitmap from the segment's `bitmaps` array. Different depths produce visually different sprites (taller at close range, shorter and stubbier further away) without the code needing to know anything about sprite layout — it just follows the mapping table.

**Self-modified dispatch.** The two callback pointers live in state fields that both the main loop and individual callbacks can read or write. This lets a segment override the drawing method for its own band without breaking the generic iteration pattern, while keeping the function call overhead low on Z80.

## Call Chain Summary

```
draw_stretchy_object_left()          [$916C]
draw_stretchy_object_right()         [$9171]
   └─> draw_stretchy_object_common()  [$9174]
         ├─> SM_91CD_callback       (for STRETCHY_TYPE_FIXED entries)
         │    └─> plot_sprite() / plot_masked_sprite()
         │
         └─> SM_9244_callback       (for percentage-type entries)
              └─> plot_sprite() [mode 2: column repeat]
```

## Relationship to Road Drawing

The road itself uses a completely different stretching technique: horizontal scanlines whose perspective scale is driven by `persp_y_scale` and computed in the per-frame preprocessing pass (`build_height_table`, `scroll_horizon`). The stretchy object system operates orthogonally — each segment's height is set once at the start of the draw call and then held constant while the sprite data is copied row by row into the back buffer.

Where road drawing repeats pixel rows to simulate a receding surface, stretchy objects stack multiple distinct bitmap segments (each representing one visual band) to form one tall sprite whose proportions change with depth. Both approaches share the same perspective table and both produce the illusion of distance on a machine with no hardware scaling support.

## Further Reading

- `road-drawing.md` — road surface drawing technique and perspective pipeline.
- `draw-road-internals.md` — call chain for `draw_road` including lane markings, edge graphics and object pass entry points.
- `ChaseHQ-Stages.h` — type definitions (`stretchy_t`, `depthset_t`, `bitmap_t`).
- `ChaseHQ.c` — implementation of `draw_stretchy_object_common()` and the callback dispatch logic.
