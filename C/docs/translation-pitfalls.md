# Z80 → C Translation Pitfalls

Recurring mistakes encountered porting Chase H.Q. from Z80 to C. Each entry
gives the root cause, the bugs it caused, and the rule that prevents it. Numbers
are stable — CLAUDE.md and the tests cite them — so merged entries keep their
heading as a pointer.

---

## 1. Signedness — `u8` where the Z80 means signed, and the casts that fix it

**Root cause:** Z80 registers carry no inherent sign; the same byte is signed or
unsigned by context. `u8` is wrong wherever a subtraction can go negative or
`JP M`/`JP P` branches on the Sign flag. Three shapes, all the same mistake at a
different width:

- **Local declared `u8`.** `if (X < 0)` is then always false and the negative
  branch is dead code (`L_step`, `A_step`, `A_delta_lo` in
  `draw_road_lanes_change`; `Anew_diff` in `dr_fill_left_stripe`). `if (X > 0)`
  is true for every non-zero value (`Aheight_diff` in `dr_fill_left_stripe`,
  fixed to `(s8)Aheight_diff > 0`). `A > 128` also misses `A == 128`, which has
  the Sign flag set — use `>= 128`.
- **Signed field used in arithmetic (was 22).** A field holding a signed offset
  may legitimately be declared `u8` because it stores 0–255, but `ADD A,D` is a
  signed addition. `draw_object_clipped`'s `D_col_pos` (248 meaning −8) computed
  `Adash_y_range += 248`, cascading into a wrapped `Adash_clip_rows`, an early
  exit and an assert inside `plot_sprite_even`. Fix: `+= (s8)D_col_pos`, or
  declare the field `s8`. The bit-7 conditional idiom is error-prone; the cast
  is always correct. (`944371a`)
- **`u16` field loaded into a wider type (was 28).** The store wraps correctly
  mod 65536, the load loses the sign: −6 arrives as 65530. During a fork,
  `layout_road` sets `road_pos = road_pos ± fork_distance`, which legitimately
  goes negative once `fork_distance` (+16 per spawning frame) exceeds it;
  `build_curve_table` reloaded ~65530 and the geometry blew up — assert in
  `build_curve_table_fill` in debug, garbage road or a seize in release. This
  was the long-standing "forks seize the game up" bug. Contributing cause: the
  fork-distance delta at `$BAD0–$BADE` dropped the `AND $0F` at `$BAD7`, so the
  per-frame delta was −256..−1 instead of −16..−1. Fix: `(s16)` at all three
  `road_pos` load sites. (`f47f822`)

Width matters when casting: the Sign flag after `SBC HL,DE` reflects bit 15, so
an `s8` cast of the `u16` result tests the low byte only — `0xFF64` is negative
as `s16` but has bit 7 clear. (`f21a880`)

**Rule:** Declare the variable `s8`/`s16` when the Z80 treats it as signed, and
drop the redundant cast at comparison sites. Where the field must stay unsigned,
cast at every use. When adding a cast to a shared field, grep for its other
readers — one unsigned load re-introduces the bug.

**Commits:** `c8251ba`, `6ce786f`, `4a9eb7f`, `2ab21904`, `f21a880`, `944371a`,
`f47f822`

---

## 2. Stale register — wrong variable after reuse or an old value swapped in

**Root cause:** Z80 reuses A (and others) for different logical values within
one function, and the C port names locals after the register, so one name
silently means different things at different points. Two shapes: a later
instruction clobbers the register before the comparison that wanted the old
value, and `LD A,reg` deliberately swaps an _older_ value back in right before a
computation.

**Bugs:**

- `dr_increasing` checked `if (A < 0x50)` after `A` had been overwritten by an
  xpos table read (always < 0x50), so the backdrop branch never fired and
  `draw_road` looped forever.
- `rm_cycle_buffer_offset`: the HEIGHT section was copied from the CURVATURE
  section without updating the escape-byte check (`if (Amapcurvebyte == 0)` for
  `if (Aheight_byte == 0)`), stalling `road_height_ptr` and corrupting the
  perspective table. (`f8241bf`)
- `ds_attributes` loads A = `$E34C` (current delta) and E = `$E34D` (previous),
  saves A to `$E34D`, then `LD A,E` at `$BD67` so the rest of the block works on
  the _previous_ delta. The C kept using A throughout, using the wrong frame's
  data whenever the block fired. The sign-extension byte must also come from E.

**Rule:** Give each logical value its own name (`Aheight_diff`, not `A`). After
copy-pasting a parallel block, check every variable name against the new
context. When the Z80 does `LD A,reg` at the start of a block, work out which
logical value — usually the older one — the block actually needs.

**Commits:** `8071d1f`, `f8241bf`, fix ds_attributes A-vs-E bug

---

## 3. Missing or wrong initialisation

**Root cause:** The Z80 starts in a known state from preceding code.
`chq_initialise()` must set every field that is not zero, especially SM
(self-modifying instruction) operands.

**Bugs:**

- SM fields never set: `dr_left_table_hi_1/2`, `dr_right_table_hi_1/2`,
  `dr.neg_lane_count`, used on the first frame before `dr_four_lane_highway`
  writes them. (`c8251ba`)
- SM field type truncation: `ahc_road_pos_b` declared `u8`, but `$B3A3`
  (`LD DE,$01D8`) is 16-bit, so 472 truncated to 216. Check whether the SM
  instruction loads a pair or a single register. (`bc1e1cb`)
- `ahc_road_pos_a`/`_b` left at 0 by calloc, clamping road_pos wrongly on frame
  one. (`bc1e1cb`)
- `road_buffer_offset` not reset in `set_up_stage` — stale pointer into the
  previous stage's buffer. (`dfdaf8b`)
- `B_iterations`/`C_range` uninitialised in `draw_road_lanes_change`; both must
  be assigned from `C_bresen_range` at the `compute_step` entry point.
  (`7a12c5b`)
- `C_bresen_range`/`B_tbl_stride` set from `A_curve_step` instead of
  `IYheight[0] - ref_height`. (`f98088b`)
- `flipped[]` never built: `bootstrap()` builds it, and the `RUN_FULL_GAME=0`
  path bypasses bootstrap — all flipped sprites black. (`c5c3e6c`)
- Stage not loaded before attract mode: `state->stage` was NULL. (`5bd1c47`)

---

## 4. Pointer arithmetic: direction and carry logic

**Root cause:** PUSH decrements SP, and the Spectrum screen layout makes
"previous scanline" a non-obvious D:E adjustment. `SUB $20` subtracts 32;
`JR NC` skips the adjustment on _no_ carry. Translations routinely invert the
sign or the condition.

