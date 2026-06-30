# `draw_road` — Call Chain and Internals

This document traces the call chain starting from `draw_road` ($C470). For the broader per-frame pipeline (height table, curve table, object pass) see `road-drawing.md`.

---

## Entry: `draw_road` ($C470)

Initialises state fields used throughout the entire descent:

- `dr_edge_thickness = 3` — controls how many scanline pairs each edge/lane thickness level covers before thinning with distance.
- `IYheightptr` — points to `height_table[1]`, the first valid height entry.
- `Ccounter = 96 - height_table[1]` — number of scanlines to draw before the first height-table transition.
- Decodes the `ROADBUF_LANES_OFFSET` pointer parity into `dr_initial_stripe_state` and `carry_stripe`, which selects the starting kerb stripe phase:
  - `carry_stripe` set → thick lane markings (`dr_stripe_table_offset = $D0`, `dr_edge_graphic_offset = 16`), chequerboard verge fill.
  - `carry_stripe` clear → narrow/no markings (`dr_stripe_table_offset = $00`, `dr_edge_graphic_offset = 48`), blank verge fill.
- Sets `dr_callback = dr_four_lane_highway` (default, overridden by scene-change).
- Falls through to `dr_read_lanes`.

---

## `dr_read_lanes` ($C4AD)

Decodes the current lanes byte (`*IXlanesptr`) to determine road configuration.

The byte is decoded as follows — bit positions refer to the byte _after_ an SLA (shift left arithmetic):

| Condition | Road type | Next call |
| --- | --- | --- |
| `left_offset is 0` | full-width / no left taper | `state->dr_callback` (usually `dr_four_lane_highway`) |
| bit 6 clear, bit 7 clear | 2-lane normal | `draw_road_lanes_change` |
| bit 6 clear, bit 7 set | 3-lane normal | `draw_road_lanes_change` |
| bit 6 set, bit 7 clear | tunnel (or tunnel transition) | `dr_dispatch` |
| bit 6 set, bit 7 set, bit 5 set | forked road | `forked_road_plotter` |
| bit 6 set, bit 7 set, bit 5 clear | dirt track | `dr_set_lane_callback` |

For normal roads, `dr_left_table_hi_{1,2}` and `dr_right_table_hi_{1,2}` are set to the `$E8xx`–`$ECxx` xpos table pages, and `dr_neg_lane_count` is set to −2 or −3 (the number of interior lane-dividers to draw).

For tunnels, the fill pattern is forced to `0xFF` and `dr_in_tunnel` is set. Transition flags (`dt_tunnel_visible`, `dt_tunnel_distance`) are written when the entry or exit row is within the current draw range.

---

## `dr_four_lane_highway` ($C534)

Used when the road has no left-edge taper (`left_offset is 0`).

Sets left/right table high bytes to the widest span (`$E8`/`$EC`) and `dr_neg_lane_count = -4` (four interior lines). Then falls through to `dr_set_lane_callback`.

---

## `dr_set_lane_callback` ($C54D)

Stores the caller-supplied callback in `state->dr_callback` (so that `dr_fill_left_stripe` can recurse back to the correct road-type handler when Ccounter expires). Falls through to `dr_dispatch`.

---

## `dr_dispatch` ($C551)

Selects the filled or unfilled rendering path based on `Bfill_pattern`:

- **Filled** (`Bfill_pattern != 0`): sets `dr_fill_fn = dr_advance_filled`, falls through to `dr_advance_filled`.
- **Unfilled** (`Bfill_pattern is 0`): sets `dr_fill_fn = dr_advance_unfilled`, falls through to `dr_advance_unfilled`.

The `dr_fill_fn` function pointer is used by `dr_fill_left_stripe` at the top of each recursive call, so the same path is followed for every scanline in this segment.

---

## Advance and rollover: `dr_advance_filled` / `dr_advance_unfilled` ($C5A1 / $C55F)

Both functions share the same shape:

1. Decrement the high byte of `DEbackbuf` (moves the backbuffer write pointer one scanline up the screen).
2. If the low nibble of the high byte is now zero, a 16-scanline block boundary has been crossed: call `dr_rollover_filled` / `dr_rollover_unfilled` to adjust the Z80 backbuffer address arithmetic (subtract 32 from the low byte; add 16 to the high byte if no borrow).
3. Otherwise, fall through to `dr_fill` (filled path) or `dr_write_scanline_unfilled` (unfilled path).

---

## `dr_write_scanline_unfilled` ($C565)

Writes a fully zeroed (blank) scanline — used for the section of the road above the horizon where no kerb fill is needed. Redirects out-of-range `DEbackbuf` values (which in the Z80 would land in ROM and silently be discarded) to the last backbuffer row. Falls through to `dr_fill_left_stripe` with `jump_index = 0` (writes 15 zero words = 30 bytes of verge).

---

## `dr_fill` ($C5A7)

The main per-scanline rendering function for filled (kerb-stripe) scanlines.

1. Reads left and right xpos table entries for the current row (`Lrow`) from the `dr_left_table_hi_2` / `dr_right_table_hi_2` pages.
2. Converts each xpos into a stripe width (0–15, in 2-byte units):
   - If xpos byte is non-zero: clamp to 0 (negative = off-screen left) or 15 (positive = off-screen right).
   - If xpos byte is zero: read the previous byte, extract bits [5:3] as a raw width, right-rotate, and clamp at 15.
