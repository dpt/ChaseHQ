# Z80 → C Translation Pitfalls

Recurring mistakes encountered porting Chase H.Q. from Z80 to C. Each entry has: the root cause, the symptom, and the fix pattern.

---

## 1. Signed/unsigned type mismatch on local variables

**Root cause:** Z80 registers are 8-bit with no inherent sign; the same byte is used signed or unsigned depending on context. Defaulting to `u8` is wrong when subtraction can produce a negative result or when `JP M`/`JP P` branches on the sign flag.

**Symptoms:**

- `if (X < 0)` on a `u8` is always false — dead negative branch. (`L_step`, `A_step`, `A_delta_lo` in `draw_road_lanes_change`; `Anew_diff` in `dr_fill_left_stripe`.)
- `if (X > 0)` on a `u8` is true for all non-zero values — unreachable negative branch. (`Aheight_diff > 0` in `dr_fill_left_stripe`, fixed to `(s8)Aheight_diff > 0`.)
- `>= 128` boundary miss: `A > 128` fails for `A == 128` (0x80 has Sign flag set). Fix: `A >= 128`.
- **`s8` cast on a 16-bit `SBC HL,DE` result:** the Sign flag reflects bit 15 of the 16-bit result; casting the `u16` result to `s8` tests bit 7 of the low byte only. `0xFF64` is negative as `s16` but has bit 7 clear in its low byte. Fix: cast to `s16`, not `s8`. (`f21a880`)

**Fix:** Declare the variable `s8` when the Z80 treats it as signed; drop the redundant `(s8)` cast at comparison sites. 16-bit arithmetic (`SBC HL,DE`, `ADD HL,DE`) needs an `s16` cast.

**Commits:** `c8251ba`, `6ce786f`, `4a9eb7f`, `2ab21904`, `f21a880`.

---

## 2. Stale register — wrong variable after reuse or an old value swapped in

**Root cause:** Z80 reuses A (and other registers) for different logical values within one function; the C port names locals after the register, so the same name can silently mean different things at different points. Two shapes:

- **Overwrite before use:** a later instruction clobbers the register before an earlier comparison (that should read the old value) executes.
- **`LD A,reg` swap-in:** the Z80 deliberately loads an *old* value into A right before a computation, even though a newer value is also in scope — easy to use the newer one by mistake since the C port just reads variable names, not instruction timing.

**Bugs:**

- `dr_increasing` checked `if (A < 0x50)` but `A` had been overwritten by an xpos table read (always < 0x50), so the backdrop branch never fired and `draw_road` looped forever.
- `rm_cycle_buffer_offset`: the HEIGHT section was copied from the CURVATURE section without updating the escape-byte check (`if (Amapcurvebyte == 0)` instead of `if (Aheight_byte == 0)`), stalling `road_height_ptr` and corrupting the perspective table. (`f8241bf`)
- `ds_attributes` (`update_screen`) loads `A = $E34C` (current delta) and `E = $E34D` (previous delta), saves A to `$E34D`, then does `LD A,E` at `$BD67` so the rest of the block operates on the *previous* delta. The C port kept using `A` (current delta) throughout, using the wrong frame's data whenever the block fired. Fix: after the save, switch to `E` for all subsequent computation — compute the sign-extension byte from `E` before shifting it.

**Fix:** Give each logical value its own named variable (`Aheight_diff`, not just `A`), and reference it everywhere the register is still logically that value. After copy-pasting a parallel block, grep every variable name and confirm it's right for the new context. When the Z80 does `LD A,reg` at the start of a block, identify which logical value (usually the *old* one) the block actually needs.

**Commits:** `8071d1f`, `f8241bf`, fix ds_attributes A-vs-E bug

---

## 3. Missing or wrong initialisation

**Root cause:** The Z80 starts in a known state from ROM/BIOS or preceding code. `chq_initialise()` must explicitly set every field that isn't zero, especially SM (self-modifying instruction) operand fields.

**Symptoms and fixes:**

- **SM fields never set:** `dr_left_table_hi_1/2`, `dr_right_table_hi_1/2`, `dr.neg_lane_count` — used on the first frame before `dr_four_lane_highway` writes them. (`c8251ba`)
- **SM field type truncation:** `ahc_road_pos_b` declared `u8` but the SM instruction at `$B3A3` (`LD DE,$01D8`) is 16-bit; storing 472 truncated to 216. Fix: declare `u16`. Check whether the SM instruction loads a register pair or a single register/byte. (`bc1e1cb`)
- **`ahc_road_pos_a`/`ahc_road_pos_b` not initialised:** default `LD DE` operands (72, 472) left at 0 by calloc, clamping road_pos incorrectly on frame one. (`bc1e1cb`)
- **`road_buffer_offset` not reset in `set_up_stage`:** stale pointer into the previous stage's buffer. (`dfdaf8b`)
- **`B_iterations`/`C_range` uninitialised in `draw_road_lanes_change`:** both must be assigned from `C_bresen_range` at the `compute_step` entry point. (`7a12c5b`)
- **`C_bresen_range`/`B_tbl_stride` set from the wrong variable:** used `A_curve_step` instead of `IYheight[0] - ref_height`. (`f98088b`)
- **`flipped[]` table not built:** `bootstrap()` builds it; the `RUN_FULL_GAME=0` path bypassed bootstrap, leaving all flipped sprites black. (`c5c3e6c`)
- **Stage not loaded before attract mode:** `state->stage` was NULL. (`5bd1c47`)

---

## 4. Pointer arithmetic: direction and carry logic

**Root cause:** Z80 PUSH decrements SP; the ZX Spectrum screen layout means "previous scanline" involves non-obvious D:E arithmetic. `SUB $20` subtracts 32; `JR NC` skips the D adjustment on *no* carry. C translations routinely invert the sign or condition.

**Bugs (all in `dr_rollover_filled`/`unfilled`, `de88b6f`/`8449d0f`):**

- `LO_ADD +32` should be `LO_ADD -32` (Z80 `SUB $20`).
- `D += 16` condition inverted: Z80 adds 16 on no-carry; C added it on the carry branch.
- `dr_rollover_unfilled`: `HI_ADD` missing entirely — the `dr_write_scanline_unfilled` call was accidentally inside a brace-less `if`.
- Read-before-decrement: Z80 `DEC D; LD A,D` reads D *after* decrement; C read D before, skipping rollover with sentinel D=0x01.
- `LO_ADD` macro computed but never assigned back — a pure expression used as a statement (no-op).

---

## 5. Wrong pointer type → wrong element stride

**Root cause:** Z80 `LD A,(HL)` always loads one byte; a `u16*` C pointer doubles any byte offset or increment applied to it.

**Bugs:**

