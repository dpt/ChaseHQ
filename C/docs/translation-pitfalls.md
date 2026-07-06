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

A second form of the same mistake: the Z80 reads two consecutive bytes with `LD B,(HL); INC HL; LD C,(HL)` and packs them into a `u16` register pair. The C translation sometimes packs them into a single `u8` variable then extracts the "high byte" with `>> 8`. Shifting a `u8` right 8 bits is always zero (the value fits in 8 bits). Fix: read the two bytes into two separate `u8` variables matching the Z80 B and C registers. (`scroll_horizon`, `$B868–$B86A`)

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

---

## 18. Partial sign extension with `|= 0xFF00`

**Root cause:** The Z80 sign-extends a byte to a 16-bit register pair by loading H with 0x00 or 0xFF depending on bit 7 of L. The C equivalent is sometimes written as `if (val & 0x80) val |= 0xFF00;`. This is only correct when `val` is already a negative `int` (bits 8–31 are already `0xFF…`). For a *positive* `int` with bit 7 set — e.g. the value 128 (`0x00000080`) — the OR produces `0x0000FF80 = 65408` instead of `−128`. The error is one full `u16` wrap (65536) and propagates into any accumulator the value is added to.

**Symptom:** In `build_curve_table`, the rounded multiply result (`HLdash_multiplied`) could land at 128 or above after the rounding step (`>> 8` + carry bit). The `|= 0xFF00` sign extension then produced 65408 for value 128, 65409 for 129, etc. `DEdash_roadposacc += 65408` drifted by 65536 relative to the Z80 road-position accumulator, corrupting all subsequent curvature table entries for that frame.

**Fix:** Use a `(s8)` cast on the low byte rather than a conditional OR:

```c
A_curvature       = HLdash_multiplied & 0xFF;
HLdash_multiplied = (s8) A_curvature; /* sign extend: Z80 DEC H / RRA */
```

`(s8)` maps 0–127 → 0–127 and 128–255 → −128–−1, exactly matching the Z80 `{H=0x00 or 0xFF, L=A}` construction, regardless of the sign of the `int` being extended.

**Rule:** Never use `|= 0xFFnn` to sign-extend a value that might be a positive `int`. Always cast through `(s8)` (8-bit source) or `(s16)` (16-bit source).

---

## 19. 8-bit subtraction result not masked back to `u8`

**Root cause:** Z80 `SUB n` operates entirely in the 8-bit accumulator; the result always wraps at 256. In C the variable is an `int`, so the subtraction is 32-bit and can go negative. If the result is then used in a subsequent iteration of a loop — rather than being consumed and discarded — the negative C value diverges from the Z80's wrapped-positive value on every future accumulation step.

**Symptom:** In `build_curve_table_fill` (`bct_endbit_A`), `Atotal &= 0xFF` was applied inside the inner loop but not after `Atotal -= Ldash` at the outer loop boundary. When the inner loop exited via overflow (carry set) and the masked `Atotal` was less than `Ldash`, the subtraction produced a negative `int`. On the next outer iteration, `Atotal += Cdash` needed fewer additions before the C overflow check (`Atotal > 0xFF`) fired, compared to the Z80's addition on the wrapped positive byte. This caused one fewer or more DE increment before each PUSH, writing wrong x-position values into the road table.

**Fix:** Apply `& 0xFF` immediately after any subtraction whose result feeds back into 8-bit accumulation:

```c
Atotal -= Ldash;
Atotal &= 0xFF; /* Z80 SUB L wraps; without mask Atotal goes negative */
```

**Rule:** Whenever a Z80 `SUB` (or `ADD`, `NEG`) result persists across loop iterations as an 8-bit accumulator, mask it back to `[0, 255]` after every operation that could leave it outside that range. The guard inside the loop body is not sufficient if a post-loop subtraction can make the value negative before the next iteration begins.

---

## 20. `JR Z` / `JR NZ` branch direction inverted

**Root cause:** `JR NZ, label` means *skip to label if non-zero* — the code that immediately follows the jump runs when the register **is** zero. Translating this as `if (reg != 0) { ... }` puts the code inside the block when the register is non-zero, exactly backwards.

**Bug:** `update_road_level` had `if (Ay_offset) { /* set up jump */ }` where the Z80 was `JR NZ,$B970` (skip jump setup if `mhc_y_offset != 0`). The jump launch code therefore ran only when the car was already airborne and was dead on the ground, so the car never launched off a road drop.

