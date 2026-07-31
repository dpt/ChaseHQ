# `draw_road` — Internals

This document traces the road render starting from `draw_road` ($C452). For the broader per-frame pipeline (height table, curve table, object pass) see `road-drawing.md`.

## Structure (C port)

The Z80 `draw_road` routine ($C452–$C8BD) is one routine built from self-modified `JP` loops. The C port is a single `draw_road` function that mirrors it: the sections named below (`dr_read_lanes`, `dr_dispatch`, `dr_fill`, `dr_fill_left_stripe`, …) are **labelled blocks inside that one function**, kept in Z80 address order, not separate functions. Read the headings below as labels.

- The **outer segment loop** ($C4AD…$C797) is a `for (;;)`: each iteration renders one road segment, then the height-check either `continue`s to the next segment or `return`s at the horizon.
- The **inner scanline loop** ($C6AD) and the **height-check** ($C6B0–$C79A) stay as `goto` between labels — the Z80 has two scanline entry points (filled/unfilled) that don't nest into a single structured loop without reordering blocks, so source order is preserved and they remain `goto`.
- The two Z80 self-modified operands are modelled as local `enum` selectors: `callback_sel` (`CB_FOUR_LANE` | `CB_DISPATCH`, was the $C4B2 `CALL`) and
  `fill_sel` (`FILL_FILLED` | `FILL_UNFILLED`, was the $C6AD `JP NZ`).
- `draw_road_lanes_change` ($C2E7), `draw_forked_road` ($C8E3) and `dr_start_backdrop_fill` ($C79A) remain **separate helper functions** called from the loop.

---

## Entry: `draw_road` ($C452)

Initialises state fields used throughout the entire descent:

- `dr.edge_thickness = 3` — controls how many scanline pairs each edge/lane thickness level covers before thinning with distance.
- `IYheightptr` — points to `height_table[1]`, the first valid height entry.
- `Ccounter = 96 - height_table[1]` — number of scanlines to draw before the first height-table transition.
- Decodes the `ROADBUF_LANES_OFFSET` pointer parity into `dr.initial_stripe_state` and `carry_stripe`, which selects the starting kerb stripe phase:
  - `carry_stripe` set → thick lane markings (`dr.stripe_table_offset = $D0`, `dr.edge_graphic_offset = 16`), chequerboard verge fill.
  - `carry_stripe` clear → narrow/no markings (`dr.stripe_table_offset = $00`, `dr.edge_graphic_offset = 48`), blank verge fill.
- Sets `callback_sel = CB_FOUR_LANE` (default, overridden by scene-change).
- Enters the segment `for (;;)` loop at the `dr_read_lanes` section.

---

## `dr_read_lanes` ($C4AD)

Decodes the current lanes byte (`*IXlanesptr`) to determine road configuration.

The byte is decoded as follows — bit positions refer to the byte _after_ an SLA (shift left arithmetic):

| Condition | Road type | Next call |
| --- | --- | --- |
| `left_offset is 0` | full-width / no left taper | `goto` per `callback_sel` (`dr_four_lane_highway` or `dr_dispatch`) |
| bit 6 clear, bit 7 clear | 2-lane normal | `draw_road_lanes_change` (helper), then `dr_dispatch` |
| bit 6 clear, bit 7 set | 3-lane normal | `draw_road_lanes_change` (helper), then `dr_dispatch` |
| bit 6 set, bit 7 clear | tunnel (or tunnel transition) | `goto dr_dispatch` |
| bit 6 set, bit 7 set, bit 5 set | forked road | `draw_forked_road` (helper), then `return` |
| bit 6 set, bit 7 set, bit 5 clear | dirt track | `callback_sel = CB_FOUR_LANE`, `goto dr_dispatch` |

For normal roads, `dr_left_table_hi_{1,2}` and `dr_right_table_hi_{1,2}` are set to the `$E8xx`–`$ECxx` xpos table pages, and `dr.neg_lane_count` is set to −2 or −3 (the number of interior lane-dividers to draw).

For tunnels, the fill pattern is forced to `0xFF` and `dr.in_tunnel` is set. Transition flags (`dt.tunnel_visible`, `dt.tunnel_distance`) are written when the entry or exit row is within the current draw range.

---

## `dr_four_lane_highway` ($C534)

Used when the road has no left-edge taper (`left_offset is 0`).

Sets left/right table high bytes to the widest span (`$E8`/`$EC`) and `dr.neg_lane_count = -4` (four interior lines). Sets `callback_sel = CB_DISPATCH` and falls into `dr_dispatch`.

---

## `dr_set_lane_callback` ($C54D) — folded away

The Z80 stored the caller-supplied callback into the $C4B2 `CALL` operand here, then fell through to `dr_dispatch`. In the merged C port that store is the `callback_sel` assignment every caller already makes, so this step is gone: callers set `callback_sel` and `goto dr_dispatch` directly.