- `HLdash` in `dr_fill` was `u16*`; `HLdash[Ldash]` treated byte offset 255 as word index 255, reading out of the 128-element array. Fix: `u8*`. (`6217000`)
- `SP_output++` for `INC SP` (`$C407`) advanced by 2 bytes instead of 1 (`u16*`). Fix: `SP_output = (u16 *)((u8 *)SP_output + 1)`. (`f21a880`)
- A third form: a `u8[]` table indexed via `LD HL,table-1; ADD HL,BC` gives a **byte** offset of `BC-1`. Halving this index (`&table[(BC-1)/2]`) under the wrong assumption that the table holds 16-bit words is a bug for a `u8` table — the correct index is `BC-1`, unscaled. (`scroll_horizon`, `$B898`)

**Fix:** Match the C pointer type to the Z80 access width; for byte-wide tables accessed via `ADD HL,BC`, the index is the raw byte offset.

---

## 6. Mutating the wrong operand — value vs. pointer, assign vs. accumulate

**Root cause:** Two related slips where the C port changes the wrong thing:

- **Accumulate vs. assign:** `ADD A,IXl; LD IXl,A` accumulates into a register across iterations; plain assignment (`IXl = A`) discards the running total.
- **Pointer step mistaken for value edit:** `LD A,(HL); DEC HL` (or `DEC HL; DEC HL`) reads the byte at HL into A, unaffected by the following pointer move. `A = *ptr - N` (or `*ptr -= N`) edits the *value* instead of stepping the *pointer*.

**Bugs:**

- `build_curve_table`: IX position was overwritten rather than accumulated, producing curvature indices of 127 into a 96-entry table. Fix: `IXl = (current_offset + 0x40 + curvature_A) & 0xFF`. (`241b3da`)
- `draw_stretchy_object_common` (`$9237`, `LD A,(HL); DEC HL; DEC HL`): C had `doc.rows_2nd = width_bytes - 2` instead of stepping the pointer. For a 2-byte-wide masked bitmap this gave `doc.rows_2nd = 0`, looping `plot_masked_sprite` past the end of the bitmap array (ASan overflow). Fix: `doc.rows_2nd = *HLptr; HLptr -= 2;`. (`dd973cb`)

**Rule:** When the Z80 loads a value then adjusts the pointer, model it as `value = *ptr; ptr ±= N;` — never fold the adjustment into the value.

---

## 7. Macro used as expression, not statement

**Root cause:** A macro defined as a pure expression (`((base)[...])`) has no side effect; used as a statement, its result is discarded.

**Bug:** `WRAPPING(ptr, delta, base)` returned a new pointer but never assigned it back. All `WRAPPINGINCREMENT` calls in `build_height_table` were no-ops; `height_table[1..21]` was never filled, and `dr_read_lanes` received a garbage IX pointer (SIGSEGV).

**Fix:** Rewrite as a statement macro that assigns in place: `((ptr) = &(base)[...])`.

**Commit:** `3619da2`

---

## 8. Carry/borrow via bit 7 (sign flag) is unreliable — for both SUB and ADD

**Root cause:** The carry flag and the sign flag (bit 7 of the result) agree only when a subtraction or addition result fits in −128..+127. Outside that range they diverge, and either direction of mistranslation is common:

- **Subtraction (`SUB`/`CP`; carry = borrow):** `if (result & 0x80)` or `if ((s8)result < 0)` in place of the carry test fires incorrectly for differences in [128, 255], where carry is clear (no borrow) but bit 7 is set.
- **Addition (`ADD`; carry = overflow, `A+B > 255`):** translating `ADD A,B; RET C` as a magnitude comparison (`if (A < B) return`) tests something unrelated; translating `ADD A,C; JR C` as a sign test (`if ((s8)A < 0)`) fires for sums in [128,255] where carry is actually clear.
- A third, unrelated mistake with the same root (int never wraps): using the unsigned-wrap idiom `if (sum < addend)` to detect u8 overflow when `sum` is declared `int` — always false.

**Bugs:**

- `draw_road_lanes_change`: `range == 0 || (range & 0x80)` missed differences above 128.
- `animate_hero_car` lower-bound clamp: `if ((s8)(L - E) < 0)` fired for L=216, E=72 (216−72=144=0x90, carry=0/no borrow) since `(s8)0x90 = −112` — clamped the road position every frame, causing flicker.
- `advance_hazard` (`$ADC1–$ADC7`): subtraction result stored straight into a `u8` struct field, then tested via `(s8)` cast on the already-wrapped result. Fix: capture both operands before subtracting and compare directly: `if (A_old_frac < A_speed_lo) C_dist++;`.
- Two-exit variant — `draw_stretchy_object_common` (`$921F–$9222`): `JR Z,exit` (clamp to 1 if zero) chained with `JR NC,keep` (keep A if positive) after `SUB C` together mean clamp when result **≤ 0**. `if ((s8)Avertical < 0)` missed the zero case. Fix: `if (Avertical <= 0) Avertical = 1;`.
- `draw_object_right_stretchy_entrypt` (`$9306–$9307`, `ADD A,B; RET C`): `if (Awidth_bytes < Bdepth) return` dropped right-side scenery. Fix: `Awidth_bytes += Bdepth; if (Awidth_bytes > 255) return;`.
- `draw_dirt_and_stones` (`$AA33–$AA34`, `ADD A,E; RET NC`): first translated as `if (A >= E) return` (an unrelated magnitude test), then re-fixed as `A += E; if (A <= 255) return;` — but with `u8 A` the wrapped sum is always ≤ 255, so the draw call stayed unreachable. Fix: test in int width before the wrapping add: `if (A + E <= 255) return; A += E;`.
- `draw_overhead` (`$90D4–$90D5`, `ADD A,C; JR C`): sign test `if ((s8)A >= 0)` skipped the span-width computation for sums in [128,255].
- `advance_hazard` (`$ADCD–$ADD1`, `ADD A,C`): `C_dist` declared `int`, so `if (C_dist < IXhazard->distance)` was always false. Fix: `if (C_dist > 255)`.

**Rule:** For subtraction, use a direct unsigned comparison (`if (L < E)`) — never `(s8)` or `& 0x80` to recover carry. For addition, add first and check the sum against 255 (`if (A > 255)` / `if (A <= 255)`), never the inputs or the sign bit. When the accumulator is `int`, compare `sum > 255`, not the unsigned-wrap idiom. When the accumulator is `u8`, the wrapped sum can never exceed 255 — test the sum in int width (`if (A + E <= 255)`) before performing the wrapping add. When two conditional jumps follow one `SUB` (`JR Z` then `JR NC`), read them together — the combined condition is usually `<= 0`.

**Commits:** `0f95209`, `bc1e1cb`, `02d2a5a`, `9409d37`, `55be0c6`

---

## 9. Wrong constant / literal

**Root cause:** Z80 constants are hex; large values can be misread or mistranscribed.

**Bug:** `Ccounter` in `draw_road` initialised to `3` instead of `$60` (96).

**Commit:** `d6b91f9`

---

## 10. Boolean convention inverted

**Root cause:** Some draw functions use 0 = is-flipped / 1 = not-flipped (or vice versa); passing a raw multi-bit flags field where a strict boolean is expected inverts both branches.

