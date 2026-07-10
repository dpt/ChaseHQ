# Z80 → C Translation Pitfalls

Recurring mistakes encountered porting Chase H.Q. from Z80 to C. Each entry has: the root cause, the symptom, and the fix pattern.

---

## 1. Signed/unsigned type mismatch on local variables

**Root cause:** Z80 registers are 8-bit with no inherent sign; the same byte is used signed or unsigned depending on context. Defaulting to `u8` is wrong when subtraction can produce a negative result or when `JP M`/`JP P` branches on the sign flag.

**Symptoms:**

- `if (X < 0)` on a `u8` is always false — the negative-direction branch is dead code. (`L_step`, `A_step`, `A_delta_lo` in `draw_road_lanes_change`; `Anew_diff` in `dr_fill_left_stripe`.)
- `if (X > 0)` on a `u8` is true for all non-zero values — the negative branch is unreachable. (`Aheight_diff > 0` in `dr_fill_left_stripe`, fixed to `(s8)Aheight_diff > 0`.)
- `>= 128` boundary miss: `A > 128` fails for `A == 128` (0x80 has Sign flag set on Z80). Fix: `A >= 128`.
- **`s8` cast on a 16-bit `SBC HL,DE` result:** `SBC HL,DE` sets the Sign flag from bit 15 of the 16-bit result. Casting the `u16` result to `s8` tests bit 7 of the low byte only. A value like `0xFF64` is negative as `s16` but has zero in bit 7 of the low byte, so the `(s8)` branch fires incorrectly. Fix: cast to `s16`. (`f21a880`)

**Fix:** Declare the variable `s8` when the Z80 treats it as signed. Remove the now-redundant `(s8)` cast at comparison sites. If the value is set by subtraction that can underflow, it must be `s8`. For 16-bit arithmetic (`SBC HL,DE`, `ADD HL,DE`) the cast must be `s16`, not `s8`.

**Commits:** `c8251ba`, `6ce786f`, `4a9eb7f`, `2ab21904`, `f21a880`.

---

## 2. Stale register — wrong variable after register reuse

**Root cause:** Z80 reuses A (and other registers) for different values within one function. The C translation names them after the register, so the same name (`A`) can hold different logical values at different points. After a later use clobbers `A`, an earlier comparison that should reference the old value uses the new one instead.

**Symptom:** `dr_increasing` checked `if (A < 0x50)` but `A` had been overwritten by an xpos table read. Because xpos low bytes are 0x48–0x49 (always < 0x50), the backdrop branch never fired and `draw_road` looped forever.

A variant is **copy-paste between adjacent sections of the same function:** `rm_cycle_buffer_offset` handles curvature, height, lanes, etc. in parallel blocks. The HEIGHT section was copied from the CURVATURE section and the escape-byte check was not updated: `if (Amapcurvebyte == 0)` instead of `if (Aheight_byte == 0)`. The escape byte was never detected in the height stream, stalling `road_height_ptr` on the escape byte and corrupting the perspective table. (`f8241bf`)

**Fix:** Give each logical value its own named variable (`Aheight_diff`), not just the register name, and reference that at all sites where the original register is still in scope. After copy-pasting a parallel block, grep for every variable name and confirm each is the right one for the new context.

**Commits:** `8071d1f`, `f8241bf`

---

## 3. Missing or wrong initialisation

**Root cause:** The Z80 starts in a known state from ROM/BIOS or from preceding code. `chq_initialise()` must explicitly set every field that isn't zero, especially SM (self-modifying instruction) operand fields.

**Symptoms and fixes:**

- **SM fields never set:** `dr_left_table_hi_1/2`, `dr_right_table_hi_1/2`, `dr_neg_lane_count` — used on the first frame before `dr_four_lane_highway` writes them. Add explicit initialisation in `chq_initialise`. (`c8251ba`)
- **SM field type truncation:** `ahc_road_pos_b` was declared `u8` but the Z80 SM instruction at $B3A3 is `LD DE,$01D8`— a 16-bit operand. Storing 472 into a`u8`field silently truncates to 216;`DEother_road_pos >> 8`then returns 0 instead of 1, making the upper-bound high-byte comparison always pass for any road_pos ≥ 256. Fix: declare the field`u16`. Check whether the Z80 SM instruction loads a register pair (16-bit) or a single register/byte (8-bit). (`bc1e1cb`)
- **SM fields `ahc_road_pos_a` / `ahc_road_pos_b` not initialised:** The default `LD DE` operands at $B395 and $B3A3 are 72 and 472. calloc-zero left both at 0, causing the road_pos clamp to fire incorrectly on the very first frame. (`bc1e1cb`)
- **`road_buffer_offset` not reset in `set_up_stage`:** Z80 zeroes this at stage start; C translation missed it, leaving a stale pointer into the previous stage's buffer. (`dfdaf8b`)
- **`B_iterations` and `C_range` uninitialised in `draw_road_lanes_change`:** Z80 `LD B,C` sets loop count from C; both must be assigned from `C_bresen_range` at the `compute_step` entry point. (`7a12c5b`)
- **`C_bresen_range`/`B_tbl_stride` set from wrong variable:** Used `A_curve_step` (animation offset) instead of `IYheight[0] - ref_height` (height span). Bresenham range and table stride were wrong in all four setup paths. (`f98088b`)
- **`flipped[]` table not built:** `bootstrap()` builds the bit-reversal lookup; the `RUN_FULL_GAME=0` path bypasses bootstrap and went straight to `chq_initialise` without building the table. All flipped sprites rendered black. (`c5c3e6c`)
- **Stage not loaded before attract mode:** `state->stage` was NULL when `attract_mode_48k` first accessed it. (`5bd1c47`)

---

## 4. Pointer arithmetic: direction and carry logic

**Root cause:** Z80 PUSH decrements SP; the ZX Spectrum screen layout means "previous scanline" involves non-obvious byte arithmetic on D:E. Z80 `SUB $20` subtracts 32; `JR NC` skips the D adjustment on _no_ carry. C translations routinely get the sign or condition inverted.

**Specific bugs:**

