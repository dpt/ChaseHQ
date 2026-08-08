# The `.map` stage road format

`C/scripts/stage_compile.py` compiles a line-based `.map` text file into the C map arrays used by `C/libraries/ChaseHQ/Data/Stage{1-6}Data.c`, and decompiles those arrays back into `.map` text.

```bash
# From C/scripts/
python3 stage_compile.py decompile ../libraries/ChaseHQ/Data/Stage1Data.c > ../maps/stage1.map
python3 stage_compile.py compile   ../maps/stage1.map > fragment.c
python3 stage_compile.py --selftest
```

`compile` writes a C _fragment_ to stdout — `#define`s, six arrays per section and the goto-table rows. Paste it into the stage file by hand. Like `convert_stage.py`, this script never edits committed data in place, and it has no CMake target for the same reason `convert_stages` is documented as destructive.

`C/maps/stage1.map` is the committed worked example.

## Why the format exists

A stage's road layout is six parallel RLE byte streams — curvature, height, lanes, hazards, leftobjs, rightobjs. Each is readable on its own and none of them shows you the road. Nothing in

```c
MAP_LANES_4(20),
MAP_LANES_DIRTTRACK(178),
MAP_LANES_4(12),
```

tells you that stage 1's right fork is a 178-unit dirt track, and nothing lines that run up against the curvature and height streams that cover the same stretch of road. Worse, the six streams have to _agree_ on how much road they cover, and nothing checks that they do.

The `.map` format turns the six streams into six columns of one table where a row is a slice of road, and the compiler enforces the agreement.

## The 2:1 rate rule

**The six streams do not advance at the same rate.** `rm_cycle_buffer_offset` in `C/libraries/ChaseHQ/Engine/Main.c` toggles `session.no_objects_flag` between 1 and 2 on every call, and only advances hazards, leftobjs and rightobjs on the `== 1` pass:

```c
if (state->session.no_objects_flag != 1) {
  *HL_rightside_ptr = 0;              /* zero the three slots, */
  ...                                 /* advance nothing */
  state->session.no_objects_flag = 1;
} else {
  ... /* read rightobjs, leftobjs, hazards */
  state->session.no_objects_flag = 2;
}
```

Those three therefore tick once per **two** road units:

```
road unit   0    1    2    3    4    5    6    7   ...  N-1
curvature   *    *    *    *    *    *    *    *         *     N
height      *    *    *    *    *    *    *    *         *     N
lanes       *    *    *    *    *    *    *    *         *     N
hazards     *         *         *         *                    N/2
leftobjs    *         *         *         *                    N/2
rightobjs   *         *         *         *                    N/2
```

so a well-formed section satisfies

```
curvature == height == lanes == 2 x hazards == 2 x leftobjs == 2 x rightobjs
```

28 of the 30 sections in the committed stages 1-5 satisfy this exactly. The compiler rejects any section that does not, naming the per-stream lengths.

The two that do not are genuine desyncs in the original data, not tool bugs: `stage3_map_height_C5A0` is 107 units against 192 of curvature, and `stage5_map_height_C684` is 358 against 444. Decompiling those files needs `--allow-desync`, which pads the short stream with its last value and marks the section `; DESYNC`.

**Stage 6 is this tool's own output.** The port-added test level was authored as `C/maps/stage6.map` and compiled into `Stage6Data.c`, so it obeys the rule by construction. It is the one stage where the `.map` file is the source and the C is generated, rather than the other way round.

## File structure

```
; stage: 1
; prefix: stage1_map
; base: 0xC000
; obj B = BUSH
; obj R = TREE
; addr start curvature = 0x5EC4

| Dst | Left objs      | Road      | Right objs | Curve | Height | Haz  | Loop  |
| --- | -------------- | --------- | ---------- | ----- | ------ | ---- | ----- |
|     |                |           |            |       |        |      | split left right |
| 288 | .144           | H : : : H | .144       | >>    | -      |      |       |
| 2   | .              | H : :  // | .          | <     | ^^     |      |       |
| 30  | B . B .3 B .3  | H : : H   | R .3 R .4  | :     | -      |      | start |
```

**The table reads bottom to top.** The last row is the start of the stage and the first row is the end of it, so the `Road` column draws the road the way the player meets it: the bottom of the page is nearest, the top is furthest away, and a lane transition slants in the direction the rail actually moves. Everything else — section labels, the running offset, `goto`/`split`/`end` — works in road order, which is the reverse of the file order.

Lines starting with `;` are directives. Lines made only of `|`, `-` and spaces are skipped, as is the column-name header row. Everything else is a table row. Column order is fixed; the header text is decoration.

### Directives