**Fix:** `if (!Ay_offset) { /* set up jump */ }` — enter the block when the tested register is zero, matching `JR NZ → skip`.

**Rule:** For every `JR Z` / `JR NZ` / `JP Z` / `JP NZ` near a translated block, identify what value the register holds at that point and confirm the C `if` condition fires on the **opposite** polarity to the Z80 jump. `JR NZ → skip` = `if (reg == 0)` in C. `JR Z → skip` = `if (reg != 0)` in C.

**Commit:** `136e57d`

---

## 21. SBC carry chain — carry not propagated between chained subtractions

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

## 22. `INC A; INC A; JP NZ` loop — u8 wrap, not decrement

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

## 23. Stale working register — `LD A,E` swap before computation

**Root cause:** The Z80 sometimes loads a register into A immediately before a computation to use its *old* value, even though a newer value is also in scope. When the C translation sees both variables live at that point, it is easy to use the newer one by mistake — the C port just reads the name, not the timing.

**Bug:** `ds_attributes` (`update_screen`) loads `A = $E34C` (current delta) and `E = $E34D` (previous delta), then saves A to `$E34D`. The Z80 then does `LD A,E` at `$BD67` so that the rest of the block — the sign extension (`SBC A,A`), the shift (`ADD A,A; ADD A,A`), and the pointer adjustment — all operate on the *previous* delta. The C port kept `A` (current delta) as the working value throughout the block, silently using the wrong frame's data for every frame that the block fired.

**Fix:** After the save (`state->horizon_attr[2] = A`), switch to `E` for all subsequent computation inside the `if (E != 0)` block. Compute `D` from `E` first (before shifting it), then shift `E`:

```c
D = (E >= 64) ? 0xFF : 0x00;   /* sign from previous delta */
E = (E << 2);                    /* E = previous * 4 */
```

**Rule:** When the Z80 does `LD A,reg` at the start of a processing block — especially after both the old and new values of `reg` are in registers — find which logical value is needed for the computation (usually the *old* one), and use that C variable, not the one that was most recently updated.

**Commit:** fix ds_attributes A-vs-E bug

---

## 24. `LD SP,HL; PUSH × N` fills backward, not forward

**Root cause:** The Z80 `PUSH` instruction decrements SP by 2 *before* writing. So `LD SP,HL; PUSH BC × 15` fills 30 bytes at addresses `HL−30 … HL−1` (backward from HL, exclusive). In C, `memset(ptr, colour, 30)` fills forward from `ptr` to `ptr+29` — exactly the wrong direction when `ptr` is an end-of-row pointer.

**Bug:** `ds_attributes` (`update_screen`) maintains `horizon_attribute` as a Z80 address pointing to the *last byte* of the current sky/ground boundary row (e.g. `$59BF` = byte 31 of attribute row 13). The Z80 sets `SP = $A186` (that pointer) and pushes 15 words backward, filling bytes 1–30 of the row. The C port called `memset(HLattrs, colour, 30)` which wrote byte 31 of that row and bytes 0–28 of the *next* row, spilling sky colour into the wrong attribute rows every frame the block fired.

**Fix:** Shift the `memset` start back by the fill length:

```c
memset(HLattrs - 30, colour, 30);   /* backward fill from end-of-row pointer */
```

**Rule:** Whenever the Z80 does `LD SP,HL` followed by N `PUSH` instructions to fill memory, the C equivalent is `memset(ptr - 2*N, value, 2*N)`. The pointer is an *exclusive upper bound*, not the start of the region. If the pointer is an end-of-line attribute pointer (pointing at byte 31 of a 32-byte row), the fill covers bytes 1–30, leaving bytes 0 and 31 untouched — match that in C.

**Commit:** fix ds_attributes backward-fill bug

---

## 25. `JP M` / `JP P` as conditional skip — not a loop

**Root cause:** `JP M, addr` jumps *forward* when the Sign flag is set (result negative). When two CALL instructions are separated by a `JP M`, it is a conditional skip over the first CALL, not a backwards branch. The structure looks like:

```
CALL first_routine      ; may be skipped
JP M, after_second      ; if first's setup result was negative, skip to after second
CALL second_routine     ; reached only when first was not skipped
after_second:
```

Translating this as `for(;;)` (or any loop) creates an infinite loop when the first CALL's row-count is 0 — the `JP M` condition fires immediately and the loop never terminates.