**Bugs** (all in `dr_rollover_filled`/`unfilled`, `de88b6f`/`8449d0f`):

- `LO_ADD +32` should be `−32` (Z80 `SUB $20`).
- `D += 16` inverted: the Z80 adds on no-carry, the C added on carry.
- `dr_rollover_unfilled`: `HI_ADD` missing — the `dr_write_scanline_unfilled`
  call had landed inside a brace-less `if`.
- Read-before-decrement: `DEC D; LD A,D` reads D _after_ the decrement; the C
  read it before, skipping rollover with sentinel D=0x01.
- `LO_ADD` computed but never assigned back — see 7.

---

## 5. Wrong pointer type → wrong element stride

**Root cause:** `LD A,(HL)` always loads one byte; a `u16*` doubles every offset
applied to it.

**Bugs:**

- `HLdash` in `dr_fill` was `u16*`, so `HLdash[Ldash]` read word index 255 of a
  128-element array. Fix: `u8*`. (`6217000`)
- `SP_output++` for `INC SP` (`$C407`) advanced 2 bytes. Fix:
  `SP_output = (u16 *)((u8 *)SP_output + 1)`. (`f21a880`)
- A `u8[]` table reached via `LD HL,table-1; ADD HL,BC` gives a **byte** offset
  of `BC−1`. Halving it (`&table[(BC-1)/2]`) assumes 16-bit entries; the correct
  index is `BC−1`, unscaled. (`scroll_horizon`, `$B898`)

**Rule:** Match the C pointer type to the Z80 access width. For byte-wide tables
reached via `ADD HL,BC`, the index is the raw byte offset. See also 29 for the
pointer-to-array form.

---

## 6. Mutating the wrong operand — value vs. pointer, assign vs. accumulate

**Root cause:** Two slips that change the wrong thing. `ADD A,IXl; LD IXl,A`
accumulates across iterations, so plain assignment discards the running total.
`LD A,(HL); DEC HL` reads the byte then moves the pointer — folding the
adjustment into the value edits the wrong object.

**Bugs:**

- `build_curve_table` overwrote the IX position instead of accumulating,
  producing curvature indices of 127 into a 96-entry table. Fix:
  `IXl = (current_offset + 0x40 + curvature_A) & 0xFF`. (`241b3da`)
- `draw_stretchy_object_common` (`$9237`): `doc.rows_2nd = width_bytes - 2`
  instead of stepping the pointer. For a 2-byte-wide masked bitmap that gives 0,
  looping `plot_masked_sprite` past the end of the bitmap array (ASan overflow).
  Fix: `doc.rows_2nd = *HLptr; HLptr -= 2;`. (`dd973cb`)

**Rule:** When the Z80 loads a value then adjusts the pointer, write
`value = *ptr; ptr ±= N;` — never fold the adjustment into the value. See 25 for
the related question of _when_ a computed result is stored back.

---

## 7. Macro used as expression, not statement

A macro defined as a pure expression (`((base)[...])`) has no side effect, so
using it as a statement discards the result. `WRAPPING(ptr, delta, base)`
returned a new pointer but never assigned it back: every `WRAPPINGINCREMENT` in
`build_height_table` was a no-op, `height_table[1..21]` was never filled and
`dr_read_lanes` got a garbage IX pointer (SIGSEGV). Fix: assign in place —
`((ptr) = &(base)[...])`.

**Commit:** `3619da2`

---

## 8. Carry/borrow via bit 7 (sign flag) is unreliable — for both SUB and ADD

**Root cause:** Carry and bit 7 of the result agree only when the result fits in
−128..+127. Outside that range they diverge, and both directions of
mistranslation are common:

- **Subtraction** (carry = borrow): `if (result & 0x80)` or
  `if ((s8)result < 0)` in place of the carry test fires for differences in
  [128, 255], where carry is clear.
- **Addition** (carry = `A+B > 255`): translating `ADD A,B; RET C` as a
  magnitude comparison tests something unrelated; translating `ADD A,C; JR C` as
  a sign test fires for sums in [128, 255] where carry is clear.
- **`int` never wraps:** the unsigned-wrap idiom `if (sum < addend)` is always
  false when `sum` is an `int`.

**Bugs:**

- `draw_road_lanes_change`: `range == 0 || (range & 0x80)` missed differences
  above 128.
- `animate_hero_car` lower-bound clamp: `if ((s8)(L - E) < 0)` fired for L=216,
  E=72 (144 = 0x90, no borrow) since `(s8)0x90 = −112` — the road position was
  clamped every frame, causing flicker.
- `advance_hazard` (`$ADC1–$ADC7`): result stored into a `u8` field, then tested
  via `(s8)` on the already-wrapped value. Fix: compare the operands directly —
  `if (A_old_frac < A_speed_lo) C_dist++;`.
- `draw_stretchy_object_common` (`$921F–$9222`): `JR Z,exit` (clamp to 1 on
  zero) chained with `JR NC,keep` (keep if positive) after `SUB C` together mean
  clamp when the result is **≤ 0**; `if ((s8)Avertical < 0)` dropped the zero
  case.
- `draw_object_right_stretchy_entrypt` (`$9306–$9307`, `ADD A,B; RET C`):
  `if (Awidth_bytes < Bdepth) return` dropped right-side scenery. Fix: add
  first, then `if (Awidth_bytes > 255) return;`.
- `draw_dirt_and_stones` (`$AA33–$AA34`, `ADD A,E; RET NC`): first written as
  `if (A >= E) return`, then re-fixed as `A += E; if (A <= 255) return;` — but
  with `u8 A` the wrapped sum is always ≤ 255, so the draw call stayed
  unreachable. Fix: test in int width _before_ the wrapping add —
  `if (A + E <= 255) return; A += E;`.
- `draw_overhead` (`$90D4–$90D5`, `ADD A,C; JR C`): `if ((s8)A >= 0)` skipped
  the span-width computation for sums in [128, 255].
- `advance_hazard` (`$ADCD–$ADD1`): `C_dist` declared `int`, so
  `if (C_dist < IXhazard->distance)` was always false. Fix: `if (C_dist > 255)`.

**Rule:** For subtraction use a direct unsigned comparison (`if (L < E)`), never
`(s8)` or `& 0x80`. For addition, add first and test the sum against 255 — in
`int` width, since a `u8` sum can never exceed it. When two conditional jumps
follow one `SUB` (`JR Z` then `JR NC`), read them together: the combined
condition is usually `≤ 0`.