---

## `dr_dispatch` ($C551)

Selects the filled or unfilled rendering path based on `Bfill_pattern`:

- **Filled** (`Bfill_pattern != 0`): sets `fill_sel = FILL_FILLED`, falls into `dr_advance_filled`.
- **Unfilled** (`Bfill_pattern is 0`): sets `fill_sel = FILL_UNFILLED`, falls into `dr_advance_unfilled`.

`fill_sel` is read at the bottom of `dr_fill_left_stripe`: the inner scanline loop `goto`s back to the matching advance section, so the same path is followed for every scanline in this segment.

---

## Advance and rollover: `dr_advance_filled` / `dr_advance_unfilled` ($C5A1 / $C55F)

Both functions share the same shape:

1. Decrement the high byte of `DEbackbuf` (moves the backbuffer write pointer one scanline up the screen).
2. If the low nibble of the high byte is now zero, a 16-scanline block boundary has been crossed: call `dr_rollover_filled` / `dr_rollover_unfilled` to adjust the Z80 backbuffer address arithmetic (subtract 32 from the low byte; add 16 to the high byte if no borrow).
3. Otherwise, fall through to `dr_fill` (filled path) or `dr_write_scanline_unfilled` (unfilled path).

---

## `dr_write_scanline_unfilled` ($C565)

Writes a fully zeroed (blank) scanline — used for the section of the road above the horizon where no kerb fill is needed. Redirects out-of-range `DEbackbuf` values (which in the Z80 would land in unintended low memory and silently be discarded) to the last backbuffer row. Falls through to `dr_fill_left_stripe` with `jump_index = 0` (writes 15 zero words = 30 bytes of verge).

---

## `dr_fill` ($C5A7)

The main per-scanline rendering function for filled (kerb-stripe) scanlines.

1. Reads left and right xpos table entries for the current row (`Lrow`) from the `dr_left_table_hi_2` / `dr_right_table_hi_2` pages.
2. Converts each xpos into a stripe width (0–15, in 2-byte units):
   - If xpos byte is non-zero: clamp to 0 (negative = off-screen left) or 15 (positive = off-screen right).
   - If xpos byte is zero: read the previous byte, extract bits [5:3] as a raw width, right-rotate, and clamp at 15.
3. Computes `dr.road_width` = 15 − right_stripe_width + left_stripe_width (the blank tarmac span between the two verges).
4. Rotates `Adash_fill_pattern` left by 1 (alternating the chequerboard phase each scanline).
5. Fills right verge: `memset` of `(15 - dr.right_stripe_width) * 2` bytes with the fill pattern.
6. Fills road surface: `memset` of the tarmac span with zero.
7. Falls through to `dr_fill_left_stripe` to fill the left verge and draw the edge/lane overlays.

---

## `dr_fill_left_stripe` ($C62E)

Fills the left verge, then overlays road edge markings and lane dashes.

1. **Left verge fill** — `memset` of `(15 - jump_index) * 2` bytes with the fill pattern, starting at `SPoutput`.

   Each two-byte edge/lane write below is `INC E` in the Z80: the low byte of the backbuffer column wraps at 0xFF with no carry into the high byte (row). The C port must recompute the second byte's address via `ADDRTOBACKBUF((DEdash_backbuf & 0xFF00) | ((Edash + 1) & 0xFF))`, not a raw pointer increment — a plain `ptr++` runs past the row boundary (and off the end of `backbuffer[]`) whenever the column offset is 0xFF.

2. **Left outer edge** ($C643) — looks up the left xpos (from
   `dr_left_table_hi_1` page). Builds a pointer into `edge_markings[]` using
   `((xpos & 7) << 2) + dr.edge_graphic_offset` as the low byte within the
   `$E4xx`page. Applies an AND-OR mask (two bytes: mask then colour) at the pixel column computed from`(xpos >> 3) & 31`.
3. **Interior lane dashes** ($C667) — loops `dr.neg_lane_count` times
   (−4 to −1), advancing through the `$E8xx`–`$ECxx`xpos table pages. For each non-zero entry, builds a pointer into`edge_markings[]`using`((xpos & 7) << 1) + dr.stripe_table_offset` and writes two bytes without masking (plain tarmac surface, no AND step).
