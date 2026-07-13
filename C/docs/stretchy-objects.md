# Stretchy Objects — Depth-Scaled Composite Sprites

## The problem: perspective without hardware help

Trees, lamp posts and telegraph poles at the side of the road need to look close when they are close and far away when they are far away. On a real camera this is automatic — perspective just happens. On a ZX Spectrum there is no hardware scaling, so if you want an object to appear taller as it approaches, you have to draw a taller bitmap yourself, frame by frame.

The obvious approach is to store a complete bitmap for every distance the object can be at (near, medium, far, and everything in between). That is expensive: ten depth levels would mean ten full-size bitmaps per object, most of them differing only slightly from their neighbours. Chase H.Q. uses a cheaper trick called a _stretchy object_: instead of one bitmap that scales, an object is built from several small bitmaps stacked on top of one another, and only some of those bitmaps change size with distance.

## The idea: stack fixed and scaled bands

Take the telegraph pole on stage 1 as a running example. It is built from five bands, stacked bottom to top on screen:

```
STRETCHY_TYPE_FIXED   → bottom segment   (depthset selected by current depth)
STRETCHY_TYPE_38PC    → lower-middle     (perspective-scaled to 38% of base)
STRETCHY_TYPE_FIXED   → middle           (constant height regardless of depth)
STRETCHY_TYPE_113PC   → upper-middle     (scaled to 113% at close range)
STRETCHY_TYPE_FIXED   → top segment      (constant height)
STRETCHY_TYPE_END     → terminator
```

The `FIXED` bands (foot, middle collar, pole head) are drawn at the same height no matter how far away the pole is — they act as visual anchors so the pole still looks like a pole close up or far away. The percentage bands (`38PC`, `113PC`) grow and shrink: at close range they stretch, at a distance they compress, and this is what makes the whole pole appear to recede as the road scrolls past. Each individual bitmap is still only drawn once per frame, just at a different height — nothing is drawn pixel by pixel in a loop to fake the stretch.

This is the pattern behind every stretchy object: a run of bands, some fixed, some scaled, stacked to build one composite sprite. We will now look at how that is represented in data, then how it is drawn.

## Data representation

### `stretchy_t` — one band

```c
typedef struct stretchy {
  u8                type;
  const depthset_t *set;
} stretchy_t;
```

An object is an array of these, one entry per band. The `type` byte says how tall the band should be drawn:

| Type | Behaviour |
| --- | --- |
| `STRETCHY_TYPE_FIXED` | Height equals the bitmap's own `height` field. No perspective scaling. |
| `STRETCHY_TYPE_25PC` … `STRETCHY_TYPE_200PC` | Height is a percentage of a base value looked up in `persp_y_scale` (see below). Higher percentages stretch more at close range. |
| `STRETCHY_TYPE_END` | Terminator. Stops the loop; no `set` pointer needed. |

### `depthset_t` and `depthset_pair_t` — picking the right bitmap for the distance

A band does not use one fixed bitmap either — the same telegraph-pole foot looks slightly different up close than it does far away, so each band carries a small table of bitmap variants, one per depth level:

```c
typedef struct depthset {
  const bitmap_t         *bitmaps;            /* array of bitmap variants */
  depthset_pair_t         pairs[DEPTHSET_MAX]; /* maps depths → bitmaps */
} depthset_t;
```

`pairs` maps a depth index (how near or far the object is) to an offset into `bitmaps`. A typical depthset has ten pairs, one per visible depth level, though some have fewer. At draw time the renderer looks up the current depth in `pairs` to decide which bitmap variant of this band to use.

### `bitmap_t` — one drawable bitmap

```c
typedef struct bitmap {
  u8        width_bytes; /* row stride in bytes */
  u8        flags;       /* BITMAPFLAG_MASKED, BITMAPFLAG_FLIPPED */
  u8        height;      /* pixel rows in this bitmap */
  const u8 *data;        /* byte-aligned variant */
  const u8 *shifted;     /* variant pre-shifted for sub-byte placement */
} bitmap_t;
```

`height` is what a `FIXED` band uses as its draw height. `flags` controls two independent things: whether the bitmap is masked (drawn with an AND mask so the road shows through the transparent parts) and whether it is flipped for use on the opposite side of the road — a flipped band uses an entirely separate `bitmap_t` array built with mirrored graphics, selected via the band's `depthset_t`, not via the `shifted` field.

`data` and `shifted` solve a different problem: the ZX Spectrum's screen memory is addressed one byte per eight pixels, so a sprite can only be blitted directly at byte-aligned x-positions. To place it at any other pixel column, the game keeps a second copy of the same bitmap pre-shifted by a few pixels, and picks whichever copy lines up with the object's actual screen position. `doc_shift_select`, derived from the object's x-coordinate in `draw_object_perspective_entrypt()` (`$930E`), selects `data` or `shifted` for exactly this reason — narrow bitmaps (where sub-byte error is most visible) tend to have genuinely different `data`/`shifted` arrays, while wider ones often reuse the same pointer for both.