**Commits:** `0f95209`, `bc1e1cb`, `02d2a5a`, `9409d37`, `55be0c6`

---

## 9. Wrong constant / literal

Z80 constants are hex and easily misread. `Ccounter` in `draw_road` was
initialised to `3` instead of `$60` (96). See 14 for the loop-count variant.

**Commit:** `d6b91f9`

---

## 10. Boolean convention inverted

Some draw functions use 0 = is-flipped, 1 = not-flipped; passing a raw multi-bit
flags field where a strict boolean is expected inverts both branches.
`draw_object_930e_entrypt` passed `flags >> 1` as `zero_flipped`, taking every
draw path in the wrong order. Fix: derive the boolean explicitly —
`zero_flipped = (flags & MASK) ? 0 : 1`.

**Commit:** `c5c3e6c`

---

## 11. `>> 8` on a `u8` is always 0

A `u8` promoted to `int` and shifted right 8 is zero; the shift was meant to
reach a second packed byte.

- `plot_sprite_flipped_even`: `*src++ >> 8` blanked half the flipped pixels.
  Fix: two separate `flip_table[*src++]` calls.
- The Z80 reads two bytes into a register pair with
  `LD B,(HL); INC HL; LD C,(HL)`; the C packed them into one `u8` and extracted
  the "high byte" with `>> 8`. Fix: two `u8` variables matching B and C.
  (`scroll_horizon`, `$B868–$B86A`)

**Commit:** `c5c3e6c`

---

## 12. Wrong array layout — `$E4xx` page

The Z80 treats a 256-byte page as one flat space; splitting it into
separately-based arrays breaks accesses that straddle the implicit boundary.
`edge_markings` started at `$E410` while all sites subtracted `$E400`, so the
maximum index (0xCC) overran the 192-byte array, and lane markings sat in a
separate array needing different arithmetic. Fix: one 256-byte array for the
whole page (16 leading zeros + 192 edge bytes + 48 lane bytes), every site
subtracting `$E400`.

**Commit:** `9998264`

---

## 13. `addr_to_xpos` even-byte masking hits the wrong byte lane

`addr_to_xpos(state, H, L)` computes `base[L / 2]`, rounding odd low bytes down
— it returns the even (position) byte of a D:E pair, not the odd byte HL pointed
at. In `dr_fill_left_stripe`, `Lrow` starts at `0xFF` and decrements by 2
(always odd), so `if (*HL == 0)` tested the position byte and every marking was
skipped. Secondary error: after `Lrow--` the position byte is `HL[-1]`, not
`*HL`. Fix: `(u8 *)hi_to_xpostab(state, H) + Lrow` for an exact byte pointer.

**Commit:** `fccab5a`

---

## 14. Hex loop bound misread — stale table slots accumulate via `+=`

`LD B,$16` is 22 decimal. Under-counting a fill loop leaves trailing slots
stale, and a later `+=` pass over the full range then grows them unboundedly.
`build_curve_table` used `B = 20`, so `curvature_table[20..21]` were never
freshly written and slot 20 grew by 3 every frame, cycling every ~85 frames
(~1.7s) and making the road near the horizon dance left-right. Fix: `B = 22`;
convert hex loop counts to decimal explicitly and note the source address.

**Commit:** `fe9323a`

---

## 15. Keydef array ordering — sentinel-shift reverses bit positions

`keyscan_keydefs` accumulates key states into `Estopbit` by repeated `RL E` from
a single sentinel bit, so after 8 shifts `keydefs[0]` lands in bit 7 (scanned
first) and `keydefs[7]` in bit 0. `USERINPUTFLAG_*` follows `QPBFUDLR` (bit 7 =
QUIT … bit 0 = RIGHT), so the array must be QUIT-first. It was initialised
RIGHT-first, mapping every key action to the wrong input (P/RIGHT read as QUIT,
and so on); Kempston mode was unaffected because it sets register bits directly.
`ks_common` (`$A0FB`)'s `AND $03; CP $03` / `AND $0C; CP $0C`
simultaneous-direction checks confirm the expected layout.

**Fix:** Descending flag order — QUIT at index 0 (bit 7) down to RIGHT at
index 7. Use physical-order constants (`KEYDEF_QUIT`, …) as indices, not
`USERINPUT_*` bit values. The same ordering governs Kempston's keyboard-only
slots `keydefs[0..2]` (QUIT, PAUSE, TURBO → bits 7/6/5 after
`RRCA×3 + AND $E0`).

---

## 16. u8 wraparound arithmetic — NEG, CPL, ADD, SUB, sign extension, wrap-terminated loops

**Root cause:** Z80 8-bit arithmetic always wraps in the accumulator; a C `int`
does not.

- `NEG` of 254 gives 2 on the Z80, −254 in C.
- `ADD`/`SUB` do not wrap at 256.
- `CPL` complements 8 bits; C `~` complements the promoted `int`, so `~0x40` is
  −65, not `$BF`, and any comparison against a `u8` then succeeds
  unconditionally.
- The sign-extension idiom `if (val & 0x80) val |= 0xFF00;` only works when
  `val` is already a negative `int`; for a positive `int` with bit 7 set it
  gives 65408, not −128.
- `INC A; INC A; JP NZ` steps by 2 and terminates when A _wraps_ to 0.
  `Aiterations += 2; while (--Aiterations > 0)` is a net step of +1 and runs
  roughly twice the iterations.

**Bugs:**

- `update_road_level` stored curvature 254 (signed −2) in a `u8`, read it back
  as `int` 254 and negated with plain `-A`, giving −254; a later `(s8)` cast
  then fired the 4.5× multiply on the wrong sign, jumping the car ~130
  road-position units per frame on any curve. (`087c724`)
- `build_curve_table`'s sign extension used `|= 0xFF00` on a rounded multiply
  result that could be a positive `int` ≥ 128, producing a 65536-unit drift in
  the road-position accumulator.
- `build_curve_table_fill` masked `Atotal &= 0xFF` inside the inner loop but not
  after `Atotal -= Ldash` at the outer boundary, corrupting x-position values
  when that subtraction went negative.
- `layout_road`'s forked-road path ran 207 iterations instead of 104, visiting
  each index twice so the second write silently overwrote the first.
- `draw_helicopter_part`'s `Abot` (A after `ADD A,B`) was `int`, reaching 509
  near a screen edge instead of wrapping mod 256, sending
  `draw_object_left_width_entrypt` down the wrong branch and flipping the sprite
  to the opposite edge. Fix: declare `u8`. (`6ba1edc`)