**Bug:** `draw_object_clipped` at `$9404–$941D` has two sequential plot calls. When `doc_rows_main − BCpadding < 0`, `JP M,$941A` skips the first CALL and jumps to the `ADD A,B` / `B=A` / jump-dispatch sequence. The `for(;;)` translation never terminated when `doc_rows_2nd == 0` (e.g. a 2-byte-wide bitmap), hanging `draw_scene_objects` on any frame containing a telegraph pole or similar narrow stretchy segment.

**Fix:** Replace the loop with two straight-line call sites and an `if` guard over the first one:

```c
if (rows_adjusted >= 0) {
    plot_sprite_even(..., rows_main);
}
plot_sprite_even(..., rows_second);
```

**Rule:** When you see `CALL; JP M/P, skip_target; CALL`, it is two operations with a conditional skip, never a loop. Look for the forward address of the jump target to confirm the direction.

**Commit:** `7e54fb2`

---

## 26. `DEC HL` after `LD A,(HL)` — pointer moves, value is unchanged

**Root cause:** Z80 `LD A,(HL)` reads the byte at HL into A; a following `DEC HL` (or `DEC HL; DEC HL`) moves the pointer backward. The value in A is not affected. The C equivalent is `A = *ptr; ptr -= N`. Writing `A = *ptr - N` or `*ptr -= N` instead modifies the *value* and leaves the pointer unchanged.

**Bug:** `draw_stretchy_object_common` at `$9237` does `LD A,(HL); DEC HL; DEC HL` to read the `rows_2nd` field and step past it. The C translation had `doc_rows_2nd = width_bytes - 2` — subtracting 2 from the value instead of the pointer. For a `width_bytes == 2` masked bitmap (tree trunks), this produced `doc_rows_2nd = 0`, causing `plot_masked_sprite` to be called with `height=0` and loop indefinitely past the end of the bitmap array (ASan global-buffer-overflow at `ChaseHQ.c:11568 case 7`).

**Fix:**

```c
doc_rows_2nd = *HLptr;   /* LD A,(HL) */
HLptr -= 2;              /* DEC HL; DEC HL */
```

**Rule:** Whenever the Z80 does `LD A,(HL)` followed by `DEC HL` / `INC HL` (or any HL arithmetic), the value in A is frozen at the moment of the load. The subsequent HL adjustment is purely a pointer movement; model it as `ptr ±= N`, never as `value ±= N`.

**Commit:** `dd973cb`

---

## 27. Signed Z80 register used in arithmetic — cast to `(s8)` at the use site

**Root cause:** A Z80 register field annotated as a signed offset (SM field, column adjustment, etc.) may be declared `u8` in C because it can hold values 0–255. When the Z80 uses it in `ADD A,D` (a signed addition), the C `+= D_col_pos` treats it as unsigned, giving wildly wrong results for values ≥ 128.

**Symptom:** `draw_object_clipped` maintains `D_col_pos` as a self-modified signed column offset. `D_col_pos = 248` represents −8 (s8). In the `doc_y_range_nonzero` adjustment, `Adash_y_range += D_col_pos` with `D_col_pos` as `u8` computed `+= 248` instead of `+= −8`, giving `Diy_diff = 258` instead of 1. The too-large difference caused `Adash_clip_rows` to wrap to −256, which `(s8)`-cast to 0, triggering the early exit and producing `D_draw_height = −255`, which asserted inside `plot_sprite_even`.

**Fix:** Cast at the arithmetic site:

```c
Adash_y_range += (s8)D_col_pos;   /* Z80 ADD A,D — D is a signed offset */
```

Or declare the field `s8` if it is never used as unsigned.

**Rule:** Any Z80 SM field or register that represents a signed offset must be cast to `(s8)` (or declared `s8`) before use in C arithmetic. The bit-7 conditional approach (`if (D & 0x80) A -= (256 - D); else A += D`) is error-prone and verbose; `(s8)` cast is always correct.

**Commits:** `944371a`

---

## 28. `ADD A,B; RET C` — carry means u8 overflow, not `A < B`

**Root cause:** The Z80 `ADD A,B` instruction sets carry when the 8-bit result overflows (`A + B > 255`). `RET C` then returns on that overflow. The C translation sometimes replaces this with `if (A < B) return` — a comparison that fires when A is less than B, which is a completely different (and almost opposite) condition for typical small positive B values.