| Directive | Meaning |
| --- | --- |
| `stage: N` | selects the `MAP_OBJ_S{N}_*` macro prefix |
| `prefix: X` | array and `#define` name prefix, e.g. `stage1_map` |
| `scale: N` | multiplies every `Dst` (default 1) |
| `base: 0x....` | first synthetic address for `*_ADDR` defines (default `0xC000`) |
| `obj C = NAME` | object symbol `C` means `MAP_OBJ_S{N}_NAME`, or the plain `MAP_OBJ_NAME` for names common to all stages (`NONE`, `TUNNEL_LIGHT`, `SHORT_POLE`); `.` is reserved for NONE |
| `addr <label> <stream> = 0x....` | pin a real address instead of synthesising one |

The decompiler emits `addr` lines for every section it reads, so a decompiled file recompiles to the same addresses.

### `Dst`

The distance the row covers, in road units, multiplied by `scale`. Blank means 1. A running offset accumulates along the section — that is, _up_ the page.

### `Road`

A 9-character canvas. Rail _k_ sits at column _2k_, the outer two rails carry the edge glyph `H` and the interior rails are `:` lane dividers.

```
col:      012345678   rails   macro
4         H : : : H   [0,4]   MAP_LANES_4        (0x00)
3L        H : : H     [0,3]   MAP_LANES_3L       (0x81)
3R          H : : H   [1,4]   MAP_LANES_3R       (0x82)
2L        H : H       [0,2]   MAP_LANES_2L       (0x01)
2M          H : H     [1,3]   MAP_LANES_2M       (0x02)
2R            H : H   [2,4]   MAP_LANES_2R       (0x03)
tunnel    # : : #     [0,3]   MAP_LANES_TUNNEL_* (0x45/0x59)
dirt      ~ : : : ~   [0,4]   MAP_LANES_DIRTTRACK(0xC1)
```

**The indent is significant** — `3R` is `3L` shifted right by one rail, and they are otherwise identical. Because that is easy to lose when hand-editing, the cell also accepts the state written out: `4`, `3L`, `3R`, `2L`, `2M`, `2R`, `TUNNEL`, `DIRT`. A lane byte with no named macro can be written `raw 0x3D`.

This geometry is not arbitrary; it is what `draw_road` does at `$C4AD` (`Main.c`, "read lanes"):

```c
A_left_hand_table_hi = A_left_offset + 0xE7;   /* lanes byte & 0x03 */
...
A_left_hand_table_hi += 3;   /* bit 7 set: 3-lane family */
A_left_hand_table_hi += 2;   /* bit 7 clear: 2-lane family */
```

Pages `$E8..$EC` are the five lane boundaries 0..4, the left rail is `0xE7 + (byte & MAP_LANES_LEFT_OFFSET_MASK)` and the right rail is 3 or 2 pages further out depending on bit 7. The 4-lane case is special-cased to `$E8`/`$EC` in `dr_four_lane_highway`, and tunnels force the right rail to `$EB`.

#### Transitions