- `advance_hazard`'s overtake gate (`$AE03–$AE0C`, `AND $E0; CPL; CP (IX+$04)`):
  `A_fc_inv = ~(state->fast_counter & 0xE0)` as `int` yielded −225..−1, so the
  comparison was true for every `u8` `dist_frac`. Every traffic car retired the
  frame its distance reached 1 instead of passing the player, and the perp's
  `$AE20` store took a truncated negative. Fix:
  `0xFF ^ (state->fast_counter & 0xE0)`. Found with the `CHQ_HAZARD_TRACE` log,
  which showed a retire at `t=88`/`frac=32` where the gate value is `$BF` = 191
  and no retire was due.

**Rule:** Cast the result of any 8-bit-wrapping op straight back to `u8`
(`(u8)(-A)`, `(u8)(a+b)`, `0xFF ^ x` for `CPL`); sign-extend by casting the low
byte through `(s8)`/`(s16)`, never by OR-ing in `0xFFnn`; re-mask accumulators
at loop boundaries as well as inside the body. For an `INC A;…;JP NZ` loop, drop
the `--` from the condition — `u8` wrap to 0 terminates it:
`do { … A += n; } while (A != 0);`.

**Commits:** `087c724`, `136e57d`

---

## 17. Off-by-one on a table the Z80 addresses from `base − k`

**Root cause:** The Z80 routinely bases a pointer below a table so that a
1-indexed register lands on entry 0. The C index must be derived from the
computed landing offset, not from the loop variable's face value.

**Bugs:**

- `stages[]` had five entries indexed 0–4 for stage numbers 1–5, so `stages[1]`
  loaded stage 2's data and `stages[5]`/`stages[6]` were out of bounds — stage 1
  opened with a four-lane road because it was running stage 2's map. Fix: seven
  entries, index 0 = `&stage1` (pregame), 1–5 = `&stage1`–`&stage5`, 6 =
  `&stage5` (end-sequence backstop). Whenever `load_stage` indexes
  `stages[wanted]`, confirm the maximum reachable `wanted`, including
  end-of-game transitions.
- `car_jump_params` (`$B059`): the Z80 forms `HL = $B057 + 2A` — base minus 2,
  so 1-indexed A lands on pair A−1. The C used `(Adiff * 2) − 1`, always odd and
  misaligned into the second half of each pair, for `(Adiff − 1) * 2`. `Adiff`
  can also reach 6 (height byte −8 at top speed), where even the Z80 reads the
  two _code_ bytes after the table at `$B063`; the C clamps to 5 with a `Conv:`
  note.
- `hazard_handler`'s slide-target lookup (`$A902–$A907`) bases HL at
  `hazard_pos_speed − 1` before adding the 1-based `current_lane`. The C indexed
  `hazard_pos_speed[current_lane]`, one column right, so a car clamped out of a
  3-lane tunnel's rightmost slot still slid toward the 4-lane road's rightmost
  x-position. (`2bf6d5e`)

---

## 18. `JR Z` / `JR NZ` / `RET Z` / `RET NZ` — branch polarity inverted

**Root cause:** `JR NZ,label` means _act if non-zero_, so the code that runs
when the jump is skipped runs on the opposite condition to what the mnemonic
suggests at a glance.

**Bugs:**

- `update_road_level`: `if (Ay_offset) { /* jump setup */ }` where the Z80 skips
  the setup when `mhc.y_offset != 0` — the launch code ran only when the car was
  already airborne. Fix: `if (!Ay_offset)`.
- `scroll_horizon` (`$B8A5–$B8A6`, `AND A; RET Z`): `if (Adiff) return` returned
  exactly when scrolling was due, making the function a no-op.
- Three-way ladder collapsed to two-way — `layout_road`'s fork-side choice
  (`$BA7A–$BA88`): `DEC D; JP M,right; JP NZ,left; <D==1: E<12 test>`. The C
  handled only `D==1` and defaulted the rest to the right fork, but the Z80
  takes the _left_ fork for `D−1` in 1..0x7F. (`f47f822`)

**Rule:** `JR NZ → skip` is `if (reg == 0)`; `JR Z → skip` is `if (reg != 0)`;
`RET Z` is `if (value == 0) return`. The fall-through should always be the
"there is work to do" path. Every exit of a branch ladder needs its own C arm —
do not fold the middle exits into the default.

**Commits:** `136e57d`, `41de175`

---

## 19. SBC carry chain — carry not propagated between chained subtractions

`SBC HL,DE` takes carry as borrow input, so in a sequence the first `SBC`'s
carry output feeds the second. Writing the second as plain `-=` leaves `carry`
stuck at the first test's result. `handle_perp_caught` (`hpc_move_perp`)
computed `carry = (HLroadpos < ROAD_LEFTMOST)` then did
`HLroadpos -= ROAD_RIGHTMOST` without updating it, making the LEFT input
unconditional and the centre-zone branch dead code. Recompute after every
subtraction the Z80 models as `SBC`:

```c
HLroadpos -= ROAD_RIGHTMOST;
carry = (HLroadpos <= 0); /* Z80 SBC HL,DE with carry_in=1 */
```

**Commit:** `136e57d`

---

## 20. `JP M` / `JP P` as conditional skip — not a loop

**Root cause:** `JP M,addr` jumps _forward_ when the Sign flag is set, so two
`CALL`s separated by `JP M` are two sequential operations with a conditional
skip over the first:

```
CALL first_routine      ; may be skipped
JP M, after_second      ; if negative, skip to after second
CALL second_routine     ; reached only when first was not skipped
after_second:
```

**Bug:** `draw_object_clipped` (`$9404–$941D`): a `for(;;)` translation never
terminated when `doc.rows_2nd == 0` (e.g. a 2-byte-wide bitmap), hanging
`draw_scene_objects` on narrow stretchy segments. Fix: two straight-line call
sites with an `if` guard over the first.

**Rule:** `CALL; JP M/P,skip_target; CALL` is never a loop. But before writing a
`Conv: NOT a loop` comment, grep the skool for every `JP`/`JR`/`DJNZ` targeting
the block — one back-edge (`$9417 JP $9404`, in this same function) makes it a
genuine loop however sequential it looks.

**Commit:** `7e54fb2`

---

## 21. SP-based bulk fill/copy — `PUSH` fills backward, `POP` copies forward

**Root cause:** `PUSH` decrements SP by 2 before writing; `POP` reads then
increments. `LD SP,HL; PUSH BC × N` fills `2N` bytes at `HL−2N … HL−1`, not
forward from HL.