**Bug:** `draw_object_930e_entrypt` passed `flags >> 1` as `zero_flipped`, taking every draw path in the wrong order. Fix: derive the boolean explicitly: `zero_flipped = (flags & MASK) ? 0 : 1`.

**Commit:** `c5c3e6c`

---

## 11. `>> 8` on a `u8` is always 0

**Root cause:** A `u8` promoted to `int` and shifted right 8 is always zero — the shift was meant to read a second packed byte.

**Bugs:**

- `plot_sprite_flipped_even`: `*src++ >> 8` always gave 0 for the second byte of a pair, rendering half the flipped pixels blank. Fix: two separate `flip_table[*src++]` calls.
- A packed-byte-pair variant: the Z80 reads two bytes with `LD B,(HL); INC HL; LD C,(HL)` into a register pair; the C port packed them into one `u8` and extracted the "high byte" with `>> 8`. Fix: read into two separate `u8` variables matching B and C. (`scroll_horizon`, `$B868–$B86A`)

**Commit:** `c5c3e6c`

---

## 12. Wrong array layout — `$E4xx` page

**Root cause:** The Z80 treats a 256-byte page as one flat space; splitting it into separately-based arrays breaks accesses that straddle the implicit boundary.

**Bug:** `edge_markings` was declared starting at `$E410` (offset 0x10) while all sites subtracted `$E400`; the max computed index (0xCC) overran the 192-byte array, and lane markings were a separate array needing different arithmetic.

**Fix:** One 256-byte array for the full `$E4xx` page (16 leading zeros + 192 edge bytes + 48 lane bytes); every site subtracts `$E400` uniformly.

**Commit:** `9998264`

---

## 13. `addr2xpos` even-byte masking hits the wrong byte lane

**Root cause:** `addr2xpos(state, z80addr)` computes `base[(z80addr & 0xFF) / 2]`, rounding odd low bytes down to the nearest even index — it returns a pointer to the even (position) byte of a D:E pair, not the odd byte HL actually pointed at.

**Bug:** In `dr_fill_left_stripe`, `Lrow` starts at `0xFF` and decrements by 2 (always odd); `addr2xpos` therefore dereferenced the position byte instead of the flag byte, so `if (*HL == 0)` never held and all markings were skipped. A secondary error: after `Lrow--`, the code re-read `*HL` for the position byte, but the correct byte is `HL[-1]`.

**Fix:** Use `hi2xpostab(state, H) + Lrow` for an exact byte pointer with no rounding; read `HL[-1]` after the decrement.

**Commit:** `fccab5a`

---

## 14. Hex loop bound misread — stale table slots accumulate via `+=`

**Root cause:** `LD B,$16` is 22 decimal, not 16 or 20. Under-counting a fill loop leaves trailing slots stale; if a later `+=` pass covers the full (larger) range every frame, the unfilled slots accumulate unboundedly, wrapping as `u8` over many frames.

**Bug:** `build_curve_table` had `B = 20` for `LD B,$16` (22 decimal); `curvature_table[20..21]` were never freshly written, so the delta-add pass grew `curvature_table[20]` by 3 every frame. Cycling every ~85 frames (~1.7s), this made the road near the horizon dance left-right visibly.

**Fix:** `B = 22`. Always convert hex loop counts to decimal explicitly, and note the source address in a comment.

**Commit:** `fe9323a`

---

## 15. Keydef array ordering — sentinel-shift reverses bit positions

**Root cause:** `keyscan_keydefs` accumulates key states into `Estopbit` via repeated `RL E`, starting from a single sentinel bit. After 8 shifts, `keydefs[0]` lands in bit 7 (first scanned) down to `keydefs[7]` in bit 0 (last scanned). `USERINPUTFLAG_*` follows `QPBFUDLR` (bit 7 = QUIT … bit 0 = RIGHT), so the keydefs array must be ordered QUIT-first to match.

**Bug:** keydefs were initialised RIGHT-first, QUIT-last — the exact opposite. Every key action mapped to the wrong input (P/RIGHT read as QUIT, 0/QUIT read as RIGHT, etc). Kempston mode was unaffected since it sets register bits directly. `ks_common` (`$A0FB`)'s `AND $03; CP $03` / `AND $0C; CP $0C` simultaneous-direction checks confirm the expected `USERINPUTFLAG_RIGHT=0x01 … UP=0x08` layout.

**Fix:** Store keydefs in descending flag order — QUIT at index 0 (bit 7) down to RIGHT at index 7 (bit 0). Use named physical-order constants (`KEYDEF_QUIT`, …), not `USERINPUT_*` bit-position values, as array indices. The same ordering governs Kempston's keyboard-only slots `keydefs[0..2]` (QUIT, PAUSE, TURBO → bits 7/6/5 after `RRCA×3 + AND $E0`).

---

## 16. u8 wraparound arithmetic — NEG, CPL, ADD, SUB, sign extension, and wrap-terminated loops

**Root cause:** Z80 8-bit arithmetic always wraps within the accumulator; a C `int` does not, so:

- `NEG` on `int` gives a large negative number instead of the wrapped positive Z80 result (Z80 `NEG` of 254 gives 2; C gives −254).
- `ADD`/`SUB` overflow does not wrap at 256.
- `CPL` complements 8 bits; C `~` complements the promoted `int`, so the result is negative (`~0x40` is −65, not `$BF`). Any comparison against a `u8` operand then succeeds unconditionally.
- The sign-extension idiom `if (val & 0x80) val |= 0xFF00;` only works when `val` is already a negative `int`; for a positive `int` with bit 7 set (e.g. 128) it produces 65408 instead of −128.
- A loop of the form `INC A; INC A; JP NZ` advances by 2 per iteration and terminates when A *wraps* from 254 to 0. Translating it as `Aiterations += 2; while (--Aiterations > 0)` gives a net step of +1, running roughly double the intended iterations and revisiting each slot.

**Bugs:**

