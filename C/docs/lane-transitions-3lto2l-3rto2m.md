# Adding MAP_LANES_3LTO2L and MAP_LANES_3RTO2M lane transitions

## Context

The road-lane system encodes lane-count sections and transitions as a
bit-packed byte (`MAP_LANES_*_VAL` in `C/libraries/ChaseHQ/Data/Stages.h`). The two existing
3-lane→2-lane narrowing transitions (`3LTO2M`, `3RTO2R`) both drop the
**leftmost** lane while holding the **right** edge fixed. There is no mirror
shape that drops the **rightmost** lane while holding the **left** edge
fixed (`3L→2L`, `3R→2M`), even though the steady-state endpoints for both
(`3L`, `2L`, `3R`, `2M`) already exist and are fully supported by the
parametric per-scanline renderer.

> **Correction (supersedes the original premise below).** Half of this was
> never new content. `0x3D` **is** original game data: stage 2's lanes stream
> writes it at `$E56D` (bank-1 skool), for 2 units, between a `3L` run of 40
> and a `2L` run of 38 — exactly a 3L→2L narrowing. It is now named
> `MAP_LANES_3LTO2L_VAL` in `Stages.h` and written as `MAP_LANES_3LTO2L(2)`
> in `Stage2Data.c`, where it previously appeared as a bare
> `/* unknown lanes val=0x3D */ 2, 0x3D`.
>
> So the claim that `0x3D` is "a bit pattern the original game never
> produces" is false, and the argument that led to it — enumerating
> `(bit7,bit5,bit4)` triples against the *named* constants — was only ever
> enumerating against the names, not against the data. `0x3E` does survive
> the same check against the data: no lanes stream in any stage uses it.
>
> What remains of this plan:
>
> - **`3LTO2L` (`0x3D`)**: not new content, and nothing to implement. The
>   engine already routes it through existing branches (bit5 set, bit7 clear →
>   `H = 0xEB`; bit4 set → the near-boundary path with the animation offset),
>   and the taper it produces has been confirmed correct on screen in stage 2.
> - **`3RTO2M` (`0x3E`)**: still genuinely invented, still needs the work
>   below.
>
> Anything below that treats `0x3D` as a free slot should be read as applying
> to `0x3E` alone. In particular the new branch's mask
> `(L_lane_flags & 0xB0) == 0x30` matches `0x3D` as well as `0x3E`, so
> implementing it as written would **change how stage 2's existing section
> renders**. Narrow the test to `0x3E` unless that change is what you want.

This is genuinely new game content with no Z80 counterpart — every other
line in `draw_road_lanes_change` mirrors a specific disassembled Z80
instruction. The new code must be clearly marked as invented (not a `Conv:`
departure from an existing translation, since there is nothing in the
original to depart from), and must use a bit pattern the original game never
produces, so it can never collide with real stage data.

## Byte value design

Verified directly against the source (`Main.c:13918-14208`), not just the
`Stages.h` decode comment:

- Bit 7 must be **0** — this is what makes `dr_read_lanes` render the road at
  2-lane width throughout the transition (matches `3LTO2M`/`3RTO2R`
  precedent); the taper comes entirely from `draw_road_lanes_change`
  overwriting one rail table.
- Bits 0-1 (`MAP_LANES_LEFT_OFFSET_MASK`) = the 2-lane endpoint's own offset,
  per the convention every existing 2↔3-family byte follows: `1` for `2L`,
  `2` for `2M`.
- Enumerating every `(bit7,bit5,bit4)` triple against all 12 existing named
  constants (steady, transition, tunnel, dirt, forked) shows `(0,1,1)` is
  the **only** combination with bit7=0 that is completely unused. This is
  forced, not a free choice — it's the only available slot. *(Wrong for
  `0x3D`: see the correction above. The original game uses that pattern.)*
- Bits 2-3 just need to be non-zero to enter the transition code at all;
  use `11` (`0xC`) to match the majority convention.

```
MAP_LANES_3LTO2L_VAL = 0x3D  // 0011_1101 (offset=1, matches 2L) — original data, now named
MAP_LANES_3RTO2M_VAL = 0x3E  // 0011_1110 (offset=2, matches 2M) — invented
```

## 1. `Stages.h`

`MAP_LANES_3LTO2L_VAL` and `MAP_LANES_3LTO2L(D)` are already present — they
were added when the stage 2 byte was identified. Only the `3RTO2M` pair
remains to be added:

```c
#define MAP_LANES_3RTO2M_VAL            (0x3E) // 0011_1110
#define MAP_LANES_3RTO2M(D)             (D), (MAP_LANES_3RTO2M_VAL)
```