**Bugs:**

- `ds_attributes` treats `horizon_attribute` as a pointer to the _last_ byte of
  the row and pushes backward 15 words, filling bytes 1–30.
  `memset(HLattrs, colour, 30)` spilled sky colour into the wrong rows. Fix:
  `memset(HLattrs - 30, colour, 30);`.
- `draw_tunnel` enters a fixed 16-slot `PUSH` chain via `JP (IX)` at a variable
  offset. A 16-case switch works but is needless:
  `n = 16 - start; SPoutput -= n*2; memset(SPoutput, fill, n*2);`.
- `plot_sprite_even`/`plot_sprite_odd` copy sprite bytes verbatim via
  `LD SP,HL; POP DE` — one `memcpy(dst, src, n)`, valid only because no mask or
  flip transform is applied.

**Rule:** N `PUSH`es after `LD SP,HL` = `memset(ptr - 2*N, value, 2*N)`; N
verbatim `POP`s = `memcpy(dst, ptr, 2*N)`. Compute N from the jump-table index
rather than unrolling a switch. Masked or flipped sprites are not `memcpy`
candidates. See 27 for what happens when the jump-table offset overshoots the
chain.

---

## 22. Signed Z80 register used in arithmetic

Merged into 1 — a field holding a signed offset needs `(s8)` at the use site, or
an `s8` declaration.

**Commit:** `944371a`

---

## 23. Shadow registers — `EX AF,AF'` and `EXX`

**Root cause:** `EX AF,AF'` swaps A _and_ the whole flag register; `EXX` swaps
BC/DE/HL. Per the CLAUDE.md convention the C keeps a separate variable for the
shadow side, and every mistake below is that variable being written, read or
scoped wrongly.

**Wrong side assigned.** Capture the value at the _bank_ point, not the unbank
point, which by then holds something unrelated.

- `scroll_horizon`'s vertical-scroll loop: the final `EX AF,AF'` hands A
  (adjusted counter) to `var_a25b` and A' (accumulated delta) to `var_a25a`; the
  C had them swapped.
- `draw_dirt_and_stones` (`$A9FF`): `EX AF,AF'` banks a table byte into A', then
  A is overwritten by an unrelated LOD index. The C captured `saved_A` at the
  _unbank_ comment, grabbing the always-non-negative index and making
  `if (saved_A < 0)` permanently dead.

**Restored flags drive the next branch.** `JP P`/`JP M` after an `EX AF,AF'`
tests the flags restored by that EX — set before the _original_ bank — not
anything since, even a `LD A,C` immediately before the branch. In
`scroll_horizon` (`$B872`) the restored AF reflects `AND A` at `$B851` (sign of
`current_curvature`), so `LD A,C; JP P,$B879` branches on curvature. The C
tested the always-non-negative table value in C, so NEG never applied and the
backdrop could only scroll rightward.

**Wrong variable compared after an odd number of swaps.** In
`draw_road_lanes_change` (`$C357`/`$C3CA`) the distance (IYl) and the masked
lane flags ping-pong through A/A'; the path to each far-boundary branch passes
`$C2EE`, `$C30A`/`$C37E` and a third EX at the branch itself, so the `CP $04`
there tests the _lane flags_. The C tested `A_dist != 4` — correct
`// EX AF,AF'` comments, wrong variable read — so the angled transition piece
only drew when a slot happened to sit at distance 4, and the masked==8/12
classes smeared the verge across the road. Count the EX instructions on the
actual path to the comparison.

**One variable for both banks (was 32).** `draw_forked_road`'s zone boundary
reads run on a shadow L′ reloaded from main L each scanline pass
(`$C96D LD A,L; EXX; LD L,A`), so boundary 4's `DEC L` (`$C9F0`) and the
zero-fill path's `LD HL,$0000` (`$C937`, the PUSH fill _value_) touch only the
shadow. Sharing one `L` meant the zero-fill zeroed the main row index,
scrambling every zone and marking read for the rest of the frame, and boundary
4's `L--` flipped the row parity for all subsequent rows once the fork-right
table came on-screen — boundary reads then read s16 low bytes as flag bytes, the
zones collapsed to full-width verge and the road vanished towards the horizon.
The first fix converted three of the four boundary reads by exact-string
matching; the fourth survived because its comment differed. (`c5b6347`,
`1c611dd`)

**Bank and unbank in different functions (was 33).** A Z80 shadow register
survives the whole intervening call chain; the C has no equivalent unless the
byte is stored in `chqstate_t`. `move_hero_car` banks `BCcount_scaled` at
`$B296`; `scroll_horizon` unbanks it at `$B854` to pick one of four
`horizon_table` rows for the curve x-scroll rate. The C treated the value as
unknowable (`Adash = 0`), reaching only two of the four rows and producing a
periodic stutter on curves. Fix: `state->curvature_scroll_shadow`, written at
the bank site and read at the unbank site — after confirming from the skool that
nothing on the path between them (`spawn_cars`, `cycle_counters`,
`play_engine_or_siren_sfx_hook`, `build_height_table`) executes `EX AF,AF'`
itself. Before writing off a cross-function shadow read as "approximate with 0",
do that grep: if nothing intervenes, the value is exact, not approximate.
(`cf269e4`)

**Commits:** `41de175`, `55be0c6`, `87f70fa`, `a0e41fd`, `c5b6347`, `1c611dd`,
`cf269e4`

---

## 24. Writing to the wrong destination

**Root cause:** Three ways the write lands somewhere the Z80 never touches: the
wrong field of the right struct, an output parameter invented for a result the
caller discards, and arguments crossed at the call site.

**Bugs:**

- `advance_hazard` (`$AE74–$AE79`): `LD (IX+$02),L` / `LD (IX+$03),H` write
  `horz_pos`/`horz_clip`, but the C wrote the low byte into `distance` (offset
  1), clobbering the distance computed a few lines earlier. Since the
  depth-sorted draw list matches on `distance`, hazards were computed every
  frame but never drawn.
- Same site, the `check_collision` call: the skool marks its result "ignored",
  but `check_collision` internally reloads `hazard->horz_pos`/`horz_clip` for
  its own bounding-box test. The C piped that reload through an `HLout`
  parameter the caller wrote back, pinning every hazard near its stale (often 0)
  position and preventing collisions.