- `dr_rollover_filled/unfilled`: `LO_ADD +32` should be `LO_ADD -32` (Z80 `SUB $20`). (`de88b6f`)
- `D += 16` condition inverted: Z80 adds 16 on no-carry (E≥32 before subtraction = result < 224); C was adding it on the carry branch. (`de88b6f`)
- `dr_rollover_unfilled`: the `HI_ADD` was missing entirely; the `dr_write_scanline_unfilled` call was accidentally inside the bare `if` body with no braces. (`de88b6f`)
- Read-before-decrement: Z80 `DEC D; LD A,D` reads D _after_ decrement; C read D before. With sentinel D=0x01 this skipped the rollover entirely. (`8449d0f`)
- `LO_ADD` macro computed but didn't assign back — was a pure expression used as a statement (no-op). (`8449d0f`)

---

## 5. Wrong pointer type → wrong element stride

**Root cause:** Z80 `LD A,(HL)` always loads one byte. If the C pointer is `u16*`, `ptr[offset]` treats a byte offset as a word index, doubling it and reading far out of bounds. The same problem applies to pointer increments: `INC SP` advances the stack pointer by exactly 1 byte, but `SP_output++` on a `u16*` advances by 2.

**Bugs:**

- `HLdash` in `dr_fill` was `u16*`; `HLdash[Ldash]` treated the byte offset 255 as a word index, reading element 255 of a 128-element array (into the adjacent `xpos_road_centre_left` table). Fix: `u8*`. (`6217000`)
- `SP_output++` for `INC SP` (`$C407`) advanced the output pointer by 2 bytes instead of 1 because `SP_output` is `u16*`. Fix: cast through `u8*`: `SP_output = (u16 *)((u8 *)SP_output + 1)`. (`f21a880`)

A third form: a `u8[]` table is indexed via `LD HL, table-1; ADD HL,BC` which gives a **byte** offset of `BC − 1` from the table base. The C translation sometimes halves this index (`&table[(BC - 1) / 2]`) under the incorrect assumption that the table holds 16-bit words. For a `u8` table the correct index is simply `BC - 1` with no division. (`scroll_horizon` vertical section, `$B898`)

**Fix:** Match the C pointer type to the Z80 access width. Byte loads/stores and single-byte SP adjustments require `u8*`; any `u16*` arithmetic silently doubles the offset. For `u8` tables accessed by `ADD HL,BC`, the C index is the byte offset with no scaling.

---

## 6. Accumulate vs. assign — ADD vs. LD

**Root cause:** Z80 `ADD A, IXl; LD IXl, A` accumulates into a register across iterations. The C translation sometimes uses plain assignment (`IXl = A`) which discards the running total.

**Bug:** `build_curve_table` IX position was overwritten rather than accumulated, producing curvature indices of 127 into a 96-entry table.

**Fix:** `IXl = (current_offset + 0x40 + curvature_A) & 0xFF` — keep the running offset.

**Commit:** `241b3da`

---

## 7. Macro used as expression, not statement

**Root cause:** A macro defined as a pure expression (`((base)[...])`) returns a value but has no side effect. If callers use it as a statement the value is discarded and nothing happens.

**Bug:** `WRAPPING(ptr, delta, base)` returned a new pointer but never assigned it back. All `WRAPPINGINCREMENT` calls in `build_height_table` were no-ops; `height_table[1..21]` was never filled; `dr_read_lanes` received a garbage IX pointer → SIGSEGV.

**Fix:** Rewrite as a statement macro that assigns in place: `((ptr) = &(base)[...])`.

**Commit:** `3619da2`

---

## 8. Carry/borrow via bit 7 (sign flag) is unreliable

**Root cause:** The Z80 pattern `LD A,X; SUB Y; JP C` (or `JR C`) detects borrow (X < Y) exactly via the carry flag. Translating `JP C` / `JR C` as `if (result & 0x80)` or `if ((s8)result < 0)` (sign flag) only works when `X - Y` fits in the −128..+127 range. Differences in [128, 255] have the carry clear (no borrow, X ≥ Y) but bit 7 set, so the branch fires incorrectly.

**Bugs:**

- `draw_road_lanes_change` used `range == 0 || (range & 0x80)` to detect that the setup path should be skipped. For differences above 128 the bit-7 check returned false incorrectly.
- `animate_hero_car` lower-bound clamp ($B3A0-$B3A1): `LD A,L; SUB E; JR C` was translated as `if ((s8)(L - E) < 0)`. For L=216, E=72: 216-72=144 = 0x90, carry=0 (no borrow, 216≥72), but (s8)0x90 = -112 triggers incorrectly. The road position 216 was clamped to 72 every frame, causing the road to flicker.
- `advance_hazard` (`$ADC1-$ADC7`): `LD A,(IX+$04); SUB (IX+$0D); LD (IX+$04),A; JR NC,$ADCD` subtracts the speed low byte from `dist_frac` and increments the distance accumulator (`C_dist`) on borrow. The C translation stored the subtraction result straight into the `u8` struct field, then tested `if ((s8) IXhazard->dist_frac < 0)` on the already-wrapped result — the same sign-bit-after-the-fact mistake, on a struct field this time rather than a local. Fixed by capturing both operands before the subtraction and comparing them directly: `if (A_old_frac < A_speed_lo) C_dist++;`.
- **Two-exit variant** — `draw_stretchy_object_common` `$921F–$9222` chains `JR Z,exit` (clamp to 1 if result==0) with `JR NC,keep` (keep A if positive) after a `SUB C`. Together they mean clamp to 1 when result **≤ 0**, not `< 0`. The C code `if ((s8) Avertical < 0) Avertical = 1` missed the zero case, leaving a sprite exactly filling the available height at height 0 instead of 1. Fix: `if (Avertical <= 0) Avertical = 1;`.

**Fix:** Use a direct unsigned comparison: `if (L < E)` (i.e. `if ((HLroad_pos & 0xFF) < (DEother_road_pos & 0xFF))`). Never use `(s8)` or `& 0x80` to recover a carry flag — the sign and carry flags from subtraction are the same only for differences in [0, 127]. This applies equally when the subtraction result is written straight into a struct field rather than held in a local: capture the pre-subtraction operands first if the comparison needs them. When two conditional jumps follow the same `SUB` (typically `JR Z` then `JR NC`), read both together — the combined condition is usually `<= 0`, not `< 0`.

