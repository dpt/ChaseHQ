# Z80 → C Translation Pitfalls

Recurring mistakes encountered porting Chase H.Q. from Z80 to C.
Each entry has: the root cause, the symptom, and the fix pattern.

---

## 1. Signed/unsigned type mismatch on local variables

**Root cause:** Z80 registers are 8-bit with no inherent sign; the same byte
is used signed or unsigned depending on context. Defaulting to `u8` is wrong
when subtraction can produce a negative result or when `JP M`/`JP P` branches
on the sign flag.

**Symptoms:**

- `if (X < 0)` on a `u8` is always false — the negative-direction branch is
  dead code. (`L_step`, `A_step`, `A_delta_lo` in `draw_road_scene_change`;
  `Anew_diff` in `dr_fill_left_stripe`.)
- `if (X > 0)` on a `u8` is true for all non-zero values — the negative branch
  is unreachable. (`Aheight_diff > 0` in `dr_fill_left_stripe`, fixed to
  `(s8)Aheight_diff > 0`.)
- `>= 128` boundary miss: `A > 128` fails for `A == 128` (0x80 has Sign flag
  set on Z80). Fix: `A >= 128`.

**Fix:** Declare the variable `s8` when the Z80 treats it as signed. Remove
the now-redundant `(s8)` cast at comparison sites. If the value is set by
subtraction that can underflow, it must be `s8`.

**Commits:** `c8251ba`, `6ce786f`, `4a9eb7f`, `2ab21904`, and this branch tip.

---

## 2. Stale register — wrong variable after register reuse

**Root cause:** Z80 reuses A (and other registers) for different values within
one function. The C translation names them after the register, so the same
name (`A`) can hold different logical values at different points. After a later
use clobbers `A`, an earlier comparison that should reference the old value
uses the new one instead.

**Symptom:** `dr_increasing` checked `if (A < 0x50)` but `A` had been
overwritten by an xpos table read. Because xpos low bytes are 0x48–0x49
(always < 0x50), the backdrop branch never fired and `draw_road` looped
forever.

**Fix:** Give each logical value its own named variable (`Aheight_diff`), not
just the register name, and reference that at all sites where the original
register is still in scope.

**Commit:** `8071d1f`

---

## 3. Missing or wrong initialisation

**Root cause:** The Z80 starts in a known state from ROM/BIOS or from
preceding code. `chq_initialise()` must explicitly set every field that
isn't zero, especially SM (self-modifying instruction) operand fields.

**Symptoms and fixes:**

- **SM fields never set:** `dr_left_table_hi_1/2`, `dr_right_table_hi_1/2`,
  `dr_neg_lane_count` — used on the first frame before `dr_four_lane_highway`
  writes them. Add explicit initialisation in `chq_initialise`. (`c8251ba`)
- **`road_buffer_offset` not reset in `set_up_stage`:** Z80 zeroes this at
  stage start; C translation missed it, leaving a stale pointer into the
  previous stage's buffer. (`dfdaf8b`)
- **`B_iterations` and `C_range` uninitialised in `draw_road_scene_change`:**
  Z80 `LD B,C` sets loop count from C; both must be assigned from
  `C_bresen_range` at the `compute_step` entry point. (`7a12c5b`)
- **`C_bresen_range`/`B_tbl_stride` set from wrong variable:** Used
  `A_curve_step` (animation offset) instead of `IYheight[0] - ref_height`
  (height span). Bresenham range and table stride were wrong in all four
  setup paths. (`f98088b`)
- **`flipped[]` table not built:** `bootstrap()` builds the bit-reversal
  lookup; the `RUN_FULL_GAME=0` path bypasses bootstrap and went straight
  to `chq_initialise` without building the table. All flipped sprites
  rendered black. (`c5c3e6c`)
- **Stage not loaded before attract mode:** `state->stage` was NULL when
  `attract_mode_48k` first accessed it. (`5bd1c47`)

---

## 4. Pointer arithmetic: direction and carry logic

**Root cause:** Z80 PUSH decrements SP; the ZX Spectrum screen layout
means "previous scanline" involves non-obvious byte arithmetic on D:E.
Z80 `SUB $20` subtracts 32; `JR NC` skips the D adjustment on _no_ carry.
C translations routinely get the sign or condition inverted.

**Specific bugs:**

- `dr_rollover_filled/unfilled`: `LO_ADD +32` should be `LO_ADD -32`
  (Z80 `SUB $20`). (`de88b6f`)
- `D += 16` condition inverted: Z80 adds 16 on no-carry (E≥32 before
  subtraction = result < 224); C was adding it on the carry branch. (`de88b6f`)
- `dr_rollover_unfilled`: the `HI_ADD` was missing entirely; the
  `dr_write_scanline_unfilled` call was accidentally inside the bare `if`
  body with no braces. (`de88b6f`)