A lane transition draws its moving rail as a ramp. Because the table reads bottom to top, a rail whose column grows as the car drives on slants `/` and one whose column shrinks slants `\`; the two ramp characters sit on the outer side of the rail's travel, and the rest of the canvas is the state being entered. The column the rail leaves is blanked, so a ramp never abuts a divider.

```
col:      012345678   macro
4>3L      H : :  \\   MAP_LANES_4TO3L
4>3R      //  : : H   MAP_LANES_4TO3R
3L>4      H : :  //   MAP_LANES_3LTO4
3R>4      \\  : : H   MAP_LANES_3RTO4
3L>2M     //  : H     MAP_LANES_3LTO2M
3R>2R       //  : H   MAP_LANES_3RTO2R
3L>2L     H :  \\     MAP_LANES_3LTO2L
2L>3L     H :  //     MAP_LANES_2LTO3L
2R>3R       \\  : H   MAP_LANES_2RTO3R
```

Those nine are the only pairs the engine defines; anything else is an error. `C/docs/lane-transitions-3lto2l-3rto2m.md` plans the missing mirror shape, `3R>2M`. Because the ramps are fiddly to draw by hand, the cell also accepts the transition written out — `4>3R` and so on — the same way it accepts a steady state written as `3R`.

Transitions are _not_ inferred from the neighbouring steady states, because the real data rules that out: `stage1_map_loop_lanes` has `MAP_LANES_4TO3R(34)` immediately followed by `MAP_LANES_3RTO4(2)`, so a transition's neighbour is not always a steady state, and a transition is not always 2 units long.

#### Tunnels

Write the whole tunnel as tunnel rows. A run of N units emits `MAP_LANES_TUNNEL_ENTRY(N-2), MAP_LANES_TUNNEL_EXIT(2)` — the entry value covers the body and only the last 2 units are the exit, which is the shape every tunnel in the committed data has.

### `Curve` and `Height`

| Curve | Macro |  | Height | Macro |
| --- | --- | --- | --- | --- |
| `<<<` | `MAP_CURVE_LEFT_VERY_HARD` |  | `^^^^` | `MAP_HEIGHT_UP7` |
| `<<` | `MAP_CURVE_LEFT_HARD` |  | `^^^` | `MAP_HEIGHT_UP5` |
| `<` | `MAP_CURVE_LEFT` |  | `^^` | `MAP_HEIGHT_UP3` |
| `:` | `MAP_CURVE_STRAIGHT` |  | `^` | `MAP_HEIGHT_UP1` |
| `>` | `MAP_CURVE_RIGHT` |  | `-` | `MAP_HEIGHT_LEVEL` |
| `>>` | `MAP_CURVE_RIGHT_HARD` |  | `v` `vv` `vvv` `vvvv` | `_DOWN1/3/5/7` |
| `>>>` | `MAP_CURVE_RIGHT_VERY_HARD` |  |  |  |

Blank means straight / level.

### `Left objs` and `Right objs`

Whitespace-separated `<symbol><count>` tokens, count in **half-units** (see the rate rule above), default 1, `.` = none. `B .3 B .10` is bush, three blank, bush, ten blank.

Evenly spaced scenery repeats, so a parenthesised run of tokens can carry a repeat count:

```
.5 T . T . T . T . T . T . T . T . T . T .3 T .3 T .3 T .3 T . T .3
.5 (T .)9 (T .3)4 T . T .3
```

Both cells mean the same thing. Groups do not nest and the count is required. The decompiler factors out whatever saves more characters than the `()` and count cost, so a pattern repeated only twice is usually left written out.

A row owns the half-units whose first road unit falls inside it, so the cells partition the stream exactly even when a row boundary lands on an odd road unit. The compiler checks each cell starts where the stream has reached, which catches a row inserted or deleted without adjusting the objects.

### `Haz`

Whitespace-separated command tokens. `MAP_HAZARD_WAIT(n)` gaps are computed from the row offsets and split across several waits above 255. A hazard row must start on an even road unit.

The engine (`Main.c`, `$BFE7: HAZARDS`) folds commands 3..9 into one latched value, `rm.hazard_byte = cmd - 3`, written to the road buffer until the next such command. Commands 10 and up are edge-triggered and re-enter the read loop without consuming a wait.

| Token | Emits | Effect |
| --- | --- | --- |
| `haz-` | `MAP_CMD_STOP_BARRIERS` (3) | latch 0 — clears the hazard |
| `haz1` `haz2` | `MAP_CMD_START_OBSTACLE_L` / `_R` (4/5) | latch 1 / 2 — one light obstacle |
| `haz3` | `MAP_CMD_START_TWO_OBSTACLES` (6) | latch 3 — a pair of light obstacles |
| `bar<` `bar>` `bar<>` | `MAP_CMD_START_BARRIERS_L` / `_R` / `_TWO_BARRIERS` (7/8/9) | latch 4 / 5 / 6 |
| `arrX` `arr<` `arr>` | `MAP_CMD_ARROW_OFF` / `_L` / `_R` (10/11/12) | `floating_arrow` |
| `cars+` `cars-` | `MAP_CMD_START_CARS` / `_STOP_CARS` (13/14) | `dont_spawn_cars` |
| `heli1` … `heli4` | `MAP_ESC, (15..18)` | `helicopter_control` |
| `esc<n>` | `MAP_ESC, (n)` | escape hatch for any unnamed code |

### `Loop`

Flow control, applied to all six streams at once.

| Token | Effect |
| --- | --- |
| `name` | defines label `name` and starts a section — one array per stream |
| `goto name` | `MAP_CMD_GOTO(NAME_<STREAM>_ADDR)`, ends the section |
| `split a b` | `MAP_CMD_SPLIT(A_..._ADDR, B_..._ADDR)`, ends the section |
| `end` | `MAP_CMD_FORK_END`, ends the section |

A `split` row must leave every other cell blank. `exit_fork` re-points all six streams at the shared `forked_road_*` / `fork_*` tables in `CommonData.c` for the fork body and its 36-unit exit block, so the row carries no road data of its own. The two labels are where the road _resumes after_ the fork — they land in `rm_*_fork_end_ptr` — not immediate branch targets.

A row after a section-ending token that carries data but no new label is unreachable, and is an error.

## Run splitting

Counts above an encoding's ceiling are split automatically: 15 for the nibble-packed curvature, height and object streams, 255 for lanes and hazard waits.

This makes compilation semantically faithful but not always textually identical. `stage1_map_start_lanes` splits its 288 units of 4-lane road as `MAP_LANES_4(254), MAP_LANES_4(34)`; recompiling produces `255, 33`. Both decode to the same 288 units. `--selftest` therefore compares the expanded per-unit streams, not the macro text. In practice 28 of stage 1's 30 arrays come back textually identical, the two lanes arrays above being the exceptions.

## Self test

`python3 stage_compile.py --selftest` round-trips the committed data:

- stages 1, 2, 4 and 6 decompile, recompile and must match per-unit — between them they cover GOTO, SPLIT, FORK_END, tunnels, dirt track, objects, barriers, arrows, cars and helicopters;
- stages 3 and 5 must be rejected by default and round-trip with `--allow-desync`.