**Commits:** `0f95209`, `bc1e1cb`, `02d2a5a`

---

## 9. Wrong constant / literal

**Root cause:** Z80 constants are written in hex; large hex values can be misread or mis-transcribed.

**Bug:** `Ccounter` in `draw_road` was initialised to `3` instead of `$60` (96). The values look nothing alike in decimal but could be confused if the skool annotation was misread.

**Commit:** `d6b91f9`

---

## 10. Boolean convention inverted

**Root cause:** Some draw functions use the convention 0 = is-flipped, 1 = not-flipped (or vice versa). Using a raw multi-bit flags field where a boolean 0/1 is expected inverts both branches.

**Bug:** `draw_object_930e_entrypt` passed `flags >> 1` as `zero_flipped` where the downstream function expected a strict 0-or-1 flag. All draw paths were taken in the wrong order.

**Fix:** Derive the boolean explicitly: `zero_flipped = (flags & MASK) ? 0 : 1`.

**Commit:** `c5c3e6c`

---

## 11. `>> 8` on a `u8` is always 0

**Root cause:** A `u8` value promoted to `int` shifted right 8 is zero for all 8-bit inputs. The shift was intended to read a second byte from a packed pixel pair.

**Bug:** `plot_sprite_flipped_even`: `*src++ >> 8` always gave 0 for the second byte of every pair, rendering half the flipped pixels blank.

**Fix:** Two separate `flip_table[*src++]` calls, matching the odd-width path.

A second form of the same mistake: the Z80 reads two consecutive bytes with `LD B,(HL); INC HL; LD C,(HL)` and packs them into a `u16` register pair. The C translation sometimes packs them into a single `u8` variable then extracts the "high byte" with `>> 8`. Fix: read the two bytes into two separate `u8` variables matching the Z80 B and C registers. (`scroll_horizon`, `$B868–$B86A`)

**Commit:** `c5c3e6c`

---

## 12. Wrong array layout — `$E4xx` page

**Root cause:** The Z80 treats a 256-byte memory page as one flat space. Splitting it into separate arrays with independent origins breaks accesses that straddle the implicit boundary.

**Bug:** `edge_markings` was declared starting at `$E410` (offset 0x10 into the page), but all access sites subtracted `$E400`. The minimum computed index was 0x10, and the maximum (with graphic offset 0xB0 and 7 xpos bits) was 0xCC = 204, overrunning the 192-byte array. Lane markings were a separate array, requiring different index arithmetic.

**Fix:** Declare one 256-byte array representing the full `$E4xx` page (16 leading zeros + 192 edge bytes + 48 lane bytes). All sites subtract `$E400` uniformly and index into this single array.

**Commit:** `9998264`

---

## 13. `addr2xpos` even-byte masking hits the wrong byte lane

**Root cause:** `addr2xpos(state, z80addr)` converts a Z80 address to a `u16*` by computing `base[(z80addr & 0xFF) / 2]`. The `/2` rounds odd low bytes down to the nearest even index, so the returned `u16*` points at the even byte — the position (E) byte of the D:E pair — not the odd byte that HL was actually pointing at.

**Bug:** In `dr_fill_left_stripe`, `Lrow` starts at `0xFF` and decrements by 2 (always odd). Using `addr2xpos(state, (H << 8) | Lrow)` to check the on-screen flag dereferenced the position byte instead of the flag byte. Every flag read returned a non-zero position value, so the `if (*HL == 0)` guard was never true and all edge and lane markings were silently skipped.

A secondary error in the same function: after `Lrow--` the code re-read `*HL` to get the position byte, but `HL` still pointed at the old odd offset. The position byte is one before that address, so the correct read is `HL[-1]`.

**Fix:** Use `hi2xpostab(state, H) + Lrow` to obtain an exact byte pointer. This gives a `u8*` at the precise odd offset Lrow without any rounding. For the position read after decrement, use `HL[-1]` not `*HL`.

**Commit:** `fccab5a`

---

## 14. Hex loop bound misread — stale table slots accumulate via `+=`

**Root cause:** Z80 loop counts are hex literals. `LD B,$16` is 22 decimal, not 16 or 20. When translated as `B = 20`, the fill loop writes too few entries. If a subsequent `+=` pass covers the full (larger) range every frame, the unfilled trailing slots accumulate the added value unboundedly. As a `u8` the slot cycles through 0–255 over many frames, producing slow oscillation in any output that depends on it.

**Bug:** `build_curve_table` had `B = 20` for `LD B,$16` (= 22 decimal, `$CC17`). `curvature_table[0..19]` was freshly written each frame; `curvature_table[20..21]` were stale. The left-side delta add pass ran over all 22 entries with `+=`, so `curvature_table[20]` grew by `persp_x_delta_left[row][20]` = 3 every frame. Cycling as `u8` every ~85 frames (~1.7 s at 50 fps), `build_curve_table_fill` read this cycling value for the last height segment, making the road near the horizon dance left-right visibly.

**Fix:** `B = 22`. Always convert hex loop-count literals to decimal explicitly and add `($CCxx LD B,$YY)` in the comment to make the source traceable.

**Commit:** `fe9323a`

---

## 15. Keydef array ordering — sentinel-shift reverses bit positions

**Root cause:** `keyscan_keydefs` uses a sentinel-bit trick: `Estopbit` starts with a single 1-bit and accumulates key states via repeated `RL E`. Each `RL` shifts the sentinel left and inserts the current key state at bit 0. After eight iterations (for `Estopbit = 0x01`) the sentinel exits through bit 7 into carry, stopping the loop. The result is:

```
bit 7 = keydefs[0]   (first scanned)
bit 6 = keydefs[1]
…
bit 0 = keydefs[7]   (last scanned)
```

The `USERINPUTFLAG_*` constants follow the `user_input` bit layout `QPBFUDLR` (bit 7 = QUIT, bit 0 = RIGHT). For the bit positions to match, the keydefs array must be stored in the same order: `keydefs[0]` = the key whose pressed state should appear in bit 7 (QUIT), down to `keydefs[7]` = RIGHT (bit 0).