- `update_road_level` stored a raw curvature byte (254 = signed −2) in a `u8`, read it back as `int` 254, and negated it with plain `-A`, giving −254 instead of 2 — a later `(s8)` cast then fired the 4.5× multiply on the wrong sign, jumping the car ~130 road-position units per frame on any curve. (`087c724`)
- `build_curve_table`'s sign extension used `|= 0xFF00` on a rounded multiply result that could be a positive `int` ≥ 128, producing a 65536-unit drift in the road-position accumulator.
- `build_curve_table_fill` (`bct_endbit_A`) masked `Atotal &= 0xFF` inside the inner loop but not after `Atotal -= Ldash` at the outer boundary, corrupting x-position table values when that subtraction went negative.
- `layout_road`'s forked-road path started `Aiterations = 0x30` with `+= 2` then `-- Aiterations > 0` (net +1 per iteration); it ran 207 iterations instead of 104, visiting each index twice and letting the second write silently overwrite the first.
- `draw_helicopter_part`'s `Abot` (models A after `ADD A,B`) was declared `int`; near a screen edge it reached up to 509 instead of wrapping mod 256, sending `draw_object_left_width_entrypt`'s `Awidth_bytes` branch down the wrong path and flipping the sprite to the opposite edge. Fix: declare `u8`. (`6ba1edc`)
- `advance_hazard`'s overtake gate (`$AE03–$AE0C`, `AND $E0; CPL; CP (IX+$04)`): `A_fc_inv = ~(state->fast_counter & 0xE0)` with `A_fc_inv` declared `int` yielded −225..−1, so `A_fc_inv < IX_hazard->dist_frac` was true for every `u8` `dist_frac`. Every traffic car retired the frame its distance reached 1 instead of passing the player, and the perp's `$AE20` store (`dist_frac = A_fc_inv`) took a truncated negative. Fix: `A_fc_inv = 0xFF ^ (state->fast_counter & 0xE0);`. Found with the `CHQ_HAZARD_TRACE` hazard-lifecycle log, which showed a retire at `t=88`/`frac=32` where the gate value is `$BF` = 191 and no retire was due.

**Fix:** Cast the result of any 8-bit-wrapping op back to `u8` immediately (`(u8)(-A)`, `(u8)(a+b)`, `(u8)(a-b)`, `0xFF ^ x` or `(u8)~x` for `CPL`); for sign extension, cast the low byte through `(s8)`/`(s16)` rather than OR-ing in `0xFFnn`; mask/cast accumulators back into range after every operation that could push them outside `[0,255]`, including at loop boundaries. For an `INC A;…;JP NZ` loop, drop the `--` from the condition entirely — `u8` wrap to 0 terminates it naturally: `do { … A += n; } while (A != 0);`.

**Commits:** `087c724`, `136e57d`

---

## 17. Stage array off-by-one — 0-indexed array with 1-indexed stage numbers

**Root cause:** The game uses stage numbers 1–5, but `stages[]` was declared with five entries indexed 0–4, so `stages[1]` loaded stage 2's data for stage 1, `stages[5]` (stage 5) and `stages[6]` (end-of-game reload) were out of bounds.

**Symptom:** Stage 1 opened with a four-lane road because it was actually running stage 2's map data.

**Fix:** Expand to seven entries: index 0 = `&stage1` (pregame), 1–5 = `&stage1`–`&stage5`, 6 = `&stage5` (end-sequence backstop). Whenever `load_stage` indexes `stages[wanted]`, confirm the maximum reachable `wanted` (including end-of-game transitions) stays within bounds.

**Related bug — table addressed via `base − 2` for 1-indexed access:** `car_jump_params` (`$B059`): the Z80 forms `HL = $B057 + 2A` — the table base minus 2, so that 1-indexed `A` lands on pair `A−1`. The C used `(Adiff * 2) − 1` (always odd, misaligned into the second half of each pair) instead of `(Adiff − 1) * 2`. Additionally `Adiff` can reach 6 (height byte −8 at top speed), where even the Z80 reads the two *code* bytes following the table at `$B063` — the C clamps to 5 (longest jump) with a `Conv:` note. When the skool loads a table pointer at `table ± k`, derive the C index from the computed landing offset, not from the loop variable's face value.

**Same pattern, single-entry stride:** `hazard_handler`'s slide-target lookup (`$A902–$A907`) bases HL at `hazard_pos_speed − 1` before adding the 1-based `current_lane`. The C indexed `hazard_pos_speed[current_lane]` directly — one column right of the intended lane — so a car clamped out of a 3-lane tunnel's rightmost slot still slid toward the 4-lane road's rightmost x-position. Fix: `hazard_pos_speed[current_lane - 1]`. (`2bf6d5e`)

---

## 18. `JR Z` / `JR NZ` / `RET Z` / `RET NZ` — branch polarity inverted

**Root cause:** `JR NZ,label` means *act if non-zero* — code that runs when the jump is skipped runs on the condition **opposite** to what the mnemonic suggests at a glance.

**Bugs:**

- `update_road_level`: `if (Ay_offset) { /* jump setup */ }` where the Z80 was `JR NZ,$B970` (skip setup if `mhc.y_offset != 0`) — the launch code ran only when the car was already airborne. Fix: `if (!Ay_offset) { … }`.
- `scroll_horizon` (`$B8A5–$B8A6`, `AND A; RET Z`): `if (Adiff) return` returned exactly when scrolling was due, making the function a no-op. Fix: `if (!Adiff) return;`.
- Three-way ladder collapsed to two-way — `layout_road` fork-side choice (`$BA7A–$BA88`): `DEC D; JP M,right; JP NZ,left; <D==1: E<12 test>`. The C handled only the `D==1` case and defaulted everything else to the right fork, but the Z80 takes the *left* fork for `D−1` in 1..0x7F. Every exit of a branch ladder needs its own C arm; do not fold the middle exits into the default. (`f47f822`)

**Rule:** `JR NZ → skip` = `if (reg == 0)` in C; `JR Z → skip` = `if (reg != 0)`. `RET Z` → `if (value == 0) return`; `RET NZ` → `if (value != 0) return`. The code that falls through should always be the "there is work to do" path.

**Commits:** `136e57d`, `41de175`

---

## 19. SBC carry chain — carry not propagated between chained subtractions

**Root cause:** `SBC HL,DE` uses carry as borrow input; when two `SBC`s appear in sequence, the first's carry output feeds the second's input. Writing the second subtraction as plain `-=` leaves `carry` stuck at the first test's result.

**Bug:** `handle_perp_caught` (`hpc_move_perp`) computed `carry = (HLroadpos < ROAD_LEFTMOST)` then did `HLroadpos -= ROAD_RIGHTMOST` without updating carry, so the LEFT input became unconditional and the centre-zone branch was dead code.

**Fix:** Recompute carry after every subtraction the Z80 models as `SBC`:

```c
HLroadpos -= ROAD_RIGHTMOST;
carry = (HLroadpos <= 0); /* Z80 SBC HL,DE with carry_in=1 */
```

**Commit:** `136e57d`

---

## 20. `JP M` / `JP P` as conditional skip — not a loop

**Root cause:** `JP M,addr` jumps forward when the Sign flag is set. Two `CALL`s separated by `JP M` is a conditional skip over the first `CALL`, not a backwards branch:

```
CALL first_routine      ; may be skipped
JP M, after_second      ; if negative, skip to after second
CALL second_routine     ; reached only when first was not skipped
after_second:
```

Translating this as a loop creates an infinite loop when the first call's row-count is 0.

**Bug:** `draw_object_clipped` (`$9404–$941D`): a `for(;;)` translation never terminated when `doc.rows_2nd == 0` (e.g. a 2-byte-wide bitmap), hanging `draw_scene_objects` on narrow stretchy segments. Fix: two straight-line call sites with an `if` guard over the first.