- `check_fork_scenery_collisions(state, DEdash, HLdash)` was called as
  `(state, HLdash_road_pos_a, DEdash_road_pos_b)`, so the road clamp bounds
  stored swapped (min 472, max 72) while the fork was active and
  `animate_hero_car` latched `road_pos` to alternating ends of the road every
  frame — a whole-screen two-frame flicker. (`1c611dd`)

**Rule:** Check the struct's field-offset table against the C member at each
write; a field written correctly earlier is a red flag if it reappears as a
write target later. Before adding an output parameter, read what the caller does
with the register after `CALL` returns — "result ignored" means do not invent
one. Where parameters are named after registers, check each call site against
the _callee's_ declared order, which need not match the caller's declaration
order.

**Commits:** `02d2a5a`, `be0ef28`

---

## 25. Field decrement timing — store the wrapped result unconditionally, or not at all

**Root cause:** Two opposite mistakes about _when_ a Z80 subtraction writes
back. `SUB $01` always executes and stores its wrapped result before branching
on carry. `LD A,(addr); DEC A; RET Z` has no matching store — the Z80 only tests
a transient copy.

**Bugs:**

- `read_map`'s perp-distance countdown (`$C096–$C0B9`): the C pre-checked
  `distance == 0` and decremented the high byte only in that branch, leaving
  `distance` pinned at 0 and the high byte free-running, eventually tripping an
  assert in `plot_turbos_and_digits`. Fix:

  ```c
  A_flags_inc = IX_hazard->distance;
  IX_hazard->distance = (u8)(A_flags_inc - 1);   /* SUB $01 always executes and stores */
  if (A_flags_inc == 0) {                        /* pre-subtract value 0 -> this subtract borrowed */
      IX_hazard->hazard_lane_OR_perp_dist_hi--;
      continue;
  }
  ```

- `play_engine_or_turbo_sfx_128k` (`$F2FA–$F301`): A is never stored back to
  `$A23A`; the field is only assigned elsewhere (`0x3C`, `0`, `1`).
  `if (--state->turbo_sfx_pitch == 0) return;` decremented the field every frame
  the turbo effect ran, terminating it on its own 59-frame schedule instead of
  being driven solely by `ay_noise_pitch`. Fix:
  `if ((u8)(state->turbo_sfx_pitch - 1) == 0) return;`.

**Rule:** Never gate a `SUB $01`/`DEC` on the pre-subtraction value to decide
_whether_ to store — perform and store the wrap, then use the captured pre-value
for the borrow branch. Conversely, before translating
`LD A,(field); DEC A; RET Z` as `if (--state->field == 0)`, check the skool for
a matching store; no store means test a copy.

**Commits:** `507aa97`; (uncommitted — `play_engine_or_turbo_sfx_128k` fix)

---

## 26. Finish the audit even after finding a real bug

**Root cause:** One symptom — "objects lose rows from the top as they grow
closer" — had three independent causes in and around `draw_object_clipped`. Each
fix was individually correct against the skool and individually insufficient.

- **`prev_buf_row()`** (`Main.c:595`) applied the back-buffer marker
  compensation (`backbuf += 0x1000`) whenever the row-group field wrapped. The
  skool (`$B759–$B769`) applies it only when the following `RRRC` subtraction
  does _not_ borrow (`JR C,$B71C` skips it). Fix: compute the subtraction first,
  gate the `+= 0x1000` on `t >= 0`.
- **`D_col_pos` sign** (`doc_y_range_nonzero`, `$9359`):
  `Adash_y_range += D_col_pos` assumed the value is always ≤ 0, but
  `draw_hazard_sprites` sets it to `persp_col - hit_wobble`, commonly positive
  for cars. For positive values the Z80's `BIT 7,D; JR NZ` selects `SUB D`, so
  the C's unconditional `+=` inflated `y_range` and routed close objects into
  the wrong re-clip path.
- **Row count from the wrong shadow register** (`doc_compute_bitmap`,
  `$93AE–$93B4`): the direct-dispatch plot calls all read their row count from
  shadow `B'`, banked with `B_clip_rows` at `$93AE`/`$93B1` — never from
  `B_height`, which is popped back into main `BC` purely for the address
  calculation. The C used `min(B_height, B_clip_rows)`, correct only when
  `B_height > B_clip_rows`. Left-side callers pass a fixed `B_height = 1`, so
  the clamp almost never fired and the left/verge path drew 1 row regardless of
  the object's real height. Fix: `B_height = B_clip_rows;` before the dispatch,
  keeping `Bdash_height` for the address calc — mirroring the fix already
  present in the neighbouring `doc_masked_rows` loop (`$945C`).

**Rule:** When a reported symptom survives a fix that is individually verified
correct, keep going. Source pointer, destination pointer and row _count_ can
each carry an independent bug feeding the same visible effect. An asymmetric
symptom is a strong clue: look for the code path that is genuinely
side-specific, not the shared body both sides call.

**Commit:** `c41e508`

---

## 27. Jump-table `JR` offsets can land inside a _later_ PUSH chain

**Root cause:** A self-modified `JR` used as a jump table is not bounded by its
own chain: the target is simply `PC + 2 + offset`, so a large offset sails past
the end of the first chain, past the instructions between chains, into the
middle of the next one.

**Bugs** (`draw_tunnel`, `$C21F–$C2E1`):

- `$C221 JR <D>` with `D = 22` (the `dt_max_fill` case) lands at `$C239`, midway
  into the _second_ chain. The row is one combined 15-push (30-byte) fill from
  the original SP, and the skipped instructions include `$C233 SUB C` and the
  second `LD SP,HL`, so the later `$C248 ADD A,C` nets `L += C` instead of
  restoring L. The C treated `D = 22` as "first fill empty" plus a normal second
  fill, drawing little or nothing on exactly the widest rows of the tunnel
  mouth.
- The second-phase loop (`$C287`) and far-wall loop (`$C2C1`) look structurally
  identical to the first but contain **no jump table**: 15 unconditional
  `PUSH`es, a full-width fill every row. The C copied the first loop's
  `dt.fill_start_b` gating into both, drawing partial bands, or nothing when the
  stored index was 16.

**Rule:** For every value the code can store into a jump-table `JR` operand,
compute the landing address by hand and read what executes from there, including
which set-up instructions get skipped. Where several fill loops sit side by
side, check each for the presence of its own jump table.

**Commit:** `e0cf3a9`

---

## 28. `u16` field that goes transiently negative

Merged into 1 — every load into a wider type needs `(s16)`.

**Commit:** `f47f822`

---

## 29. Pointer-to-array parameter indexed directly — steps whole tables, not elements