**Symptom:** The keydefs were initialised with `keydefs[USERINPUT_RIGHT=0]` first and `keydefs[USERINPUT_QUIT=7]` last — the exact opposite of the `USERINPUTFLAG_*` expectations. In keyboard mode every key action was mapped to the wrong input: pressing P (RIGHT) set bit 7, read as QUIT; pressing 0 (QUIT) set bit 0, read as RIGHT; and so on for all eight keys. Kempston input was unaffected because it sets the Kempston register bits directly, already matching `USERINPUTFLAG_*` positions.

The Z80 post-processing (`ks_common` at `$A0FB`) confirms the expected layout: `AND $03; CP $03` checks bits 0+1 for simultaneous LEFT+RIGHT, and `AND $0C; CP $0C` checks bits 2+3 for simultaneous UP+DOWN — consistent with `USERINPUTFLAG_RIGHT=0x01`, `USERINPUTFLAG_LEFT=0x02`, `USERINPUTFLAG_DOWN=0x04`, `USERINPUTFLAG_UP=0x08`.

**Fix:** Store the keydefs in descending flag order — QUIT at index 0 (→ bit 7), down to RIGHT at index 7 (→ bit 0). Use named physical-order constants (`KEYDEF_QUIT`, …, `KEYDEF_RIGHT`) rather than `USERINPUT_*` values as array indices, since `USERINPUT_*` are bit positions, not keydef slots.

The same ordering is required in Kempston mode: `keyscan_keydefs` scans `keydefs[0..2]` for the three keyboard-only inputs. With the corrected order these are QUIT, PAUSE, TURBO — placed by the subsequent `RRCA×3 + AND $E0` post-processing into bits 7, 6, 5, matching `USERINPUTFLAG_QUIT/PAUSE/TURBO`.

---

## 16. u8 wraparound arithmetic — NEG, ADD, SUB, and sign extension

**Root cause:** Z80 8-bit arithmetic (`NEG`, `ADD A,n`, `SUB n`) and sign-extension (`H=0x00/0xFF` from bit 7 of L) always wrap/extend within an 8-bit accumulator. In C the variable is typically an `int`, so:

- `NEG` on `int` gives a large negative number instead of the wrapped positive Z80 result (e.g. Z80 `NEG` of 254 gives 2; C gives −254).
- `ADD`/`SUB` overflow does not wrap at 256 the way Z80 does.
- A common but wrong sign-extension idiom, `if (val & 0x80) val |= 0xFF00;`, only works when `val` is already a negative `int`. For a positive `int` with bit 7 set (e.g. 128), it produces `0xFF80 = 65408` instead of `−128` — a full 65536 error that propagates into any accumulator the value feeds.

If left unmasked/unextended, the error cascades into later subtraction, multiplication, or sign-flag tests fed by the same value.

**Bugs:**

- `update_road_level` stored the raw curvature byte (254 = signed −2) in a `u8` field, read it back as `int` 254, and negated it with plain `-Acurrent_curvature`, giving −254 instead of 2. A later subtraction and `(s8)` cast produced the wrong sign, so the 4.5× multiply fired on −255 instead of 1, jumping the car ~130 road-position units per frame on any curve. (`087c724`)
- `build_curve_table`'s sign extension used `|= 0xFF00` on a rounded multiply result that could be a positive `int` ≥ 128, producing a 65536 drift in the road-position accumulator every frame it fired.
- `build_curve_table_fill` (`bct_endbit_A`) masked `Atotal &= 0xFF` inside the inner loop but not after `Atotal -= Ldash` at the outer loop boundary; when that subtraction went negative, the next outer iteration needed fewer additions to trip the C overflow check than the Z80's wrapped byte would have, writing wrong x-position table values.

**Fix:** Cast the result of any 8-bit-wrapping op back to `u8` immediately: `result = (u8)(-A)`, `result = (u8)(a + b)`, `result = (u8)(a - b)`. For sign extension, cast the low byte through `(s8)` (or `(s16)` for a 16-bit source) rather than OR-ing in `0xFFnn`. Whenever a Z80 `SUB`/`ADD`/`NEG` result persists across loop iterations as an accumulator, mask or cast it back into range after every operation that could push it outside `[0, 255]` — a guard inside the loop body is not enough if a post-loop step can leave it out of range before the next iteration begins.

**Commit:** `087c724`

---

## 17. Stage array off-by-one — 0-indexed array with 1-indexed stage numbers

**Root cause:** The Z80 game uses stage numbers 1–5. The C `stages[]` lookup array was declared with five entries indexed 0–4 (`stages[0] = &stage1` … `stages[4] = &stage5`). The game sets `wanted_stage_number = 1` at startup and indexes directly with it, so `stages[1]` loaded stage 2's data for stage 1, every stage loaded the wrong data, stage 5 (`stages[5]`) was out of bounds, and the end-of-game reload (`wanted_stage_number = 6`, `stages[6]`) was also out of bounds.

**Symptom:** Stage 1 opened with a four-lane road because it was actually running stage 2's map data (`stage2_map_lanes` starts with `MAP_LANES_4(2)`). Stage 1's three-lane start (`MAP_LANES_3L(30)`) was never seen.

**Fix:** Expand `stages[]` to seven entries. Index 0 holds `&stage1` for the pregame path (`wanted_stage_number = 0`); indices 1–5 hold `&stage1`–`&stage5` for game stages 1–5; index 6 holds `&stage5` as a safe backstop for the brief end-of-game reload.

```c
const stage_t *stages[MAX_STAGES + 2] = {
    &stage1, /* [0]: pregame */
    &stage1, /* [1]: game stage 1 */
    &stage2, /* [2]: game stage 2 */
    ...
    &stage5, /* [6]: end-sequence */
};
```

**Check:** whenever `load_stage` indexes `stages[wanted]`, confirm that the maximum value `wanted` can reach (including end-of-game transitions) is within the declared array size.

---

## 18. `JR Z` / `JR NZ` / `RET Z` / `RET NZ` — branch polarity inverted

**Root cause:** `JR NZ, label` (or `RET NZ`) means _act if non-zero_ — the code that immediately follows a skipped jump, or the code that falls through a `RET`, runs on the **opposite** condition to what the mnemonic suggests at a glance. Translating `JR NZ` as `if (reg != 0) { ...code that should run on skip... }`, or `RET Z` as `if (value) return`, inverts the condition.