**Bug:** `draw_object_right_stretchy_entrypt` at `$9306–$9307` does `ADD A,B; RET C` to skip drawing right-side objects when the road edge plus the depth offset wraps past 255 (i.e., the object is off-screen right). The C translation had `if (Awidth_bytes < Bdepth) return`, which fired when the x-position was *less than* the depth (a completely different guard). For near-horizon rows where `xpos_road_centre[k]` is small (e.g. 5–35) and depth is 16–36, the wrong guard returned early, silently dropping those objects. On 3-lane sections where small xpos values occur most often, entire rows of right-side scenery vanished, making the road surface visible through the empty space — appearing as if objects were intruding into the road.

**Fix:**

```c
} else {
    Awidth_bytes += Bdepth;         /* $9306: ADD A,B */
    if (Awidth_bytes > 255) return; /* $9307: RET C — u8 overflow */
}
```

**Rule:** `ADD A,B; RET C` is an overflow guard, not a magnitude comparison. Translate it as: add first, then check if the result exceeds 255. The condition `A < B` (which checks whether the *inputs* have a certain order) is unrelated to carry from addition.

The inverse form, `ADD A,B; RET NC`, returns when there is **no** carry (sum ≤ 255) and falls through only on overflow. C equivalent: `A += B; if (A <= 255) return;`. Do not use `if (A >= B) return` — that fires on a magnitude comparison and is almost always true for any positive A.

**Bug (RET NC):** `dust_stones_stuff` at `$AA33–$AA34` does `ADD A,E; RET NC` to skip the left-helicopter draw when the x-position plus pixel-width does not overflow (object is not wrapping). The C code `if (A >= E) return` checked whether the sum was at least as large as the pixel-width addend — true for almost any non-zero A — making the draw call unreachable.

**Commits:** `9409d37`, `55be0c6`

---

## 29. `RET Z` / `RET NZ` early exit — polarity is the inverse of the fallthrough code

**Root cause:** `RET Z` returns *when the tested register is zero*. Translating this as `if (value) return` inverts the condition: the function now returns when the value is non-zero (i.e., when there is work to do) and only falls through when the value is zero (when there is nothing to do).

**Bug:** `scroll_horizon` at `$B8A5–$B8A6` does `AND A; RET Z` to return early when `fast_counter − horizon_y_step == 0` (no ticks have elapsed). The C code `if (Adiff) return` returned whenever the two counters *differed*, which is exactly when the vertical scroll should run. The function was therefore a no-op every frame that any scrolling was due, and only fell through (to do nothing meaningful) on the rare frame when the counters happened to be equal.

**Fix:** `if (!Adiff) return;`

**Rule:** For every `RET Z` / `RET NZ` (or `JP Z` / `JR Z`), the C `if` must use the **opposite** polarity to the Z80 instruction. `RET Z` → `if (value == 0) return` (or `if (!value) return`). `RET NZ` → `if (value != 0) return`. Cross-check: the fallthrough code should be the "there is work to do" path.

**Commit:** `41de175`

---

## 30. `EX AF,AF'` accumulator swap — wrong variable receives the banked value

**Root cause:** `EX AF,AF'` (or `EXX`) inside a processing loop shuttles one accumulator through A' while the main register computes something else. On the paired `EX AF,AF'` that restores the main register, A gets the banked accumulated value and A' gets the current main value. The C variables for the two logical values must be assigned from the correct sides after each swap.

**Bug:** `scroll_horizon` (`$B848`): inside the vertical-scroll loop, `EX AF,AF'` at `$B8AC` banks Adiff (ticks remaining) into A' and unbanks Ahorizon_y_a25a_delta (accumulated delta) into A. The loop then does `ADD A,C` to accumulate into A, and the paired `EX AF,AF'` at `$B8AE` puts the accumulated delta back into A'. After the loop, `EX AF,AF'` at `$B8CB` hands A (= adjusted Bcounter) to `var_a25b` and A' (= Ahorizon_y_a25a_delta) to `var_a25a`. In C, the two update assignments were swapped: `horizon_y_accum += Ahorizon_y_a25a_delta` and `horizon_y_step += Bcounter`, where the Z80 does the opposite (`var_a25a += B`, `var_a25b += A'`).

**Fix:**

```c
state->horizon_y_accum += Bcounter;             /* $B8B4: var_a25a += B */
/* ... sign-extend BCcounter ... */
state->session.horizon_level += BCcounter;
/* EX AF,AF' unbanks Ahorizon_y_a25a_delta */
state->horizon_y_step += Ahorizon_y_a25a_delta; /* $B8CC: var_a25b += A (delta) */
```