Stage bitmap tables are declared `const bitmap_t (*x)[SPRITE_FRAMES]` — a
pointer to a whole 6-entry table. `x[A]` advances by A _tables_; the A-th
element is `(*x)[A]`. Both compile silently because both yield
`const bitmap_t *`.

`draw_dirt_and_stones` (`$AA13–$AA19`: `L = A*7; ADD HL,DE`, one 7-byte entry
per LOD) used `DEbitmaps[A]`, reading up to 30 entries past
`stage1_stones_bitmaps` (ASan: just past `stage1_map_goto_table`). The garbage
descriptor went to the sprite plotter, which scribbled 0xAA dither bytes over
`road_buffer` — surfacing as the `draw_scene_objects` `Aobj=170` assert on
dirt-track sections. Fix: `HLbitmap = &(*DEbitmaps)[A];`.

**Rule:** For any `(*ptr)[N]` parameter, element access is `(*ptr)[i]`; `ptr[i]`
is a table stride. `draw_helicopter` still contains the suspect form
(`helibitmap = *helibitmaps++;` beside a "can't be right" comment) — audit it
against `$AA38` when helicopter data is completed.

**Commit:** `e457f6b`

---

## 30. C arithmetic does not inherit the Z80's structural bounds

**Root cause:** `draw_forked_road` derives five scanline fill widths as
differences of cumulative zone positions (each 0..15). The Z80 emits each zone
through a self-modified `JR` into its own fixed 15-`PUSH` chain (`$CA11–$CA65`),
so a crossed boundary (negative difference) at worst makes the `JR` skip past
its chain — a bounded one-scanline glitch. The C `(u8)(a − b)` wrapped to ~250
and the `memset`s ran hundreds of bytes backwards out of the backbuffer.

**Bug:** On transient rows during dirt/fork transitions the zone positions cross
(`pos_EA < pos_E8`). `road_buffer` right-side object slots took the stripe fill
byte, tripping the `draw_scene_objects` `Aobj <= 9` assert many frames later
when the scan window reached it. Fix: compute the widths as `int` and fill with
a 15-pair budget — each zone takes at most its non-negative width, right to
left, the lefthand verge takes the remainder, so the scanline is always exactly
filled and the cursor cannot escape.

**Rule:** Where the Z80's worst case is bounded by _code structure_ —
fixed-length PUSH chains, page-wrapped `INC L`, self-modified low bytes —
reproduce the cap explicitly and say so in a `Conv:` comment. A wild write that
stays inside `chqstate` is invisible to ASan; the road-buffer diagnostics
(`rm_prewrite`, `chq_test_max_side_object`) and a bisecting watchpoint found
this one.

**Commits:** `e457f6b`, `c5b6347`

---

## 31. Chained self-modified sections — apply each section's `INC H` at its entry, with its exact count

**Root cause:** `draw_forked_road` walks the xpos pages twice per scanline, and
both walks advance H by `INC H` at the _start_ of each step with irregular
counts. Two opposite miscounts, months apart, in one function:

- **Fill boundaries** (`$C973–$C9F6`): the Z80 walks `$E8`, `INC H ×2` → `$EA`,
  `INC H` → `$EB`, `INC H ×2` → `$ED` (left road, median, right road). The C
  read consecutive `$E8/$E9/$EA/$EC`, shaping every fork scanline from the wrong
  tables — striped garbage across the fork.
- **Marking sections** (`$CA90/$CAAD/$CACE/$CAF2/$CB0F`): each of sections 2–6
  _begins_ with a single `INC H`. The C incremented _after_ each section, so
  every section from 2 on read the previous section's table: the lane dash drew
  at the left-edge position, the edge markings walked inward one boundary, and
  `$ED` — the right road's right edge — never received a marking.

**Rule:** Transcribe each section's `INC H` count at its entry point and verify
every read's H page against the skool's address comments. Do not assume the
pages are consecutive (the fill skips two) or that a shared increment can trail
the section body (the markings advance before reading).

**Commits:** `c5b6347`, `a5f0989`

---

## 32. One C variable modelling both banks of a register

Merged into 23 — give the banked side its own variable, then audit every
mutation of the shared name.

**Commits:** `c5b6347`, `1c611dd`

---

## 33. `EX AF,AF'` banked in one function, read in another

Merged into 23 — the shadow value needs a `chqstate_t` field, not a local.

**Commit:** `cf269e4`

---

## 34. `INC E` wraps only the low byte

**Root cause:** `dr_fill_left_stripe`'s two-byte edge/lane writes are
`LD (DE),A; INC E; LD (DE),A`. `INC E` wraps 0xFF to 0x00 with no carry into D,
so the second byte always lands in the _same_ backbuffer row at column 0. C
`*ptr++ = …; *ptr = …` instead carries into the next row, and off the end of
`backbuffer[]` at the last row, whenever the column offset is 0xFF.

**Secondary bug, same commit:** `Ldash_backbuf` (low byte of
`DEdash_backbuf + 31`) was `int`, so the unmasked +31 could exceed 255 and
corrupt the row when recombined into an address.

**Rule:** Model any single-register `INC r` feeding a second memory access as
masked low-byte arithmetic —
`ADDRTOBACKBUF((DEdash_backbuf & 0xFF00) | ((Edash + 1) & 0xFF))` — not a
pointer increment, and declare every stand-in for an 8-bit register as `u8`,
never `int`. Check whether the register is a pair (`INC DE`, carries) or single
(`INC E`, wraps) before translating.

**Commit:** `275014e`

---

## 35. `XOR A; IN A,($FE)` is an any-key read, not a half-row read

**Root cause:** Half-rows are selected by the _high_ byte of the port address.
`LD A,$F7; IN A,($FE)` reads `$F7FE`, the "1"–"5" row. `XOR A; IN A,($FE)` reads
`$00FE`, asserting all eight row-select lines and returning the AND of every
row: any key. The two differ by one instruction and often sit a few bytes apart.

**Bug:** All three `XOR A` sites in bank 3 (`$FC00` options-menu exit debounce,
`$FECA` redefine-keys pre-capture debounce, `$FF02` test-mode confirmation) were
translated as `port_KEYBOARD_12345`, copied from the genuine `LD A,$F7` poll at
`$FBAE` a few lines above, `Conv:` comment and all — so the test-mode screen
could only be dismissed with "1"–"5". The equivalent 48K routines (`$ED0B`,
`$ED43`) were right first time: the main skool annotates them
`; Read keyboard port $00FE`, the bank-3 lines are bare. Fix:
`port_BORDER_EAR_MIC` (`$00FE`), which `zx_in` already implements as the AND of
all eight half-row queries.