**Bugs:**

- `update_road_level` had `if (Ay_offset) { /* set up jump */ }` where the Z80 was `JR NZ,$B970` (skip jump setup if `mhc_y_offset != 0`). The jump launch code therefore ran only when the car was already airborne, so the car never launched off a road drop. Fix: `if (!Ay_offset) { /* set up jump */ }`.
- `scroll_horizon` at `$B8A5–$B8A6` does `AND A; RET Z` to return early when two counters are equal (no ticks elapsed). The C code `if (Adiff) return` returned whenever the counters _differed_ — exactly when the vertical scroll should run — making the function a no-op on every frame that scrolling was due. Fix: `if (!Adiff) return;`.

**Rule:** For every `JR Z`/`JR NZ`/`JP Z`/`JP NZ`, the C `if` guarding the fallthrough code uses the **opposite** polarity to the jump: `JR NZ → skip` = `if (reg == 0)` in C; `JR Z → skip` = `if (reg != 0)` in C. For `RET Z`/`RET NZ`, the same inversion applies to the early-exit guard: `RET Z` → `if (value == 0) return`; `RET NZ` → `if (value != 0) return`. Cross-check: the code that falls through should be the "there is work to do" path.

**Commits:** `136e57d`, `41de175`

---

## 19. SBC carry chain — carry not propagated between chained subtractions

**Root cause:** Z80 `SBC HL,DE` uses the carry flag as borrow input. When two `SBC` instructions appear in sequence, the carry output of the first feeds the carry input of the second. The C translation sets `carry` from the first subtraction, but if the second subtraction is written as plain `-=` the carry is never updated, so `if (carry)` after the second subtraction still reflects the first test.

**Bug:** `handle_perp_caught` (`hpc_move_perp`) computed `carry = (HLroadpos < ROAD_LEFTMOST)` then did `HLroadpos -= ROAD_RIGHTMOST` without updating carry. We only reach the second subtraction when `carry == 1` from the first test, so `if (carry)` after it was always true — the LEFT input was unconditional and the straight-ahead (centre zone) branch was dead code.

**Fix:** After any subtraction that the Z80 models as `SBC` with carry input, recompute carry from the result:

```c
HLroadpos -= ROAD_RIGHTMOST;
carry = (HLroadpos <= 0); /* Z80 SBC HL,DE with carry_in=1 */
```

**Rule:** Whenever two or more `SBC` instructions appear back-to-back in the Z80, the C translation must update `carry` between them. The carry variable is never live across C statements automatically.

**Commit:** `136e57d`

---

## 20. `INC A; INC A; JP NZ` loop — u8 wrap, not decrement

**Root cause:** The Z80 loop pattern `INC A; INC A; JP NZ` advances A by 2 each iteration and continues until A wraps from 254 to 256 = 0 (u8). Translating the loop as `while (--Aiterations > 0)` after `Aiterations += 2` gives a net step of +1 per iteration (not +2), running far more iterations than intended and revisiting each table slot multiple times.

**Bug:** `layout_road` forked-road path started `Aiterations = 0x30` and used `Aiterations >> 1` as the road-table index. With `+= 2` then `-- Aiterations > 0`, the net step was +1, so each index (24, 25, …) was visited twice. The loop ran 207 iterations instead of 104, and the second write of each slot used a different `*SProadright++` value, silently overwriting the first calculation.

**Fix:** Remove the `--` from the loop condition. Since `Aiterations` is `u8`, the wrap to 0 terminates the loop naturally:

```c
Aiterations += 2;
} while (Aiterations != 0); /* Z80: INC A; INC A; JP NZ — exits on u8 wrap */
```

**Rule:** Any Z80 `INC A; … JP NZ` (or `ADD A,n; JP NZ`) loop terminates when A wraps through 0. The C equivalent is `while ((u8)(A += n) != 0)` or a `do { ... A += n; } while (A != 0)` with `u8 A`. Never use `--` inside the condition of such a loop.

**Commit:** `136e57d`

---

## 21. Stale working register — `LD A,E` swap before computation

**Root cause:** The Z80 sometimes loads a register into A immediately before a computation to use its _old_ value, even though a newer value is also in scope. When the C translation sees both variables live at that point, it is easy to use the newer one by mistake — the C port just reads the name, not the timing.

**Bug:** `ds_attributes` (`update_screen`) loads `A = $E34C` (current delta) and `E = $E34D` (previous delta), then saves A to `$E34D`. The Z80 then does `LD A,E` at `$BD67` so that the rest of the block — sign extension, shift, pointer adjustment — all operate on the _previous_ delta. The C port kept `A` (current delta) as the working value throughout, silently using the wrong frame's data every time the block fired.

**Fix:** After the save (`state->horizon_attr[2] = A`), switch to `E` for all subsequent computation. Compute `D` from `E` first (before shifting it), then shift `E`:

```c
D = (E >= 64) ? 0xFF : 0x00;   /* sign from previous delta */
E = (E << 2);                    /* E = previous * 4 */
```

**Rule:** When the Z80 does `LD A,reg` at the start of a processing block — especially after both the old and new values of `reg` are in registers — find which logical value is needed for the computation (usually the _old_ one), and use that C variable, not the one that was most recently updated.

**Commit:** fix ds_attributes A-vs-E bug

---

## 22. SP-based bulk fill/copy — `PUSH` fills backward, `POP` copies forward

**Root cause:** `PUSH` decrements SP by 2 _before_ writing; `POP` reads 2 bytes then increments SP by 2 _after_. `LD SP,HL; PUSH BC × N` therefore fills `2N` bytes backward from HL (at `HL−2N … HL−1`), not forward. `LD SP,HL; POP DE × N` reads `2N` bytes forward from HL, verbatim, into the destination. Naive C translations get the fill direction wrong, or unroll the byte-copy loop as a repetitive switch instead of a `memcpy`/`memset`.

**Bugs:**