**Rule:** `CALL; JP M/P,skip_target; CALL` is two operations with a conditional skip, never a loop. Before writing a `Conv: NOT a loop` comment, grep the skool for every `JP`/`JR`/`DJNZ` targeting the block — one back-edge (e.g. `$9417 JP $9404` in the same function) makes it a genuine loop regardless of how sequential it looks.

**Commit:** `7e54fb2`

---

## 21. SP-based bulk fill/copy — `PUSH` fills backward, `POP` copies forward

**Root cause:** `PUSH` decrements SP by 2 before writing; `POP` reads then increments after. `LD SP,HL; PUSH BC × N` fills `2N` bytes backward from HL (at `HL−2N … HL−1`), not forward. `LD SP,HL; POP DE × N` reads `2N` bytes forward, verbatim.

**Bugs:**

- `ds_attributes` treats `horizon_attribute` as a pointer to the *last byte* of the current row and pushes backward 15 words, filling bytes 1–30. `memset(HLattrs, colour, 30)` (forward) spilled sky colour into the wrong rows. Fix: `memset(HLattrs - 30, colour, 30);`.
- `draw_tunnel` enters a fixed 16-slot `PUSH` chain via `JP (IX)` at a variable offset, filling `2*(16-start)` bytes backward. A 16-case switch works but is needless: `n = 16 - start; SPoutput -= n*2; memset(SPoutput, fill, n*2);`.
- `plot_sprite_even`/`plot_sprite_odd` copy sprite bytes verbatim via `LD SP,HL; POP DE` — a single `memcpy(dst, src, n)` replaces a byte-by-byte switch, valid only because no mask/flip transform is applied.

**Rule:** N `PUSH`es after `LD SP,HL` = `memset(ptr - 2*N, value, 2*N)` (backward, exclusive upper bound). N verbatim `POP`s = `memcpy(dst, ptr, 2*N)` (forward). Compute N from the jump-table index rather than unrolling a switch; flipped/masked sprites are not `memcpy` candidates.

---

## 22. Signed Z80 register used in arithmetic — cast to `(s8)` at the use site

**Root cause:** A field representing a signed offset (SM field, column adjustment) may be declared `u8` since it holds 0–255; using it in `ADD A,D` (signed addition) without a cast treats it as unsigned.

**Bug:** `draw_object_clipped`'s `D_col_pos` (self-modified signed column offset; 248 represents −8). `Adash_y_range += D_col_pos` as `u8` computed `+= 248` instead of `+= −8`, cascading into a wrapped `Adash_clip_rows`, an early exit, and an assert inside `plot_sprite_even`.

**Fix:** `Adash_y_range += (s8)D_col_pos;` — or declare the field `s8` outright. The bit-7 conditional idiom (`if (D & 0x80) …`) is error-prone; `(s8)` cast is always correct.

**Commit:** `944371a`

---

## 23. `EX AF,AF'` — wrong side receives the banked value, or restored flags misread

**Root cause:** `EX AF,AF'` swaps both A and F (all flags). Two related mistakes:

1. **Wrong variable assigned after unbank.** Inside a shuttle or loop, one C variable is accumulated in A while another is banked in A'. At the paired `EX AF,AF'`, A receives the banked value and A' receives what was live in A — capture the value banked at the *first* EX immediately there, not at the unbank point (which by then holds an unrelated value).
2. **Restored flags, not current A, drive the next branch.** `JP P`/`JP M` right after an `EX AF,AF'` tests the flags *restored* by that EX — set by whatever ran before the *original* bank — not any instruction that ran in between, even a `LD A,C` immediately before the branch.
3. **Wrong variable compared after unbank.** When two values ping-pong through A/A' across several branches, a `CP` after an odd number of swaps tests the *other* value. Count the EX instructions on the actual path to the comparison to know which side is live in A.

**Bugs:**