**Rule:** Read the instruction that loads A, not the `IN`. Where a routine polls
both a specific row and "any key", expect them within a few bytes of each other,
and do not carry the row constant or its `Conv:` comment across. An uncommented
skool line is not a licence to pattern-match the neighbouring block.

**Commit:** "Fix: Accept any key where the Z80 reads every keyboard half-row"

---

## 36. A dispatch target that "looks like a crash" is usually a deliberate stack unwind

**Root cause:** A computed dispatch can legitimately land on a `POP` that
discards the current routine's return address, so the following `JP` returns two
or more levels up. Read as straight-line code this looks like stack corruption,
and the tempting conclusion — "this byte can never be dispatched" — turns a real
command into a no-op. `POP HL` immediately before a `JP`, with the popped value
never used, is the signature: the pop is there for its stack effect, not its
data.

**Bug:** The 128K title tune never stopped. Nothing clears the tune-active flag
`$F223` on a timer — the tune _data_ ends with pattern command `0x8E`, whose
dispatch (`$EE9D JP (HL)`) lands on `$ED2F POP HL; JP $ED0B`. The `POP` discards
`advance_channel_pattern`'s return address so `stop_music_and_silence`'s `RET`
unwinds past the rest of the driver, back to `titlescr_music` at `$F832`. The
port's `default` case treated `0x8E` as a no-op on the recorded assumption that
the target was "almost certainly a crash, so this byte is assumed never to
appear as an executed command". It sits at `title_tune0_data[959]` (`$F600`) and
is dispatched every play-through. Fix: handle `0x8E`, and model the unwind by
returning a flag the caller acts on.

The same idiom appears in the drum-cue script reader:
`$F829 POP HL; POP HL; DI; JP $ED0B` unwinds _two_ frames for that script's own
end byte. It was translated correctly, which is the tell — the same "impossible"
shape twice in one bank is a convention, not a bug in the original.

**Rule:** Before dismissing a dispatch target as unreachable, check whether it
starts with a `POP` of a register that is then unused. Count the pops to find
how many levels it returns, then give the C an explicit return value rather than
falling through. Settle it by tracing, not by reasoning: one grep of the trace
for the dispatch address is decisive. Data is evidence too — a byte occurring at
the end of every tune's stream is a marker, not noise.

**Commit:** `157f863`

---

## 37. Trace-verifying a banked address — the same address holds different code in different banks

**Root cause:** `$C000-$FFFF` is the paged window, so an emulator trace filtered
on a bare address mixes every bank that was paged in during the run. Statistics
over that mixture describe nothing real.

**Bug:** Calibrating `TITLE_ANIM_TSTATES` from `trace_calibrate.py` on `$C6C7`
(`titlescr_animate_frame`'s loop body) gave a median inter-hit delta of 6184
T-states — nonsense for a loop that opens with `EI; HALT`. Of the 5294 hits,
4749 were `XOR $20` from another bank and only 545 were the title loop's
`LD B,$06`. Filtered on the instruction, the median is 70908 — one 128K frame,
spread 70905..70911.

**Rule:** When trace-verifying anything at `$C000` or above, check the mix
first:

```sh
grep -E '^\$C6C7\\t' trace.log | awk -F'\\\\t' '{print $7}' | sort | uniq -c | sort -rn
```

One distinct instruction means the address is unambiguous in that run. Two or
more means every number derived from it is contaminated. Below `$C000` the
window is unpaged and this cannot arise.

**Commit:** `157f863`

---

## 38. High-score flash-phase bytes — wrong rotation model, and a mutation with no source instruction

**Root cause:** Two independent bugs in the same feature (hiscore name-entry
flicker), both found chasing one user report ("the confirmed name-row text
pulses far too fast").

**Bug A — `RLC` on a flag byte is a multi-frame duty cycle, not a per-frame
toggle.** `RLC (HL)` rotates the whole byte and branches on the bit that rotates
out (carry), not a single persistent bit. A byte with more than one bit set —
e.g. seeded `0xF0` — spends several consecutive rotations with carry set, then
several with carry clear, before repeating. Translating the toggle as
`flag ^= 1` (bit semantics) instead of an 8-bit rotate collapses an
N-consecutive-frames-on/N-off square wave into a 1-frame alternation, N times
too fast.

`hiscore.draw_erase_toggle` (real `$C58D`, seeded from the `$C580` ROM template
= `0xF0`) is `RLC`'d once per frame in `redraw_name_frame`/`name_entry_frame`;
`JR C` selects erase, fallthrough selects draw. `0xF0` rotated gives four
consecutive carry-set frames then four carry-clear, an 8-frame (~6.25Hz) cycle.
The C port used `draw_erase_toggle ^= 1` seeded to `0`, alternating every frame
(~25Hz) — the confirmed name-row text flickered four times faster than the
original, audible against Fuse's visibly slower pulse. Fix: rotate the byte
(`(toggle << 1) | (toggle >> 7)`) and read bit 0 as carry, seeded `0xF0`,
mirroring `flash_phase_a`'s already-correct treatment in the same file.

**Bug B — a mutation with no matching Z80 instruction, contradicting the
porter's own comment.** `name_entry_dispatch`'s 12-frame timer-gated block
(`$C172-$C19B` in the skool) had `flash_phase_a ^= 1;` and a conditional
`flash_phase_b ^= 1;` spliced in. The skool has exactly two sites touching
`$C59A`/`$C59B` (`$C1A8-$C1AB`, `$C1CA-$C1CD`), both unconditional `RLC`s run
every frame from a different function (`fast_blink_best_officers_cell`) — and
that function's own prologue comment already says so ("$C1A8-$C1C2 also runs
every frame (not gated by the blink timer)"). The extra XOR corrupted the
fast-blink phase every 12 frames, glitching the "BEST OFFICERS" marquee's
flicker on top of the row-text bug. Fix: delete both lines; nothing in the
timer-gated block touches these fields on real hardware.

**Rule:** Before translating any `RLC`/`RRC` self-toggle as a boolean flip,
check its seed value — an alternating seed (`0x55`, `0xAA`) really does toggle
every rotation, but any other seed produces runs of same-carry frames whose
length depends on the seed's bit pattern; model the byte and rotate it, do not
compress it to one bit. Separately, when a field name recurs across two
functions, grep the skool for every address that writes it — a mutation with no
corresponding instruction is a fabrication, and a prologue comment that already
states the field's real cadence is a direct contradiction, not just a hint.

**Commit:** (uncommitted)