## 2. `Main.c`: `draw_road_lanes_change` (~line 13918)

**Geometry.** For both new shapes the fixed edge is the one already passed
in (`H_left_hand_table_hi = offset + 0xE7`, the 2-lane endpoint's own left
edge — which is *also* the 3-lane endpoint's left edge, since both share
the same offset by construction). The moving edge is the **right** edge:
destination (2-lane's own right rail) = `H_left_hand_table_hi + 2`; seed
source (3-lane's own right rail, one page further out) = destination `+ 1`.
This is the mirror of the existing `3LTO2M`/`3RTO2R` mechanic, which writes
into `H` itself (already the moving left edge) seeded from `H - 1`.

**What `0x3D` does today.** Nothing intercepts it, so it takes the general
path: bits 2-3 non-zero enters the lane-change code, bit 5 set with bit 7
clear rewrites `H_left_hand_table_hi` to `0xEB` (`xpos_road_centre_right`),
and bit 4 set selects the near-boundary path (`A_curve_step = 0x20`,
`C_ref_height = IY[1]`, animation offset applied). That is the same
treatment `2LTO3L` (`0x2D`) gets bar the bit-4 path, which is what you would
want of its mirror. Confirmed on screen: drive stage 2 to the section after
the tunnel, where a 3L run of 40 narrows to a 2L run of 38, and the taper
renders correctly as it stands. Any branch that captures `0x3D` is therefore
a regression, not a fix.

**New branch.** Insert immediately after `(*IY_heightptr)--;` (line 13967),
*before* the existing `if (L_lane_flags & (1 << 5))` check (line 13970) —
our marker also has bit5 set, so it must be intercepted first or it would
fall into the existing fixed-page override logic:

```c
      // New: {3LTO2L, 3RTO2M} — no Z80 counterpart. Mirror of {3LTO2M,
      // 3RTO2R}: those hold the right edge fixed and taper the left edge
      // (write into H itself, seeded from H-1). These hold the left edge
      // fixed (H_left_hand_table_hi, unchanged) and taper the right edge:
      // destination is the narrow (2-lane) right rail, H+2; seed is the
      // wide (3-lane) right rail one page further out, H+3.
      if ((L_lane_flags & 0xB0) == 0x30) {
        H_left_hand_table_hi += 2;

        A_curve_step = 0x20;                 // matches 3RTO2R's pairing
        C_ref_height = (*IY_heightptr)[1];
        A_bresen_range = (*IY_heightptr)[0] - C_ref_height;
        if ((s8) A_bresen_range <= 0)
          goto drlc_continue;

        C_bresen_range = A_bresen_range;
        B_HLzone_stride = A_bresen_range * 2;
        L_left_hand_table_lo = ~((96 - (*IY_heightptr)[0]) << 1);
        HL_left_hand_table = addr_to_xpos(state, H_left_hand_table_hi, L_left_hand_table_lo);

        SP_output = HL_left_hand_table;
        H_left_hand_table_hi++; // seed: wide (3-lane) right edge
        HL_left_hand_table = addr_to_xpos(state, H_left_hand_table_hi, L_left_hand_table_lo);
        DE_roadpos = (HL_left_hand_table[0] << 8) + HL_left_hand_table[-1];

        L_left_hand_table_lo -= B_HLzone_stride;
        H_left_hand_table_hi--; // back to narrow (2-lane) right edge
        HL_left_hand_table = addr_to_xpos(state, H_left_hand_table_hi, L_left_hand_table_lo);

        A_anim_offset = ((state->fast_counter >> 3) & 0x1C) + A_curve_step;
        DE_roadpos -= A_anim_offset; // tunable: try += if the taper looks inverted

        goto drlc_shared_tail;
      }
```

No new local variables are required — every value reuses an existing
declaration from the top of the function. This keeps the diff to one new
`if` block plus one new label; it does **not** duplicate the ~40-line
Bresenham fill.

**New label.** Add `drlc_shared_tail:` immediately before the existing
`HL_pos_delta = ...` line (currently line 14134), so the new branch's
success path joins the function's existing tail (delta computation,
Bresenham fill, steep-step fallback) unchanged.

**Required companion fix — the `-256` correction.** At line 14144:

```c
if ((*IX_lanesptr)[0] & (1 << 5))
  SP_output -= 256;
```

This corrects `SP_output` by exactly one table page (each `xpos_road_*`
table is a contiguous 256-byte/128-entry block — see `State.h:732-743`)
for the cases that override `H` to a fixed proxy page (`0xEC`/`0xEB`) and
then need to shift back to the real target page. Our new branch also sets
bit5 (it was the only free bit), but never uses a proxy page — its `H`
arithmetic already lands on the correct page throughout, so this
correction must **not** apply to it. Testing the raw bit again here would
misfire. Fix: introduce a plain boolean set only on the genuine
override path, and test that instead of re-testing the byte:

```c
u8   H_is_override_page;  /* true when H was rewritten to a fixed proxy page (new — no Z80 register) */
```

Declared at the top of the function with the other locals, then:

```c
H_is_override_page = 0;                       // before the if/else-if chain
...
if (L_lane_flags & (1 << 5)) {                 // existing override branch
  H_is_override_page = 1;
  ...
}
...
if (H_is_override_page)                        // replaces the raw bit5 re-test at line 14144
  SP_output -= 256;
```

**Tunables to verify visually, not just by unit test:** `A_curve_step`/
`C_ref_height` pairing (`0x20`/`IY[1]` chosen to match `3RTO2R`'s
convention — the alternative is `0x00`/`IY[2]`, matching `3LTO2M`) and the
`+=`/`-=` sign on `DE_roadpos` for `A_anim_offset` (mirrored to `-=` by
analogy with the existing left/right sign convention, but not derivable
with certainty from the disassembly since this code path doesn't exist in
the original). Getting either wrong will be visually obvious (edge snaps or
moves the wrong way) when driving through a stage that uses the new
transition — check this during the build/run verification step below.

## 3. `get_spawn_lanes` (~line 8826) — no change needed

Hand-verified for both new bytes: `get_spawn_lanes` only inspects bit 7 and
bit 1 of the lane byte. `0x3D` (bit7=0, bit1=0) yields the same
`(min,max)` result as steady `MAP_LANES_2L_VAL`; `0x3E` (bit7=0, bit1=1)
yields the same result as steady `MAP_LANES_2M_VAL`. Both are exactly the
results wanted for traffic spawning during these transitions.

## 4. Test harness fix — `chq_test_draw_road_lanes_change` (`Main.c:19356`)

This wrapper currently hardcodes `0xEC` for `H_left_hand_table_hi`
regardless of `lane_flags`:

```c
draw_road_lanes_change(state, 0, 0, 0x0100,
                       0xEC /* H_left_hand_table_hi */, 0xFF, &local_IX, &local_IY);
```

Harmless today because the only byte currently exercised (`0xBD`/
`4TO3L`) overrides `H` internally regardless of what's passed. Our new
branch relies on receiving the *correct* offset-derived `H` (it computes
`H+2`/`H+3` from it), so passing `0xEC` unconditionally would make
`hi_to_xpostab` hit its `assert(0)` for an out-of-range page. Fix by
deriving `H` the same way `dr_read_lanes` does:

```c
draw_road_lanes_change(state, 0, 0, 0x0100,
                       (lane_flags & MAP_LANES_LEFT_OFFSET_MASK) + 0xE7,
                       0xFF, &local_IX, &local_IY);
```

Verified safe for the existing test: `4TO3L` still gets internally
overridden to `0xEC`, so this changes nothing for it.

## 5. New tests — `C/Tests/UnitTest.c`

Add two tests immediately after `test_drlc_writes_xpos_entries` (line 446),
following its exact structure (snapshot the rail table expected to change,
run the transition, assert at least one entry differs), and register both
in `main()` next to the existing call (~line 868):

- `test_drlc_writes_xpos_entries_3lto2l` — snapshot `xpos_road_centre`
  (0xEA = destination for offset1+2), call with `MAP_LANES_3LTO2L_VAL`,
  assert it changes and `xpos_road_left` (0xE8, the fixed edge) does not.
- `test_drlc_writes_xpos_entries_3rto2m` — snapshot
  `xpos_road_centre_right` (0xEB = destination for offset2+2), call with
  `MAP_LANES_3RTO2M_VAL`, assert it changes and `xpos_road_centre_left`
  (0xE9, the fixed edge) does not.

Use `height_offset=1` (matching the existing test's near-tier case).

## Verification

1. `cmake --build cmake-build-debug --target ChaseHQ_Tests &&
   ./cmake-build-debug/ChaseHQ_Tests` — all tests (existing + 2 new) pass.
2. `cmake --build cmake-build-debug && ./cmake-build-debug/ChaseHQ` — add a
   short test stretch to a stage's lane byte-stream using
   `MAP_LANES_3L(N), MAP_LANES_3LTO2L(2), MAP_LANES_2L(N)` (or the `3R`/`2M`
   equivalent), drive through it, and visually confirm the road tapers
   smoothly on the right with the left edge staying put — this is the real
   check on the two tunables flagged above, since an unrealistic sign or
   curve choice would only be visible on screen, not in a unit-test
   assertion.