- `ds_attributes` maintains `horizon_attribute` as a pointer to the _last byte_ of the current sky/ground boundary row. The Z80 sets `SP` to that pointer and pushes 15 words backward, filling bytes 1–30 of the row. The C port called `memset(HLattrs, colour, 30)` (forward), spilling sky colour into the wrong attribute rows every frame the block fired. Fix: `memset(HLattrs - 30, colour, 30);` — the pointer is an exclusive upper bound.
- `draw_tunnel` uses `JP (IX)` to enter a fixed 16-slot `PUSH` chain at a variable offset, filling `2 * (16 − start)` bytes backward. A 16-case fall-through `switch` works but is needless; the count is `n = 16 - start`, so `SPoutput -= n * 2; memset(SPoutput, fill, n * 2);` replaces it entirely.
- `plot_sprite_even`/`plot_sprite_odd` copy sprite bytes verbatim via `LD SP,HL; POP DE` pairs. A fall-through switch copying byte-by-byte works but the byte count follows directly from the jump-table index, so a single `memcpy(dst, src, n)` replaces it — valid only because the bytes are copied verbatim with no mask or flip transform.

**Rule:** `LD SP,HL` followed by N `PUSH`es is `memset(ptr - 2*N, value, 2*N)` — backward from an exclusive upper-bound pointer. `LD SP,HL` followed by N `POP`s that write each byte verbatim is `memcpy(dst, ptr, 2*N)` — forward. Compute N from the jump-table entry index rather than unrolling a switch. Flipped or masked sprites are not candidates for `memcpy`: they apply a per-byte lookup/transform.

**Commits:** fix ds_attributes backward-fill bug (`draw_tunnel`, `plot_sprite_even`/`odd` simplifications)

---

## 23. `JP M` / `JP P` as conditional skip — not a loop

**Root cause:** `JP M, addr` jumps _forward_ when the Sign flag is set (result negative). When two CALL instructions are separated by a `JP M`, it is a conditional skip over the first CALL, not a backwards branch. The structure looks like:

```
CALL first_routine      ; may be skipped
JP M, after_second      ; if first's setup result was negative, skip to after second
CALL second_routine     ; reached only when first was not skipped
after_second:
```

Translating this as `for(;;)` (or any loop) creates an infinite loop when the first CALL's row-count is 0 — the `JP M` condition fires immediately and the loop never terminates.

**Bug:** `draw_object_clipped` at `$9404–$941D` has two sequential plot calls. When `doc_rows_main − BCpadding < 0`, `JP M,$941A` skips the first CALL and jumps to the exit sequence. The `for(;;)` translation never terminated when `doc_rows_2nd == 0` (e.g. a 2-byte-wide bitmap), hanging `draw_scene_objects` on any frame containing a telegraph pole or similar narrow stretchy segment.

**Fix:** Replace the loop with two straight-line call sites and an `if` guard over the first one:

```c
if (rows_adjusted >= 0) {
    plot_sprite_even(..., rows_main);
}
plot_sprite_even(..., rows_second);
```

**Rule:** When you see `CALL; JP M/P, skip_target; CALL`, it is two operations with a conditional skip, never a loop. Look for the forward address of the jump target to confirm the direction. Before writing any `Conv: NOT a loop` comment for a similar block, grep the skool for every `JP`/`JR`/`DJNZ` that targets an address inside it — a single back-edge (e.g. `draw_object_clipped` `$9417 JP $9404`) makes it a genuine loop regardless of how sequential the surrounding code looks.

**Commit:** `7e54fb2`

---

## 24. `DEC HL` after `LD A,(HL)` — pointer moves, value is unchanged

**Root cause:** Z80 `LD A,(HL)` reads the byte at HL into A; a following `DEC HL` (or `DEC HL; DEC HL`) moves the pointer backward. The value in A is not affected. The C equivalent is `A = *ptr; ptr -= N`. Writing `A = *ptr - N` or `*ptr -= N` instead modifies the _value_ and leaves the pointer unchanged.

**Bug:** `draw_stretchy_object_common` at `$9237` does `LD A,(HL); DEC HL; DEC HL` to read the `rows_2nd` field and step past it. The C translation had `doc_rows_2nd = width_bytes - 2` — subtracting 2 from the value instead of the pointer. For a `width_bytes == 2` masked bitmap (tree trunks), this produced `doc_rows_2nd = 0`, causing `plot_masked_sprite` to be called with `height=0` and loop indefinitely past the end of the bitmap array (ASan global-buffer-overflow at `ChaseHQ.c:11568 case 7`).

**Fix:**

```c
doc_rows_2nd = *HLptr;   /* LD A,(HL) */
HLptr -= 2;              /* DEC HL; DEC HL */
```

**Rule:** Whenever the Z80 does `LD A,(HL)` followed by `DEC HL` / `INC HL` (or any HL arithmetic), the value in A is frozen at the moment of the load. The subsequent HL adjustment is purely a pointer movement; model it as `ptr ±= N`, never as `value ±= N`.

**Commit:** `dd973cb`

---

## 25. Signed Z80 register used in arithmetic — cast to `(s8)` at the use site

**Root cause:** A Z80 register field annotated as a signed offset (SM field, column adjustment, etc.) may be declared `u8` in C because it can hold values 0–255. When the Z80 uses it in `ADD A,D` (a signed addition), the C `+= D_col_pos` treats it as unsigned, giving wildly wrong results for values ≥ 128.

**Symptom:** `draw_object_clipped` maintains `D_col_pos` as a self-modified signed column offset. `D_col_pos = 248` represents −8 (s8). In the `doc_y_range_nonzero` adjustment, `Adash_y_range += D_col_pos` with `D_col_pos` as `u8` computed `+= 248` instead of `+= −8`, giving `Diy_diff = 258` instead of 1. The too-large difference caused `Adash_clip_rows` to wrap to −256, which `(s8)`-cast to 0, triggering the early exit and producing `D_draw_height = −255`, which asserted inside `plot_sprite_even`.

**Fix:** Cast at the arithmetic site:

```c
Adash_y_range += (s8)D_col_pos;   /* Z80 ADD A,D — D is a signed offset */
```

Or declare the field `s8` if it is never used as unsigned.

**Rule:** Any Z80 SM field or register that represents a signed offset must be cast to `(s8)` (or declared `s8`) before use in C arithmetic. The bit-7 conditional approach (`if (D & 0x80) A -= (256 - D); else A += D`) is error-prone and verbose; `(s8)` cast is always correct.