- Read-before-decrement: Z80 `DEC D; LD A,D` reads D _after_ decrement;
  C read D before. With sentinel D=0x01 this skipped the rollover entirely.
  (`8449d0f`)
- `LO_ADD` macro computed but didn't assign back — was a pure expression
  used as a statement (no-op). (`8449d0f`)

---

## 5. Wrong pointer type → wrong element stride

**Root cause:** Z80 `LD A,(HL)` always loads one byte. If the C pointer is
`u16*`, `ptr[offset]` treats a byte offset as a word index, doubling it and
reading far out of bounds.

**Bug:** `HLdash` in `dr_fill` was `u16*`; `HLdash[Ldash]` treated the byte
offset 255 as a word index, reading element 255 of a 128-element array (into
the adjacent `xpos_road_centre_left` table). Fix: `u8*`.

**Commit:** `6217000`

---

## 6. Accumulate vs. assign — ADD vs. LD

**Root cause:** Z80 `ADD A, IXl; LD IXl, A` accumulates into a register
across iterations. The C translation sometimes uses plain assignment
(`IXl = A`) which discards the running total.

**Bug:** `build_curve_table` IX position was overwritten rather than
accumulated, producing curvature indices of 127 into a 96-entry table.

**Fix:** `IXl = (current_offset + 0x40 + curvature_A) & 0xFF` — keep the
running offset.

**Commit:** `241b3da`

---

## 7. Macro used as expression, not statement

**Root cause:** A macro defined as a pure expression (`((base)[...])`)
returns a value but has no side effect. If callers use it as a statement
the value is discarded and nothing happens.

**Bug:** `WRAPPING(ptr, delta, base)` returned a new pointer but never
assigned it back. All `WRAPPINGINCREMENT` calls in `build_height_table`
were no-ops; `height_table[1..21]` was never filled; `dr_read_lanes`
received a garbage IX pointer → SIGSEGV.

**Fix:** Rewrite as a statement macro that assigns in place:
`((ptr) = &(base)[...])`.

**Commit:** `3619da2`

---

## 8. Borrow detection via bit 7 is unreliable

**Root cause:** The Z80 pattern `LD A,X; SUB Y; JP C` detects borrow (X < Y)
exactly. Translating `JP C` as `if (result & 0x80)` (bit-7 = sign flag)
only works when `X - Y` fits in the −128..+127 range. Differences > 128
wrap and lose the sign flag, silently skipping the branch.

**Bug:** `draw_road_scene_change` used `range == 0 || (range & 0x80)` to
detect that the setup path should be skipped. For differences above 128
the bit-7 check returned false incorrectly.

**Fix:** Use a direct comparison: `if (IYheight[0] <= C_val)` — exact for
all values and more obviously correct.

**Commit:** `0f95209`

---

## 9. Wrong constant / literal

**Root cause:** Z80 constants are written in hex; large hex values can be
misread or mis-transcribed.

**Bug:** `Ccounter` in `draw_road` was initialised to `3` instead of
`$60` (96). The values look nothing alike in decimal but could be confused
if the skool annotation was misread.

**Commit:** `d6b91f9`

---

## 10. Boolean convention inverted

**Root cause:** Some draw functions use the convention 0 = is-flipped,
1 = not-flipped (or vice versa). Using a raw multi-bit flags field where
a boolean 0/1 is expected inverts both branches.

**Bug:** `draw_object_930e_entrypt` passed `flags >> 1` as `zero_flipped`
where the downstream function expected a strict 0-or-1 flag. All draw
paths were taken in the wrong order.

**Fix:** Derive the boolean explicitly: `zero_flipped = (flags & MASK) ? 0 : 1`.

**Commit:** `c5c3e6c`

---

## 11. `>> 8` on a `u8*` is always 0

**Root cause:** A `u8` value promoted to `int` shifted right 8 is zero
for all 8-bit inputs. The shift was intended to read a second byte from
a packed pixel pair.

**Bug:** `plot_sprite_flipped_even`: `*src++ >> 8` always gave 0 for the
second byte of every pair, rendering half the flipped pixels blank.

**Fix:** Two separate `flip_table[*src++]` calls, matching the odd-width
path.

**Commit:** `c5c3e6c`

---

## 12. Wrong array layout — `$E4xx` page

**Root cause:** The Z80 treats a 256-byte memory page as one flat space.
Splitting it into separate arrays with independent origins breaks accesses
that straddle the implicit boundary.

**Bug:** `edge_markings` was declared starting at `$E410` (offset 0x10
into the page), but all access sites subtracted `$E400`. The minimum
computed index was 0x10, and the maximum (with graphic offset 0xB0 and
7 xpos bits) was 0xCC = 204, overrunning the 192-byte array. Lane markings
were a separate array, requiring different index arithmetic.

**Fix:** Declare one 256-byte array representing the full `$E4xx` page
(16 leading zeros + 192 edge bytes + 48 lane bytes). All sites subtract
`$E400` uniformly and index into this single array.

**Commit:** `9998264`