4. **Right outer edge** ($C68A) — mirrors the left edge using `dr_right_table_hi_1` and `dr.right_edge_offset` (= `dr.edge_graphic_offset + 1`, so that the right edge's mask/colour bytes are offset by one slot within the edge marking data).
5. **Inner scanline loop** ($C6AD) — decrements `Ccounter`. If still positive, `goto`s back to the advance section chosen by `fill_sel` (`dr_advance_filled` / `dr_advance_unfilled`) to draw the next scanline. Only when `Ccounter` reaches zero does execution fall into the height-check block below.

---

## Height-check block ($C6B0–$C79A)

Runs once per height-table entry, after Ccounter scanlines have been drawn.

### `dr_set_stripes` ($C6B2)

Toggles stripe phase and edge thickness each time it is reached (each height-table step):

- `dr.initial_stripe_state ^= 1`: every other step flips the kerb phase (`dr.fill_pattern ^= 0x55`, `dr.edge_graphic_offset ^= 0x20`).
- `dr.edge_thickness` counts down from 3 (or 5 when reset). When it hits zero, `dr.stripe_xor_base` advances by `$10` (shifting the edge graphic to a thinner variant) and `dr.edge_thickness` resets to 5. This is how the road markings appear to thin with distance.

### Height comparison ($C703)

Reads `**IYheightptr` (current) and `**(IYheightptr+1)` (next) from the height table. Advances both `IYheightptr` and `IXlanesptr` (with circular wrap). Dispatches on the signed difference:

| Condition | Label | Action |
| --- | --- | --- |
| diff is 0 | `dr_level_road` | Advance `Lrow` by −2; check for tunnel flags; jump to `dr_set_stripes` |
| diff > 0 (S flag clear) | `dr_increasing` | Set `Ccounter = diff`; if diff < 80: `continue` the outer loop (re-enter `dr_read_lanes`); else fall to backdrop |
| diff < 0 (S flag set) | `dr_decreasing` | Check tunnel flags, then `dr_calc_height_delta` |

### `dr_calc_height_delta` ($C774)

When decreasing: computes the combined delta over the next two height entries. If the combined delta is a small negative (−32..−1), adjusts `Lrow` and `goto`s back to `dr_decreasing`. If the combined delta turns positive or is large, sets `Ccounter = delta` and `continue`s the outer loop (re-enters `dr_read_lanes`), or falls to backdrop.

---

## `dr_start_backdrop_fill` ($C79A)

Called when the road has risen to the horizon (large uphill delta or `dr_increasing` with diff ≥ 80).

1. Computes the number of sky rows from the backbuffer pointer nibble and the horizon level, clamped to `dr.sky_rows`.
2. Selects the backdrop source (even/odd `dr.horizon_x_scroll` → different pre-shifted copy) and the horizontal scroll offset via a jump-table cut into a 21-byte LDI chain.
3. Copies `dr.sky_rows` rows of backdrop data to the ZX screen destination.
4. Fills remaining rows above the backdrop with `0x00` (open sky) or `0xFF` (tunnel ceiling).

---

## Summary diagram

Single `draw_road` function; every `dr_*` name is a label in Z80 address order. `(fall)` = fall through to the next label; `[helper]` = a separate function.

```
draw_road:
  <entry setup>   callback_sel = CB_FOUR_LANE
  for (;;) {                                        outer segment loop ($C4AD..$C797)
  dr_read_lanes:
    [left_offset=0]     goto dr_four_lane_highway | dr_dispatch   (per callback_sel)
    [normal 2/3-lane]   draw_road_lanes_change() [helper]; callback_sel=CB_FOUR_LANE; goto dr_dispatch
    [tunnel]            Bfill_pattern = tunnel; goto dr_dispatch
    [fork]              draw_forked_road() [helper]; return
    [dirt]              callback_sel = CB_FOUR_LANE; goto dr_dispatch
  dr_four_lane_highway: callback_sel = CB_DISPATCH; (fall)
  dr_dispatch:          fill_sel = FILL_FILLED ? goto dr_dispatch_filled : (fall)
  dr_advance_unfilled:  [rollover] goto dr_rollover_unfilled; (fall)
  dr_write_scanline_unfilled:  goto dr_fill_left_stripe
  dr_rollover_filled:   goto dr_fill
  dr_rollover_unfilled: goto dr_write_scanline_unfilled
  dr_dispatch_filled:   fill_sel = FILL_FILLED; (fall)
  dr_advance_filled:    [rollover] goto dr_rollover_filled; (fall)
  dr_fill:              (fall)
  dr_fill_left_stripe:
      ... draw one scanline (verges, edges, lane dashes) ...
      if (--Ccounter > 0) goto dr_advance_filled | dr_advance_unfilled   inner loop ($C6AD, per fill_sel)
    dr_set_stripes:     ... toggle stripe phase / thin edges; advance IY/IX; height diff ...
      diff == 0  -> dr_level_road         -> goto dr_set_stripes            (level advance loop)
      diff  > 0  -> dr_increasing         -> continue (next segment) | (fall) dr_backdrop
      diff  < 0  -> dr_decreasing         -> dr_calc_height_delta
                    dr_small_negative     -> goto dr_decreasing (loop) | continue (next segment)
    dr_backdrop:        dr_start_backdrop_fill() [helper]; return          (horizon)
  }
```