**Commit:** `944371a`

---

## 26. Addition carry mistranslated — overflow guards, `(s8)` sign tests, and `int` accumulators

**Root cause:** After `ADD A,B`, carry means the 8-bit sum overflowed (`A + B > 255`) — a completely different condition from a magnitude comparison of the inputs, and unrelated to the sign flag (bit 7 of the result). Three related mistakes recur:

- Translating `ADD A,B; RET C` as `if (A < B) return` — a magnitude comparison, not an overflow check.
- Translating `ADD A,C; JR C` as `if ((s8) A < 0)` — testing the sign flag (bit 7) instead of carry. For sums in [128, 255], carry is clear (no overflow) but bit 7 is set, so the sign test fires when it shouldn't.
- Using the unsigned-wrap idiom `if (sum < addend)` to detect u8 overflow when `sum` is declared `int`. Since `int` never wraps, this is always false after a non-negative addition.

**Bugs:**

- `draw_object_right_stretchy_entrypt` (`$9306–$9307`, `ADD A,B; RET C`): C had `if (Awidth_bytes < Bdepth) return`, an unrelated magnitude guard. For near-horizon rows with small xpos and larger depth, this returned early and dropped right-side scenery objects, making the road surface visible through empty space. Fix: `Awidth_bytes += Bdepth; if (Awidth_bytes > 255) return;`.
- `dust_stones_stuff` (`$AA33–$AA34`, `ADD A,E; RET NC` — inverse form, returns when there is _no_ carry): C had `if (A >= E) return`, true for almost any positive A, making the draw call unreachable. Fix: `A += E; if (A <= 255) return;`.
- `draw_overhead` (`$90D4–$90D5`, `ADD A,C; JR C`): C used `if ((s8) A >= 0)` (sign test) instead of `if (A <= 255)` (carry test), skipping the span-width computation for sums in [128, 255] where the Z80 would have entered it.
- `advance_hazard` (`$ADCD–$ADD1`, `ADD A,C`): `C_dist` was declared `int`; `if (C_dist < IXhazard->distance)` was always false. Fix: `if (C_dist > 255)` — the `int` sum exceeds 255 exactly when the Z80 8-bit addition would have carried.

**Fix/Rule:** Add first, then check the _sum_, never the inputs or the sign bit: `A += B; if (A > 255) { /* carry branch */ }` or `if (A <= 255) { /* no-carry branch */ }`. When the accumulator is `int`, use `sum > 255` in place of the unsigned-wrap idiom `sum < addend`.

**Commits:** `9409d37`, `55be0c6`

---

## 27. `EX AF,AF'` — wrong side receives the banked value, or restored flags misread

**Root cause:** `EX AF,AF'` swaps both A and F (all flags) with their shadow counterparts. Two related mistakes:

1. **Wrong variable assigned after unbank.** Inside a shuttle or loop, one C variable is accumulated in A while another is banked in A'. At the paired `EX AF,AF'` that restores the main register, A receives the banked value and A' receives the value that was live in A — the two C variables must be assigned to the correct sides, not swapped, and not both captured at the unbank point (the value banked at the _first_ `EX AF,AF'` must be captured there, immediately after it is loaded — capturing it at the unbank point instead grabs whatever unrelated value A holds by then).
2. **Restored flags, not current A, drive the next branch.** A `JP P`/`JP M` immediately after an `EX AF,AF'` tests the flags restored by that EX — which reflect whatever instruction set them _before_ the original bank — not any instruction that ran between the EX and the branch. If a new value is loaded into A in between (`LD A,C; JP P`), the branch is independent of that value.

**Bugs:**

- `scroll_horizon` vertical-scroll loop: `EX AF,AF'` banks `Adiff` (ticks remaining) and unbanks `Ahorizon_y_a25a_delta` (accumulated delta). The final `EX AF,AF'` after the loop hands A (adjusted counter) to `var_a25b` and A' (accumulated delta) to `var_a25a`. The C update assignments were swapped (`horizon_y_accum += Ahorizon_y_a25a_delta` instead of `+= Bcounter`, and vice versa for `horizon_y_step`).
- `dust_stones_stuff` (`$A9FF`): `EX AF,AF'` banks a table byte into A', then A is overwritten by an unrelated LOD index computation. The C code captured `saved_A = A` at the _second_ (unbank) `EX AF,AF'` comment, grabbing the LOD index (always ≥ 0) instead of the table byte, making the intended `if (saved_A < 0)` branch dead code on every frame. Fix: assign `saved_A = A` at the _first_ (bank) comment, immediately after the value is loaded.
- `scroll_horizon` (`$B872`): the restored AF reflects `AND A` at `$B851` (sign of `current_curvature`). The immediately following `LD A,C; JP P,$B879` (`LD A,C` doesn't affect flags) branches on `current_curvature`'s sign, not C's. The C code tested `if ((s8) Aregular < 0)` (the table value C, always non-negative), so NEG was never applied and the backdrop could only scroll rightward. Fix: test `if ((s8) current_curvature < 0)` instead.

**Rule:** At a bank-point `EX AF,AF'`, assign the shadow C variable immediately, not at the unbank point. After a loop-terminating `EX AF,AF'`, trace which C variable was live in A vs A' and assign each to the correct state field. When `JP P`/`JP M` follows an `EX AF,AF'`, trace back to the flag-setting instruction (`AND A`, `OR A`, `CP`, or arithmetic) that preceded the _original_ bank to identify what is actually being tested — not the current A register.

**Commits:** `41de175`, `55be0c6`, `87f70fa`

---

## 28. `LD (IX+n),reg` — writing to the wrong struct field of the same object

**Root cause:** A function that writes several `IX+n` fields of the same struct in quick succession is easy to mistranslate if two of those fields hold conceptually related values (e.g. both are "positions"). The comment above the write may correctly identify the intended field, but the assignment targets a different one that happens to already exist on the struct and compile without complaint.

**Bug:** `advance_hazard` (`$AE74–$AE79`): `LD (IX+$02),L` / `LD (IX+$03),H` writes the `check_collision` result into `horz_pos` (offset 2) and `horz_clip` (offset 3). The C translation wrote the low byte into `IXhazard->distance` (offset 1) instead of `IXhazard->horz_pos`:

```c
IXhazard->distance  = HL & 0xFF;   /* wrong field: offset 1, not 2 */
IXhazard->horz_clip = HL >> 8;
```

This clobbered the distance value that had just been correctly accumulated a few lines earlier at `dh_adfa` (`IXhazard->distance = A_dist`), overwriting it every frame with the hazard's on-screen horizontal position. Since the depth-sorted draw list built immediately afterwards reads `IXhazard->distance` to place the hazard, the recorded distance was essentially garbage (frequently 0), and the draw loop — which matches depth exactly against `Biterations` counting 20 downto 1 — could never find the hazard at any depth. `draw_hazard_sprites` was entered but the depth-match test failed on every call, so hazards were computed every frame but never drawn.

**Fix:**

```c
IXhazard->horz_pos  = HL & 0xFF;   /* $AE74: IX[2] */
IXhazard->horz_clip = HL >> 8;     /* $AE77: IX[3] */
```

**Rule:** When translating consecutive `LD (IX+n),reg` writes, check the struct's field-offset table (declared IX+n comments elsewhere in the same function) against the actual C member name used at each write site — don't rely on the prose in the surrounding comment alone. A field that was written correctly by name in an _earlier_ part of the function (e.g. `distance` at `dh_adfa`) is a red flag if it reappears as the target of an unrelated write later in the same function; re-verify against the skool offset, not the variable's plausible-sounding name.

**Commit:** `02d2a5a`

---

## 29. Invented "output parameter" writes back stale state instead of passing the caller's value through

**Root cause:** When a Z80 `CALL` is followed by code that clearly doesn't use the returned register (the skool marks it "result ignored", or no subsequent instruction reads it), a C translation can still be tempted to give the callee an output parameter "for completeness" — especially if the callee happens to read and reconstruct that same register from struct fields internally, for its own unrelated purposes (e.g. a bounding-box test). If the C translation then writes that internally-reloaded value back through the invented output pointer, it silently discards whatever the caller actually passed in.

**Bug:** `advance_hazard` (`$AE74–$AE79`) writes the freshly-computed screen position into `horz_pos`/`horz_clip` and then calls `check_collision` (`$AE7A`) purely for its side effect (setting `hit_timer` on a hit). The skool comment at `$AE7A` reads "Call check_collision (result ignored)" — the Z80 caller never reads HL again. But `check_collision` internally reloads `hazard->horz_pos`/`horz_clip` (its _own_ current struct values, needed for its bounding-box overlap test) into local variables also named for HL's halves, and the C port wired those locals up to an `HLout` output parameter:

```c
if (HLout) *HLout = HL;                              /* caller's input, correct so far */
...
L_horz_pos  = hazard->horz_pos;                       /* reloaded for check_collision's own use */
H_horz_clip = hazard->horz_clip;
if (HLout) *HLout = (H_horz_clip << 8) | L_horz_pos;  /* overwrites with STALE struct state */
```

The caller then wrote this stale value back into the hazard: `IXhazard->horz_pos = HL & 0xFF;` — permanently pinning every hazard near its _previous_ (often template-default, i.e. 0) screen position regardless of the position just computed a few lines above. Symptom: hazard sprites collapsed to a sliver at screen x≈0, moved with distance/height as normal, and never triggered a collision (the hero's hit box at x∈[104,144] never reached x≈0).

**Fix:** Match the skool ordering — write the struct fields _before_ the call, and drop the output parameter entirely once no caller needs it:

```c
IXhazard->horz_pos  = HL & 0xFF;   /* $AE74: IX[2] — written before the CALL */
IXhazard->horz_clip = HL >> 8;     /* $AE77: IX[3] */
(void) check_collision(state, 0, HL, IXhazard);   /* result genuinely ignored */
```

**Rule:** Before adding an output parameter to a translated helper, check every call site's skool for what happens to the relevant register immediately after the `CALL` returns. "Result ignored" (or no subsequent read of that register before it's next written) means the C port should not invent one either — even if the callee's internal logic happens to touch a same-named register for its own purposes. An output parameter that exists only because _a_ register of that name is reloaded inside the callee, without confirming the _caller_ ever reads it back, is a fabricated data path that can overwrite a value the caller already computed correctly.

**Commit:** `be0ef28`

---

## 30. `SUB $01; JR C` pre-check instead of post-decrement — u8 never wraps

**Root cause:** Z80 `SUB $01` always executes and always stores its wrapped 8-bit result before branching on the carry flag it sets. A C translation that instead checks `if (value == 0)` _before_ decrementing, and only decrements a different field in that branch, never lets the original field wrap to 255 — it gets stuck at 0 forever, while the sibling field it decrements instead underflows unboundedly on every subsequent frame.

**Bug:** `read_map`'s perp-distance countdown (`$C096–$C0B9`) does `LD A,(IX+1); SUB $01; LD (IX+1),A; JR C,...`: the low byte is decremented and stored unconditionally, and the carry (0 → 255 wrap) selects whether the high byte is also decremented. The C translation instead pre-checked `if (distance == 0)` and, on that branch, decremented `hazard_lane_OR_perp_dist_hi` while leaving `distance` at 0 forever. Once `distance` first hit exactly 0 with a nonzero high byte, the high byte underflowed every frame thereafter, producing a runaway combined distance that tripped `assert(HLdistance >= 0 && HLdistance < 10000)` in `plot_turbos_and_digits`.

**Fix:**

```c
A_flags_inc = IX_hazard->distance;              /* value before the subtract, for the borrow test */
IX_hazard->distance = (u8)(A_flags_inc - 1);     /* SUB $01 always executes and stores, wrapping 0->255 */
if (A_flags_inc == 0) {                          /* pre-decrement value was 0 -> this subtract borrowed */
    IX_hazard->hazard_lane_OR_perp_dist_hi--;
    continue;
}
```

**Rule:** Never gate a Z80 `SUB $01`/`DEC` on the pre-subtraction value to decide _whether_ to store the wrapped result. Always perform and store the wrap first; use the captured pre-subtraction value only to detect the borrow/zero condition for the branch that follows.

**Commit:** `507aa97`
