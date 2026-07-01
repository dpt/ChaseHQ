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
- **SM field type truncation:** `ahc_road_pos_b` was declared `u8` but the Z80 SM instruction at $B3A3 is `LD DE,$01D8`— a 16-bit operand. Storing 472 into a`u8`field silently truncates to 216;`DEother_road_pos >> 8`then returns 0 instead of 1, making the upper-bound high-byte comparison always pass for any road_pos ≥ 256. The fix is to declare the field`u16`. Check: does the Z80 SM instruction load a register pair (16-bit operand) or a single register/byte (8-bit)? (`bc1e1cb`)
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

**Fix:** Match the C pointer type to the Z80 access width. Byte loads/stores and single-byte SP adjustments require `u8*`; any `u16*` arithmetic silently doubles the offset.

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

## 8. Borrow detection via bit 7 is unreliable

**Root cause:** The Z80 pattern `LD A,X; SUB Y; JP C` (or `JR C`) detects borrow (X < Y) exactly via the carry flag. Translating `JP C` / `JR C` as `if (result & 0x80)` or `if ((s8)result < 0)` (sign flag) only works when `X - Y` fits in the −128..+127 range. Differences in [128, 255] have the carry clear (no borrow, X ≥ Y) but bit 7 set, so the branch fires incorrectly.

**Bugs:**

- `draw_road_lanes_change` used `range == 0 || (range & 0x80)` to detect that the setup path should be skipped. For differences above 128 the bit-7 check returned false incorrectly.
- `animate_hero_car` lower-bound clamp ($B3A0-$B3A1): `LD A,L; SUB E; JR C` was translated as `if ((s8)(L - E) < 0)`. For L=216, E=72: 216-72=144 = 0x90, carry=0 (no borrow, 216≥72), but (s8)0x90 = -112 triggers incorrectly. The road position 216 was clamped to 72 every frame, causing the road to flicker.

**Fix:** Use a direct unsigned comparison: `if (L < E)` (i.e. `if ((HLroad_pos & 0xFF) < (DEother_road_pos & 0xFF))`). Never use `(s8)` or `& 0x80` to recover a carry flag — the sign and carry flags from subtraction are the same only for differences in [0, 127].

**Commits:** `0f95209`, `bc1e1cb`

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

## 11. `>> 8` on a `u8*` is always 0

**Root cause:** A `u8` value promoted to `int` shifted right 8 is zero for all 8-bit inputs. The shift was intended to read a second byte from a packed pixel pair.

**Bug:** `plot_sprite_flipped_even`: `*src++ >> 8` always gave 0 for the second byte of every pair, rendering half the flipped pixels blank.

**Fix:** Two separate `flip_table[*src++]` calls, matching the odd-width path.

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

**Symptom:** The keydefs were initialised with `keydefs[USERINPUT_RIGHT=0]` first and `keydefs[USERINPUT_QUIT=7]` last. This placed the RIGHT key at bit 7 of the result and QUIT at bit 0 — the exact opposite of the `USERINPUTFLAG_*` expectations. In keyboard mode every key action was mapped to the wrong input: pressing P (RIGHT) set bit 7, which the game read as QUIT; pressing 0 (QUIT) set bit 0, which the game read as RIGHT; and so on for all eight keys. Kempston (arrow-key/joystick) input was unaffected because it sets the Kempston register bits directly, which already match the `USERINPUTFLAG_*` positions.

The Z80 post-processing (`ks_common` at `$A0FB`) confirms the expected layout: `AND $03; CP $03` checks bits 0+1 for simultaneous LEFT+RIGHT, and `AND $0C; CP $0C` checks bits 2+3 for simultaneous UP+DOWN — consistent with `USERINPUTFLAG_RIGHT=0x01`, `USERINPUTFLAG_LEFT=0x02`, `USERINPUTFLAG_DOWN=0x04`, `USERINPUTFLAG_UP=0x08`.

**Fix:** Store the keydefs in descending flag order — QUIT at index 0 (→ bit 7), down to RIGHT at index 7 (→ bit 0). Use named physical-order constants (`KEYDEF_QUIT`, …, `KEYDEF_RIGHT`) rather than `USERINPUT_*` values as array indices, since `USERINPUT_*` are bit positions, not keydef slots.

The same ordering is required in Kempston mode: `keyscan_keydefs` scans `keydefs[0..2]` for the three keyboard-only inputs. With the corrected order these are QUIT, PAUSE, TURBO — placed by the subsequent `RRCA×3 + AND $E0` post-processing into bits 7, 6, 5, matching `USERINPUTFLAG_QUIT/PAUSE/TURBO`.

---

## 16. u8 arithmetic instructions — NEG, ADD, SUB results must stay u8

**Root cause:** Z80 arithmetic instructions (`NEG`, `ADD A,n`, `SUB n`, etc.) operate on the 8-bit accumulator; the result always wraps at 256. In C, the variable is typically an `int`, so the operation uses 32-bit arithmetic. For values ≥ 128, the C result and the Z80 result diverge:

- `NEG` of `u8` value 254: Z80 gives 2 (`256 − 254`). C gives `−254`.
- `ADD A, n` with overflow: Z80 wraps at 256; `int` does not.

If the large (or negative) C result is then used in further arithmetic — a subtraction, a multiply, a sign-flag test — the error cascades.

**Symptom:** `update_road_level` stored the raw curvature byte in `state->current_curvature` (a `u8`). The value 254 (`0xFE`, = signed −2) was read back as `int` 254 and negated with plain `−Acurrent_curvature`, giving −254. A later subtraction of `curvature_ticks` (1) gave −255. `(s8)(−255)` = 1, which is positive, so the 4.5× multiply fired on −255 instead of the correct 1, producing `horizontal_adjust ≈ −132` instead of −4. The car jumped ~130 road-position units per frame on any curve.

**Fix:** When the Z80 instruction is a pure 8-bit arithmetic op on a register that may hold values ≥ 128, cast the result back to `u8` before using it in C arithmetic:

```c
/* Z80: NEG ; A = -A (u8) */
Acurrent_curvature = (u8)(-Acurrent_curvature);
```

The same applies to `ADD` and `SUB` when overflow is expected: `result = (u8)(a + b)` or `result = (u8)(a - b)`. The `(s8)` sign-test at branch sites is unaffected — only the stored/accumulated value needs the cast.

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