- `scroll_horizon` vertical-scroll loop: the final `EX AF,AF'` hands A (adjusted counter) to `var_a25b` and A' (accumulated delta) to `var_a25a`; the C update assignments were swapped.
- `draw_dirt_and_stones` (`$A9FF`): `EX AF,AF'` banks a table byte into A', then A is overwritten by an unrelated LOD index. The C code captured `saved_A = A` at the *unbank* comment (grabbing the always-non-negative LOD index) instead of the *bank* comment, making the intended `if (saved_A < 0)` branch permanently dead. Fix: assign at the bank point.
- `scroll_horizon` (`$B872`): the restored AF reflects `AND A` at `$B851` (sign of `current_curvature`); the following `LD A,C; JP P,$B879` (LD doesn't affect flags) branches on curvature's sign, not C's. The C code tested `(s8)Aregular < 0` (the always-non-negative table value C), so NEG was never applied and the backdrop could only scroll rightward. Fix: test `(s8)current_curvature < 0`.
- `draw_road_lanes_change` (`$C357`/`$C3CA`): the distance (IYl) and the masked lane flags shuttle through A/A'. The path to each far-boundary branch passes through `$C2EE` (bank distance), `$C30A`/`$C37E` (unbank distance for `CP $02`) and then a third EX at the branch itself, so the `CP $04` there tests the *masked lane flags*, not the distance. The C tested `A_dist != 4` — the correct `// EX AF,AF'` comments were present but the wrong variable was read — so the angled transition piece only drew when a slot happened to sit at distance 4, and the masked==8/12 classes ran the interpolation with the wrong reference height, smearing the verge across the road. Fix: test `Adash_masked_lane_flags != 4`.

**Rule:** Assign the shadow variable at the bank-point `EX AF,AF'`, never the unbank point. After a loop-terminating `EX AF,AF'`, trace which variable was live in A vs A' and assign each to the correct field. When `JP P`/`JP M` follows an `EX AF,AF'`, trace back to the flag-setting instruction before the *original* bank to find what's actually tested.

**Commits:** `41de175`, `55be0c6`, `87f70fa`, `a0e41fd`

---

## 24. Writing to the wrong struct field, or inventing an output parameter for a discarded result

**Root cause:** Two related destination mistakes when a function writes several fields of the same struct or passes a result back to its caller:

- **Wrong field, same struct:** a comment correctly names the intended field, but the assignment targets a different (already-existing, similarly-named) field on the same struct.
- **Invented output parameter:** when a `CALL`'s result is genuinely unused by the caller (skool: "result ignored"), but the callee happens to reload a same-named register internally for its own unrelated purpose, the C port wires that internal reload up to a fabricated output parameter — silently discarding whatever the caller actually passed in when it writes the "output" back.

**Bugs:**

- `advance_hazard` (`$AE74–$AE79`): `LD (IX+$02),L` / `LD (IX+$03),H` writes `horz_pos`/`horz_clip`, but the C port wrote the low byte into `distance` (offset 1) instead of `horz_pos` (offset 2), clobbering the distance value computed a few lines earlier. Since the depth-sorted draw list matches on `distance`, hazards were computed every frame but never drawn.
- `advance_hazard` (`$AE74–$AE79`, `check_collision` call): the skool marks the following `check_collision` call's result "ignored", but `check_collision` internally reloads `hazard->horz_pos`/`horz_clip` for its own bounding-box test, and the C port piped that reload through an `HLout` parameter the caller then wrote back into the hazard — pinning every hazard near its stale (often 0) previous position and preventing collisions.
- **Swapped call arguments:** `check_fork_scenery_collisions(state, DEdash, HLdash)` was called as `(state, HLdash_road_pos_a, DEdash_road_pos_b)`, crossing the pair over so the road clamp bounds `ahc_road_pos_a/b` stored swapped (min 472, max 72) while the fork was active; `animate_hero_car` then latched `road_pos` to alternating ends of the road every frame — a whole-screen two-frame flicker. When parameters are named after registers, check each call site against the *callee's* declared order, which need not match the caller's variable-declaration order. (`1c611dd`)

**Fix:** Cross-check the struct's field-offset table against the actual C member written at each site — don't trust the surrounding comment's prose alone; a field written correctly earlier in the function is a red flag if it reappears as an unrelated write target later. Before adding an output parameter, check every call site's skool for what happens to the register immediately after `CALL` returns — "result ignored" means don't invent one, even if the callee's internals happen to touch a same-named register. Match the skool ordering (write struct fields before the call) and drop the parameter once no caller needs it.

**Commits:** `02d2a5a`, `be0ef28`

---

## 25. Field decrement timing — write the wrapped result unconditionally, or don't write it at all

**Root cause:** Two opposite mistakes about *when* a Z80 subtraction writes back to the field it reads from:

- **Gating the store on the pre-subtraction value:** `SUB $01` always executes and stores its wrapped result before branching on carry. Pre-checking `if (value == 0)` and only decrementing a *different* (sibling) field in that branch leaves the original field stuck at 0 forever while the sibling underflows unboundedly every subsequent frame.
- **Storing when the Z80 never does:** `LD A,(addr); DEC A; RET Z` has no matching `LD (addr),A` — the Z80 only tests a transient copy. `if (--state->field == 0) …` performs a write the original never makes, permanently corrupting the field.

**Bugs:**

- `read_map`'s perp-distance countdown (`$C096–$C0B9`, `LD A,(IX+1); SUB $01; LD (IX+1),A; JR C`): the C port pre-checked `distance == 0` and decremented the high byte only in that branch, leaving `distance` pinned at 0 and the high byte free-running — eventually tripping an assert in `plot_turbos_and_digits`. Fix:

  ```c
  A_flags_inc = IX_hazard->distance;
  IX_hazard->distance = (u8)(A_flags_inc - 1);   /* SUB $01 always executes and stores */
  if (A_flags_inc == 0) {                        /* pre-subtract value was 0 -> this subtract borrowed */
      IX_hazard->hazard_lane_OR_perp_dist_hi--;
      continue;
  }
  ```

- `play_engine_or_turbo_sfx_128k` (`$F2FA–$F301`): the Z80 loads `turbo_sfx_pitch` into A, decrements the copy, and `RET Z`s on it — A is never stored back to `$A23A`; the field is only ever assigned elsewhere (`0x3C`, `0`, `1`). `if (--state->turbo_sfx_pitch == 0) return;` decremented the field itself every frame the turbo effect ran, terminating it on its own 59-frame schedule instead of being driven solely by `ay_noise_pitch`. Fix: test a copy without storing it back: `if ((u8)(state->turbo_sfx_pitch - 1) == 0) return;`.

**Rule:** Never gate a `SUB $01`/`DEC` on the pre-subtraction value to decide *whether* to store — always perform and store the wrap first, using the captured pre-value only for the borrow/zero branch. Conversely, before translating `LD A,(field); DEC A; RET Z/NZ` as `if (--state->field == 0)`, check the skool for a matching store back — no store means test a decremented copy only.

**Commits:** `507aa97`; (uncommitted — `play_engine_or_turbo_sfx_128k` fix)

---

## 26. Three compounding bugs, one symptom — always finish the audit even after finding a real one

**Root cause:** A single visible symptom ("objects lose rows from the top as they grow closer") had three independent causes in and around `draw_object_clipped`. Each one, found and fixed in isolation, was individually correct against the skool yet left the symptom visible, because the others were still active. Only auditing every candidate site (not stopping at the first confirmed bug) found the dominant cause.

**Bugs:**

- **`prev_buf_row()`** (`Main.c:595`) applied the back-buffer marker-nibble compensation (`backbuf += 0x1000`) unconditionally whenever the row-group field wrapped. The skool (`$B759–$B769`) only applies this compensation when the following `RRRC` subtraction does **not** borrow (`JR C,$B71C` skips it); when the row-group itself wraps past zero, the marker is deliberately left uncompensated. Fix: compute the subtraction first, then gate the `+= 0x1000` on `t >= 0`.
- **`D_col_pos` sign** (`doc_y_range_nonzero`, `$9359`): `Adash_y_range += D_col_pos` assumed `D_col_pos` is always `<= 0` (true historically, but `draw_hazard_sprites` sets it to `persp_col - hit_wobble`, which is commonly positive for cars/hazards). For positive values the Z80's `BIT 7,D; JR NZ` selects `SUB D`, not `ADD A,D` — the C's unconditional `+=` added instead of subtracted, inflating `y_range` and routing close objects into the wrong re-clip path. Fix: branch on sign explicitly (`D_col_pos < 0 ? += : -=`).
- **Row count from the wrong shadow register** (`doc_compute_bitmap`, `$93AE–$93B4`): the direct-dispatch plot calls (`plot_sprite`, `plot_sprite_flipped`, `plot_masked_sprite_by_width`, etc.) all read their row count from shadow `B'`, which is banked with `B_clip_rows` at `$93AE`/`$93B1` — never from `B_height`, which is popped back into main `BC` immediately after purely for the back-buffer address calculation. The C port instead used `min(B_height, B_clip_rows)`, correct only when `B_height > B_clip_rows`. Left-side callers (`draw_object_left_width_entrypt`) pass a fixed `B_height = 1`, so the clamp almost never fired and the left/verge path always drew 1 row regardless of the object's real clipped height — explaining both the "grows worse as the object grows" pattern and the left/right asymmetry (the right path's perspective-derived `B_height` happened to exceed `B_clip_rows` far less often, masking the bug there). Fix: `B_height = B_clip_rows;` unconditionally before the dispatch, keeping `Bdash_height` for the address calc — mirroring a fix already present in the neighbouring `doc_masked_rows` loop (`$945C`).

**Rule:** When a user-reported symptom persists after a fix that is individually verified correct against the skool, don't stop — the fix may be real but not sufficient. Audit every candidate computation feeding the same visible effect (source pointer, destination pointer, and row/height *count* can each carry an independent bug) before concluding the investigation. A left/right (or otherwise asymmetric) symptom is a strong clue: look for the one code path that is genuinely side-specific, not just the shared body both sides call into.

**Commit:** `c41e508`