3. Computes `dr_road_width` = 15 − right_stripe_width + left_stripe_width (the blank tarmac span between the two verges).
4. Rotates `Adash_fill_pattern` left by 1 (alternating the chequerboard phase each scanline).
5. Fills right verge: `memset` of `(15 - dr_right_stripe_width) * 2` bytes with the fill pattern.
6. Fills road surface: `memset` of the tarmac span with zero.
7. Falls through to `dr_fill_left_stripe` to fill the left verge and draw the edge/lane overlays.

---

## `dr_fill_left_stripe` ($C62E)

Fills the left verge, then overlays road edge markings and lane dashes.

1. **Left verge fill** — `memset` of `(15 - jump_index) * 2` bytes with the fill pattern, starting at `SPoutput`.
2. **Left outer edge** ($C643) — looks up the left xpos (from
   `dr_left_table_hi_1` page). Builds a pointer into `edge_markings[]` using
   `((xpos & 7) << 2) + dr_edge_graphic_offset` as the low byte within the
   `$E4xx`page. Applies an AND-OR mask (two bytes: mask then colour) at the pixel column computed from`(xpos >> 3) & 31`.
3. **Interior lane dashes** ($C667) — loops `dr_neg_lane_count` times
   (−4 to −1), advancing through the `$E8xx`–`$ECxx`xpos table pages. For each non-zero entry, builds a pointer into`edge_markings[]`using`((xpos & 7) << 1) + dr_stripe_table_offset` and writes two bytes without masking (plain tarmac surface, no AND step).
4. **Right outer edge** ($C68A) — mirrors the left edge using `dr_right_table_hi_1` and `dr_right_edge_offset` (= `dr_edge_graphic_offset + 1`, so that the right edge's mask/colour bytes are offset by one slot within the edge marking data).
5. **Loop / recurse** — decrements `Ccounter`. If still positive, calls `state->dr_fill_fn` to advance the backbuffer and draw the next scanline. Only when `Ccounter` reaches zero does execution fall into the height-check block below.

---

## Height-check block ($C6B0–$C79A)

Runs once per height-table entry, after Ccounter scanlines have been drawn.

### `dr_set_stripes` ($C6B2)

Toggles stripe phase and edge thickness each time it is reached (each height-table step):

- `dr_initial_stripe_state ^= 1`: every other step flips the kerb phase (`dr_fill_pattern ^= 0x55`, `dr_edge_graphic_offset ^= 0x20`).
- `dr_edge_thickness` counts down from 3 (or 5 when reset). When it hits zero, `dr_stripe_xor_base` advances by `$10` (shifting the edge graphic to a thinner variant) and `dr_edge_thickness` resets to 5. This is how the road markings appear to thin with distance.

### Height comparison ($C703)

Reads `**IYheightptr` (current) and `**(IYheightptr+1)` (next) from the height table. Advances both `IYheightptr` and `IXlanesptr` (with circular wrap). Dispatches on the signed difference:

| Condition | Label | Action |
| --- | --- | --- |
| diff is 0 | `dr_level_road` | Advance `Lrow` by −2; check for tunnel flags; jump to `dr_set_stripes` |
| diff > 0 (S flag clear) | `dr_increasing` | Set `Ccounter = diff`; if diff < 80: recurse via `dr_read_lanes`; else fall to backdrop |
| diff < 0 (S flag set) | `dr_decreasing` | Check tunnel flags, then `dr_calc_height_delta` |

### `dr_calc_height_delta` ($C774)

When decreasing: computes the combined delta over the next two height entries. If the combined delta is a small negative (−32..−1), adjusts `Lrow` and loops back to `dr_decreasing`. If the combined delta turns positive or is large, sets `Ccounter = delta` and recurses via `dr_read_lanes` or falls to backdrop.

---

## `dr_start_backdrop_fill` ($C79A)

Called when the road has risen to the horizon (large uphill delta or `dr_increasing` with diff ≥ 80).

1. Computes the number of sky rows from the backbuffer pointer nibble and the horizon level, clamped to `dr_sky_rows`.
2. Selects the backdrop source (even/odd `dr_horizon_x_scroll` → different pre-shifted copy) and the horizontal scroll offset via a jump-table cut into a 21-byte LDI chain.
3. Copies `dr_sky_rows` rows of backdrop data to the ZX screen destination.
4. Fills remaining rows above the backdrop with `0x00` (open sky) or `0xFF` (tunnel ceiling).

---

## Summary diagram

```
draw_road
 └─ dr_read_lanes
     ├─ [left_offset=0] → dr_four_lane_highway → dr_set_lane_callback
     ├─ [normal 2/3-lane] → draw_road_lanes_change → dr_set_lane_callback
     ├─ [tunnel]          → dr_dispatch
     ├─ [fork]            → forked_road_plotter
     └─ [dirt]            → dr_set_lane_callback

dr_set_lane_callback → dr_dispatch

dr_dispatch
 ├─ [fill_pattern != 0] → dr_dispatch_filled → dr_advance_filled ─┐
 └─ [fill_pattern == 0] → dr_advance_unfilled ─────────────────────┤

dr_advance_filled                                                   │
 ├─ [rollover] → dr_rollover_filled → dr_fill                      │
 └─ [normal]   → dr_fill                                           │
                  └─ dr_fill_left_stripe ◄─────────────────────────┘
                      └─ [Ccounter > 0] recurse via dr_fill_fn
                         [Ccounter = 0] → dr_set_stripes
                                           └─ dr_level_road / dr_increasing / dr_decreasing
                                               └─ dr_read_lanes  (next segment)
                                               └─ dr_start_backdrop_fill  (horizon)

dr_advance_unfilled
 ├─ [rollover] → dr_rollover_unfilled → dr_write_scanline_unfilled
 └─ [normal]   → dr_write_scanline_unfilled
                  └─ dr_fill_left_stripe  (as above)
```