## Drawing pipeline

The entry points are `draw_stretchy_object_left()` and `draw_stretchy_object_right()` (`$916C` and `$9171`). Both simply hand off to `draw_stretchy_object_common()` (`$9174`), which loops over the `stretchy_t[]` array band by band:

1. Read the `type` byte. If it is `STRETCHY_TYPE_END`, stop.
2. Look up the current depth in the band's `depthset_t` to pick which bitmap variant to draw.
3. Work out the draw height:
   - `STRETCHY_TYPE_FIXED` — use the bitmap's own `height` field unchanged.
   - A percentage type — look up the base height in `persp_y_scale` for the current frame and depth, then scale it by the band's percentage.
4. Work out where this band sits horizontally, using the running total of all preceding bands' widths (see _Stacking bands_, below).
5. Call the drawing routine for this band (see _Callback dispatch_, below).

### The `persp_y_scale` table

```c
const u8 persp_y_scale[8][22];
```

This is the shared perspective table: eight animation frames (the road's scroll cycle) by 22 depth levels, giving a base height in pixels for each combination. Percentage-type bands multiply this base value by their own factor (25% through 200%). `FIXED` bands ignore the table completely — that is the whole point of being fixed.

### Stacking bands

Each band tracks a running column total, `C_total`, which is the combined width of all bands drawn so far in this object. That value is negated into `doc_col_pos` to clip the band's left edge against the screen. Because the next band starts exactly where the last one's bitmap ended, the bands stack with no gap between them, forming one continuous composite sprite.

## Callback dispatch

Two function-pointer fields drive the actual pixel copying: `SM_91CD_callback` and `SM_9244_callback`. `FIXED` bands call `SM_91CD_callback`; percentage bands call `SM_9244_callback`. Keeping these behind function pointers means the loop in `draw_stretchy_object_common()` does not need to know how a band is actually rendered — it just calls whichever callback the band's type selects.

- **`SM_91CD_callback`** (fixed bands) leads to `plot_sprite()` or `plot_sprite_flipped()`, which copy bitmap rows from source to back-buffer one row at a time, working backwards through the buffer as they go. Masked bitmaps are combined with an AND mask; which of `data` or `shifted` is read is decided by `doc_shift_select`, based on the object's horizontal sub-byte position.

- **`SM_9244_callback`** (percentage bands, `doc_plot_mode == 2`) does a two-phase draw: it draws fixed-width top and bottom edges of the band first, then repeats the middle row of the bitmap down through the required scaled height, wrapping its source pointer back to the start of the middle bitmap after each pass. This is how a single small bitmap can fill a band whose height changes every frame.

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

The two pointers are self-modified fields in `chqstate_t` — on the Z80 this is code patching its own dispatch target rather than an indirect call through a stored pointer, which is why they carry the `SM_` prefix used throughout this codebase for self-modified addresses.

## Clipping

A band is skipped entirely if it falls outside the visible screen area: below row 191, above the top of the playfield, or beyond the road's left or right edge. The available vertical space for the object at its current screen position is `IYheight[53 + offset] - IYheight[offset]`; if that is less than one pixel, the band exits immediately without drawing anything.

For percentage bands, the scaled height calculated above can be larger than the space actually available on screen. When that happens only the top portion of the bitmap is drawn and the rest is clipped away — the band never draws past the edge of its allotted screen space.

## Why build objects this way

Building one composite sprite out of several small bitmaps rather than storing a full-size bitmap per depth level saves memory: five bands of around 32 bytes each is far cheaper than one 128-byte bitmap repeated for every depth level. The fixed bands give each object a recognisable shape at every distance, while the percentage bands carry the actual perspective effect. Depth selection is pushed down into the `depthset_pair_t` mapping, so the drawing loop itself never needs to know anything about how many depth variants a particular band has — it just looks the current depth up in the table.

## Relationship to road drawing

The road surface uses a different technique for the same illusion: horizontal scanlines whose perspective scale is also driven by `persp_y_scale`, computed once per frame in `build_height_table` and `scroll_horizon`. Road drawing repeats pixel rows to simulate a receding surface; stretchy objects instead stack several distinct bitmap bands, each height calculated once at the start of the draw call and then held constant while its pixels are copied into the back buffer. Both techniques share the same perspective table and solve the same underlying problem — faking depth on a machine with no scaling hardware — but they solve it in different ways.

## Further reading

- `road-drawing.md` — road surface drawing technique and perspective pipeline.
- `draw-road-internals.md` — call chain for `draw_road` including lane markings, edge graphics and object pass entry points.
- `Stages.h` — type definitions (`stretchy_t`, `depthset_t`, `bitmap_t`).
- `ChaseHQ.c` — implementation of `draw_stretchy_object_common()` and the callback dispatch logic.