**Rule:** After a paired `EX AF,AF'` that terminates a loop, trace which C variable was being accumulated in A vs A' and assign the correct one to each state field. The variable that was in A' (banked) during the loop is retrieved by the final EX; do not confuse it with the variable that was live in A just before the EX.

A second form: `EX AF,AF'` used as a **simple bank-then-unbank shuttle** (no loop). The C convention is to assign the shadow variable at the bank point (`// EX AF,AF'` comment where A is saved) and read it back at the unbank point. The mistake is to place the assignment at the unbank point instead, which captures whatever A holds at that moment — usually a completely different value.

**Bug (shuttle):** `dust_stones_stuff` (`$A9FF`): `EX AF,AF'` banks table byte 3 into A', then A is overwritten by the LOD index computation. The paired `EX AF,AF'` at `$AA21` should unbank table byte 3. In C, `saved_A = A` appeared at the second `// EX AF,AF'` comment (unbank), capturing the LOD index (0–5, always ≥ 0) rather than the table byte. The `if (saved_A < 0)` branch was therefore dead code on every frame.

**Fix:** Place `saved_A = A` at the first `// EX AF,AF'` comment (the bank), immediately after the value is loaded. Do not reassign it at the unbank point.

**Commits:** `41de175`, `55be0c6`

---

## 31. `int` accumulator — carry idiom `sum < addend` always false

**Root cause:** The standard unsigned-arithmetic trick for detecting u8 overflow after `sum += addend` is `if (sum < addend)` — if the byte wrapped, the truncated sum is less than the addend. This idiom only works when `sum` is a true `u8` (or other unsigned type that wraps at its maximum). When `sum` is `int`, the addition never wraps and `sum < addend` is always false after a non-negative addition.

**Bug:** `dh_draw_one_hazard` (`$ADCD–$ADD1`): `C_dist += IXhazard->distance` adds the hazard distance to a running accumulator. The Z80 `ADD A,C` sets carry when the byte overflows, advancing the perpendicular lane counter. The C variable `C_dist` was declared `int`; the idiom `if (C_dist < IXhazard->distance)` was therefore always false, making the perp lane-advance path dead code on every frame.

**Fix:** Check `if (C_dist > 255)` — the `int` sum exceeds 255 exactly when the Z80 8-bit addition would have carried.

```c
C_dist += IXhazard->distance;   /* $ADD0: ADD A,C */
if (C_dist > 255) {             /* $ADDB: JR NC — carry from ADD */
    A_lane++;
    ...
}
```

**Rule:** Never use `sum < addend` to detect u8 carry when `sum` is `int`. Use `sum > 255` instead. The idiom `sum < addend` is only valid for unsigned integer types that actually wrap.

**Commit:** `55be0c6`

---

## 32. `(s8)` cast on ADD result used as carry guard

**Root cause:** After a Z80 `ADD A,C`, carry signals overflow (`A + C > 255`). The sign flag is a separate bit, set when bit 7 of the result is 1. Using `(s8) A >= 0` (or `(s8) A < 0`) to decide whether to enter a block tests the sign flag, not carry. For sums in the range 128–255 (carry clear, bit 7 set), the sign test fires incorrectly — entering or skipping a block the Z80 would take the other path.

This is the addition counterpart of pitfall 8 (which covers subtraction borrow). Both mistakes replace a carry/borrow check with a sign-flag check, but the affected instruction and the direction of the error differ.

**Bug:** `draw_overhead` (`$90D4–$90D5`): `A = IXxpos[0] + Cdepth; JR C,$90E4` exits early when the addition overflows. The C code `if ((s8) A >= 0)` entered the span-width computation when bit 7 was clear. For sums 128–255 (no carry, bit 7 set) the C code skipped the block while the Z80 entered it, computing a narrower display span than intended.

**Fix:**

```c
A = IXxpos[0] + Cdepth;   /* $90D1-$90D4: ADD A,C */
if (A <= 255) {            /* $90D5: JR C,$90E4 — skip on u8 overflow */
    ...
}
```

**Rule:** After a Z80 `ADD A,x; JR C` (or `RET C`), the C guard is `if (A > 255)` for the carry branch and `if (A <= 255)` for the no-carry branch. Never use `(s8)` to recover a carry signal from an addition result.

**Commit:** `55be0c6`