---

## 27. Jump-table `JR` offsets can land inside a *later* PUSH chain — compute the landing address, don't assume the range

**Root cause:** A self-modified `JR` used as a jump table (`JR <offset>` where the offset selects how many `PUSH`es to skip) is not bounded by its own chain. The Z80 jump target is simply `PC + 2 + offset`, so a large offset sails past the end of the first chain, past any instructions between the chains and into the middle of the *next* one. The translation must be derived from the computed landing address of every offset the code can store, not from the assumption that offset N means "skip N of these 16 pushes".

**Bugs (`draw_tunnel`, `$C21F–$C2E1`, commit `e0cf3a9`):**

- `$C221 JR <D>` with `D = 22` (the `dt_max_fill` case) lands at `$C239` — midway into the *second* PUSH chain. The row is one combined 15-push (30-byte) fill from the original SP, and crucially the skipped instructions include `$C233 SUB C` and the second `LD SP,HL`, so the later `$C248 ADD A,C` nets `L += C` instead of restoring L. The C treated `D = 22` as "first fill empty" followed by a normal second fill — drawing little or nothing on exactly the widest rows of the tunnel mouth.
- The second-phase loop (`$C287`) and far-wall loop (`$C2C1`) look structurally identical to the first loop but contain **no jump table at all**: 15 unconditional `PUSH`es, a full-width fill every row. The C copied the first loop's `dt.fill_start_b` gating into both, drawing partial bands (or nothing when the stored index was 16).

**Rule:** For every value the code can store into a jump-table `JR` operand, compute the landing address by hand and read what actually executes from there — including which set-up instructions between chains get skipped. When several fill loops sit side by side, check each one for the presence or absence of its own jump table rather than assuming they share the first loop's shape.

**Commit:** `e0cf3a9`

---

## 28. `u16` state field holds a mod-65536 value that goes transiently negative — cast `(s16)` at every load

**Root cause:** Z80 16-bit arithmetic wraps mod 65536 and downstream `SBC`/comparisons interpret the value as signed. A C `u16` field wraps correctly on *store*, but loading it into a wider `int` loses the sign: −6 arrives as 65530. This is the 16-bit sibling of pitfall 16 — there the store-side wrap was missing; here the store is fine and the *load* side is wrong.

**Bugs:**

- During a road fork, `layout_road` temporarily sets `road_pos = road_pos ± fork_distance`, which legitimately goes negative once `fork_distance` (growing +16 per spawning frame) exceeds `road_pos`. `build_curve_table` reloaded the `u16` field as ~65530 and the geometry blew up — assert in `build_curve_table_fill` in debug builds, garbage road or a seize in release. This was the long-standing "forks seize the game up" bug. Fix: `(s16)` cast at all three `road_pos` load sites in `build_curve_table`.
- Same crash, contributing cause: the fork-distance delta at `$BAD0–$BADE` dropped the `AND $0F` (`$BAD7`), so the per-frame delta was −256..−1 instead of −16..−1, driving `fork_distance` negative far sooner.

**Rule:** When a `u16` state field models a Z80 register pair used in signed arithmetic, every load into a wider type needs `(s16)`, with a `Conv:` comment. Grep for other readers of the same field when adding the cast — one unsigned load re-introduces the bug.

**Commit:** `f47f822`

---

## 29. Pointer-to-array parameter indexed directly — steps whole tables, not elements

**Root cause:** Stage bitmap tables are declared `const bitmap_t (*x)[SPRITE_FRAMES]` — pointer to a whole 6-entry table. `x[A]` advances by A *tables* (A×6 entries); the A-th element is `(*x)[A]`. Both forms compile silently because both yield a `const bitmap_t *`.

**Bug:** `draw_dirt_and_stones` (`$AA13–$AA19`: `L = A*7; ADD HL,DE` — one 7-byte entry per LOD) used `DEbitmaps[A]`, reading up to 30 entries past `stage1_stones_bitmaps` (ASan: read just past `stage1_map_goto_table`). The garbage bitmap descriptor was handed to the sprite plotter, which scribbled 0xAA dither bytes over `road_buffer` — surfacing as the `draw_scene_objects` `Aobj=170` assert on dirt-track sections. Fix: `HLbitmap = &(*DEbitmaps)[A];`.

**Rule:** For any `(*ptr)[N]` parameter, element access is `(*ptr)[i]`; `ptr[i]` is a table stride. `draw_helicopter` still contains the suspect form (`helibitmap = *helibitmaps++;` next to a "can't be right" comment) — audit it against `$AA38` when helicopter data is completed.

**Commit:** `e457f6b`

---

## 30. Zone widths as `u8` differences — the Z80's fixed PUSH chains bound the damage; C arithmetic escapes it

**Root cause:** `draw_forked_road` derives five scanline fill widths as differences of cumulative zone positions (each 0..15). The Z80 emits each zone by a self-modified `JR` into its own fixed 15-`PUSH` chain (`$CA11–$CA65`), so each zone is structurally capped at 15 pairs and a crossed boundary (negative difference) at worst makes the `JR` skip past its chain — a bounded one-scanline glitch. The C `(u8)(a − b)` wrapped a crossed boundary to ~250 and the `memset`s ran hundreds of bytes backwards out of the backbuffer, overwriting `road_buffer` with the 0xAA stripe pattern.

**Bug:** On transient rows during dirt/fork transitions the zone positions cross (`pos_EA < pos_E8` etc.). Frame-deterministic corruption: `road_buffer` right-side object slots took the stripe fill byte, tripping the `draw_scene_objects` `Aobj <= 9` assert many frames later when the scan window reached the byte. Fix: compute the widths as `int` and fill with a 15-pair budget — each zone takes at most its non-negative width, right to left, and the lefthand verge takes the remainder, so the scanline is always exactly filled and the cursor cannot escape.

**Rule:** When the Z80's worst-case damage is bounded by *code structure* — fixed-length PUSH chains, page-wrapped `INC L`, self-modified low bytes — plain C arithmetic does not inherit the bound. Reproduce the structural cap explicitly (clamp, mask or skip) and say so in a `Conv:` comment. A wild write that stays inside `chqstate` is invisible to ASan; the road-buffer corruption diagnostics (`rm_prewrite`, `chq_test_max_side_object`) and a bisecting watchpoint found this one.

**Commits:** `e457f6b`, `c5b6347`

---

## 31. Chained self-modified sections — apply each section's `INC H` at its entry, with its exact count

**Root cause:** `draw_forked_road` walks the xpos pages twice per scanline: the fill boundary reads and the six marking sections both advance H by `INC H` at the *start* of each step, and the counts are irregular. Two opposite miscounts, months apart, in the same function:

- **Fill boundaries** (`$C973–$C9F6`): the Z80 walks `$E8`, `INC H ×2` → `$EA`, `INC H` → `$EB`, `INC H ×2` → `$ED` (left road, median, right road). The C read consecutive `$E8/$E9/$EA/$EC`, shaping every fork scanline from the wrong tables — striped garbage across the whole fork.
- **Marking sections** (`$CA90/$CAAD/$CACE/$CAF2/$CB0F`): each of sections 2–6 *begins* with a single `INC H`. The C incremented *after* each section, so every section from 2 on read the previous section's table: the lane dash drew at the left-edge position ("dashed centre line where the left edge should be"), the edge markings walked inward one boundary, and `$ED` — the right road's right edge — never received a marking ("no right edge, just ragged fill").

**Rule:** For a chain of self-modified sections, transcribe each section's exact `INC H` count at its entry point and verify every read's H page against the skool's address comments. Do not assume the pages are consecutive (the fill skips two) or that a shared increment can trail the section body (the markings advance before reading).

**Commits:** `c5b6347`, `a5f0989`

---

## 32. One C variable modelling both banks of a register — shadow-side mutations leak into the main side

**Root cause:** The Z80 banks L via `EXX`: `draw_forked_road`'s zone boundary reads run on a shadow L′ reloaded from main L every scanline pass (`$C96D LD A,L; EXX; LD L,A`), so boundary 4's `DEC L` (`$C9F0`) and the zero-fill path's `LD HL,$0000` (`$C937` — the PUSH fill *value*) touch only the shadow side. The C used a single `L` for both banks.

**Bugs:**

- The zero-fill path's `L = 0` zeroed the main row index, scrambling every zone and marking read for the rest of the frame.
- Boundary 4's `L--` leaked into main L the moment the fork-right table came on-screen, flipping the row parity for all subsequent rows: boundary reads then treated s16 low bytes as flag bytes, the zones collapsed to full-width verge and the road vanished towards the horizon.
- The first fix converted three of the four boundary reads to a distinct `Ldash`; the fourth survived because the conversion was applied by exact-string matching and that site's comment differed. Grep for every mutation of the shared name after such a conversion.

**Rule:** Give the banked side its own variable (`Ldash`, per the CLAUDE.md EXX convention), assigned at the bank point every pass, and afterwards audit every `L--`/`L -= n` in the function for which bank it belongs to.

**Commits:** `c5b6347`, `1c611dd`

---

## 33. `EX AF,AF'` banked in one function, read in another — the shadow value needs a persistent state field, not a local

**Root cause:** Section 23 covers `EX AF,AF'` mistranslated *within* one function's local shadow variable. Here the bank and the unbank are in two different functions, separated in time by a frame's worth of other calls. A Z80 shadow register survives across the whole intervening call chain (nothing else touches AF'); the C port has no equivalent unless the banked byte is stored in `chqstate_t` at the bank site and read back at the unbank site.

**Bug:** `move_hero_car` banks `BCcount_scaled` via `EX AF,AF'` at `$B296`; `scroll_horizon` unbanks it at `$B854` to pick one of four `horizon_table` rows for the curve x-scroll rate. The C treated the unbank site as if the value were unknowable (`Adash = 0`), reaching only two of the four table rows and producing a periodic stutter in curved-road scrolling. Fix: add `state->curvature_scroll_shadow`, written at the bank site and read at the unbank site — after confirming, from the skool, that no call on the path between the two (`spawn_cars`, `cycle_counters`, `play_engine_or_siren_sfx_hook`, `build_height_table`) executes `EX AF,AF'` itself.

**Rule:** Before writing off a cross-function shadow read as "uninitialised, approximate with 0", grep every call on the path between bank and unbank for `EX AF,AF'` — if none appears, the value is not approximate, it is the exact byte banked upstream, and needs a dedicated `chqstate_t` field to carry it.

**Commit:** `cf269e4`

---

## 34. `INC E` wraps only the low byte — a raw pointer `++` carries into the row and overruns the buffer

**Root cause:** `dr_fill_left_stripe`'s two-byte edge/lane writes are `LD (DE),A` then `INC E; LD (DE),A` in the Z80 — `INC E` wraps 0xFF to 0x00 with no carry into D, so the second byte always lands in the *same* backbuffer row, wrapping to column 0. A C `u8 *ptr; *ptr++ = …; *ptr = …` instead carries into the next row's memory (and, at the last row, off the end of `backbuffer[]`) whenever the column offset is 0xFF.

**Secondary bug, same commit:** `Ldash_backbuf` (the low byte of `DEdash_backbuf + 31`) was declared `int` instead of `u8`; the unmasked +31 column computation could then exceed 255 and corrupt the high byte (row) when recombined into a backbuffer address, rather than wrapping as the Z80's 8-bit `L'` would.

**Fix:** Recompute the second byte's address as `ADDRTOBACKBUF((DEdash_backbuf & 0xFF00) | ((Edash + 1) & 0xFF))` rather than incrementing the pointer, and declare any C variable standing in for an 8-bit Z80 register (`L'`, `E`, …) as `u8`, never `int`, even when it is only ever used as an offset.

**Rule:** Any Z80 `INC r` (single 8-bit register, not a register pair) that feeds a second memory access must be modelled as masked-low-byte arithmetic, not pointer increment — check whether the register is a pair (`INC DE`, carries) or a single register (`INC E`, wraps) before translating.

**Commit:** `275014e`

---

## 35. `XOR A; IN A,($FE)` is an any-key read, not a half-row read

**Root cause:** Keyboard reads select half-rows through the *high* byte of the port address. `LD A,$F7; IN A,($FE)` reads port `$F7FE` — the "1"-"5" row only. `XOR A; IN A,($FE)` reads port `$00FE`, which asserts all eight row-select lines at once and returns the AND of every row: any key on the keyboard. The two differ by one instruction and are often a few bytes apart in the same routine.

**Bug:** All three `XOR A; IN A,($FE)` sites in bank 3 (`$FC00` options-menu exit debounce, `$FECA` redefine-keys pre-capture debounce, `$FF02` test-mode confirmation screen) were translated as `port_KEYBOARD_12345`, copied from the genuine `LD A,$F7` poll at `$FBAE` a few lines above, `Conv:` comment and all. Symptom: the test-mode screen could only be dismissed with keys "1"-"5"; the original accepts any key. The equivalent 48K routines in the main binary (`$ED0B`, `$ED43`) were translated correctly first time — the main skool annotates them `; Read keyboard port $00FE`, whereas the bank-3 skool lines are bare.

**Fix:** `port_BORDER_EAR_MIC` (`$00FE`) for the all-rows read. `zx_in` in `libraries/ZXSpectrum/Spectrum.c` already implements it as the AND of all eight half-row queries.

**Rule:** Read the instruction that loads A, not the `IN` — `IN A,($FE)` alone tells you nothing about which keys are being sampled. When a routine polls both a specific row and "any key", expect them within a few bytes of each other, and do not carry the row constant (or its `Conv:` comment) across when transcribing the second one. An uncommented skool line is not a licence to pattern-match the neighbouring block.

**Commit:** "Fix: Accept any key where the Z80 reads every keyboard half-row"
