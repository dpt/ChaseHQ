# Stage / Level In-Memory Format

This document describes how a stage (level) is represented in memory in the C port: the top-level `stage_t` struct, the `scenedata_t` read-head, the six parallel map byte-streams, and the escape-command protocol that creates forks, junctions, and event triggers within those streams.

Source files: `C/libraries/ChaseHQ/Data/Stages.h`, `C/libraries/ChaseHQ/Data/Stage*Data.c`.

---

## Stage lookup table

```c
const stage_t *stages[MAX_STAGES + 2]; /* [0..6] */
```

Indexed by `state->wanted_stage_number`:

| Index | Content |
| --- | --- |
| 0 | Stage 1 data (pre-game / wanted_stage_number = 0) |
| 1–5 | Game stages 1–5 |
| 6 | Stage 5 data again (end-sequence; wanted_stage_number briefly reaches 6) |

`state->stage` is set to `stages[wanted_stage_number]` whenever the stage changes (see `set_up_stage` in `Main.c`).

---

## `stage_t` — top-level per-stage data

Defined in `Stages.h`, instantiated as `const stage_t stage1` … `stage5` in each `Data/Stage*Data.c`.

```
stage_t {
    backdrop[BACKDROP_WIDTH × BACKDROP_HEIGHT]   // 10 × 24 = 240 bytes
    addrof_perp_mugshot_attributes               // ZX attribute bytes for mugshot
    addrof_perp_mugshot_bitmap                   // NULL on stage 1 (no bitmap)
    ground_colour                                // u16 attribute word (two bytes)
    addrof_hittable_objects                      // hittable_t array (barriers etc.)
    addrof_right_hand_handlers                   // pointer into obj_t array at [-1]
    addrof_right_hand_objects                    // obj_t array for roadside objects RHS
    addrof_right_hand_short_pole_object          // specific obj_t for short poles RHS
    addrof_left_hand_handlers                    // pointer into obj_t array at [-1]
    addrof_left_hand_objects                     // obj_t array for roadside objects LHS
    addrof_left_hand_short_pole_object           // specific obj_t for short poles LHS
    addrof_perp_description                      // chatter byte sequence
    addrof_arrest_messages                       // arrest text rendering commands
    addrof_helicopter_stuff_1/_2                 // NULL if stage has no helicopter
    bitmaps_stones / bitmaps_dust                // 6-frame sprite sequences
    bitmaps_perp_car                             // perp vehicle bitmaps
    bitmaps_vehicles[4]                          // NPC vehicle bitmaps
    car_spawn_delay                              // frames between NPC car spawns
    perp_lane_change_base                        // base counter for perp lane changes
    perp_approach_base                           // base counter for perp approach
    stage_data    (scenedata_t)                  // used during gameplay
    attract_data  (scenedata_t)                  // used during attract mode
    chatter_strings                              // NULL-terminated array of string ptrs
}
```

### Backdrop

A `BACKDROP_WIDTH × BACKDROP_HEIGHT` (10 × 24 = 240) byte bitmap drawn as the sky / scenery strip behind the road. Stored in pixel-row-major order, 10 bytes (80 pixels) per row, 24 rows. Each stage has a unique backdrop.

### Object tables (`obj_t`)

Each roadside object entry in `stage1_right/left_hand_graphics_defs[]` has:

```c
struct obj {
    u8          hit_max_or_min;     // hit zone boundary (right: hi, left: lo)
    u8          hit_min_or_max;     // hit zone boundary (right: lo, left: hi)
    u8          impact_speed_cap;   // speed cap for crash damage calculation
    const void *arg;                // pointer to rendering data (stretchy_t etc.)
    obj_handler_t *handler;         // draw_stretchy_object_right/left etc.
};
```

The arrays are initialised at index `[-1]` (one element before the start) to match the Z80 convention where the handler reads the _preceding_ entry for metadata.

---

## `scenedata_t` — active map read-head

```c
struct scenedata {
    u16       road_pos;              // $A26C: car's lateral road position
    const u8 *road_curvature_ptr;   // $A26E: → curvature stream
    const u8 *road_height_ptr;      // $A270: → height stream
    const u8 *road_lanes_ptr;       // $A272: → lanes stream
    const u8 *road_rightside_ptr;   // $A274: → right-hand object stream
    const u8 *road_leftside_ptr;    // $A276: → left-hand object stream
    const u8 *road_hazard_ptr;      // $A278: → hazard / event stream
};
```

`state->scenedata` is the _live_ copy that `read_map` and the movement code mutate each frame. It is initialised from `stage->stage_data` (or `stage->attract_data`) by `set_up_stage`.

### `road_pos`

The car's lateral position on the road, encoded as a u16 where **higher values = further left, lower values = further right**:

| Constant         | Value    | Meaning             |
| ---------------- | -------- | ------------------- |
| `ROAD_RIGHTMOST` | `0x00F5` | rightmost road edge |
| `ROAD_LEFTMOST`  | `0x0105` | leftmost road edge  |

The value straddles 0x0100 (256) at road centre; the high byte carries the left-of-centre bit. Horizontal movement adds/subtracts from `road_pos`; the result is clamped to `[ROAD_RIGHTMOST, ROAD_LEFTMOST]`.

### Stream pointers

All six pointers are initialised to `array[-1]` (one byte _before_ the first entry). `read_map` pre-increments each pointer before reading, so the first byte consumed is `array[0]`.

---

## Map streams

Each `scenedata_t` points into six independent byte-streams. They advance in lockstep: each call to `read_map` reads one logical entry from each stream and writes the decoded result into the 256-byte circular `road_buffer` (`state->road_buffer`), which is divided into 32-byte slots:

| Slot offset | `ROADBUF_*_OFFSET`         | Stream             |
| ----------- | -------------------------- | ------------------ |
| 0×32 = 0    | `ROADBUF_CURVATURE_OFFSET` | Curvature          |
| 1×32 = 32   | `ROADBUF_HEIGHT_OFFSET`    | Height             |
| 2×32 = 64   | `ROADBUF_LANES_OFFSET`     | Lanes              |
| 3×32 = 96   | `ROADBUF_RIGHTOBJS_OFFSET` | Right-hand objects |
| 4×32 = 128  | `ROADBUF_LEFTOBJS_OFFSET`  | Left-hand objects  |
| 5×32 = 160  | `ROADBUF_HAZARDS_OFFSET`   | Hazards / events   |

`state->roadbufptr` is the circular write pointer into `road_buffer`. Each frame, the road-drawing code reads from `roadbufptr + ROADBUF_*_OFFSET` to get the current road state for each channel.

---

### Curvature stream

One byte per entry:

```
bits 7..4  duration D  (number of frames this value holds, 1..15)
bits 3..0  curve type
```

| Low nibble | Macro                       | Meaning         |
| ---------- | --------------------------- | --------------- |
| 0          | `MAP_CURVE_STRAIGHT`        | straight        |
| 1          | `MAP_CURVE_RIGHT`           | gentle right    |
| 2          | `MAP_CURVE_RIGHT_HARD`      | hard right      |
| 3          | `MAP_CURVE_RIGHT_VERY_HARD` | very hard right |
| 9          | `MAP_CURVE_LEFT`            | gentle left     |
| 10         | `MAP_CURVE_LEFT_HARD`       | hard left       |
| 11         | `MAP_CURVE_LEFT_VERY_HARD`  | very hard left  |

Use `MAP_CURVE_STRAIGHT(D)`, `MAP_CURVE_RIGHT(D)`, etc. to construct entries.

---

### Height stream

One byte per entry, same layout as curvature:

```
bits 7..4  duration D
bits 3..0  grade
```

| Low nibble | Macro              | Meaning          |
| ---------- | ------------------ | ---------------- |
| 1          | `MAP_HEIGHT_UP7`   | steepest climb   |
| 3          | `MAP_HEIGHT_UP5`   |                  |
| 5          | `MAP_HEIGHT_UP3`   |                  |
| 7          | `MAP_HEIGHT_UP1`   | shallow climb    |
| 8          | `MAP_HEIGHT_LEVEL` | flat             |
| 9          | `MAP_HEIGHT_DOWN1` | shallow descent  |
| 11         | `MAP_HEIGHT_DOWN3` |                  |
| 13         | `MAP_HEIGHT_DOWN5` |                  |
| 15         | `MAP_HEIGHT_DOWN7` | steepest descent |

---

### Lanes stream

Two bytes per logical entry:

```
byte 0: duration D
byte 1: lanes_byte
```

`lanes_byte` encodes the road geometry. The bottom two bits (`MAP_LANES_LEFT_OFFSET_MASK = 0x03`) are the left-hand edge offset; the remaining bits select the road type:

| Value | Macro | Road type |
| --- | --- | --- |
| `0x00` | `MAP_LANES_4` | 4-lane road |
| `0x81` | `MAP_LANES_3L` | 3-lane, offset left |
| `0x82` | `MAP_LANES_3R` | 3-lane, offset right |
| `0x01` | `MAP_LANES_2L` | 2-lane, offset left |
| `0x02` | `MAP_LANES_2M` | 2-lane, centred |
| `0x03` | `MAP_LANES_2R` | 2-lane, offset right |
| `0xBD` | `MAP_LANES_4TO3L` | transition 4→3 left |
| `0x8E` | `MAP_LANES_4TO3R` | transition 4→3 right |
| `0xAD` | `MAP_LANES_3TO4L` | transition 3→4 left |
| `0x9E` | `MAP_LANES_3TO4R` | transition 3→4 right |
| `0x06` | `MAP_LANES_3TO2L` | transition 3→2 left |
| `0x0F` | `MAP_LANES_3TO2R` | transition 3→2 right |
| `0x2D` | `MAP_LANES_2TO3L` | transition 2→3 left |
| `0x3D` | `MAP_LANES_3LTO2L` | transition 3L→2L; stage 2 only, at `$E56D` |
| `0x1F` | `MAP_LANES_2TO3R` | transition 2→3 right |
| `0x41` | `MAP_LANES_TUNNEL_VAL` | tunnel body |
| `0x45` | `MAP_LANES_TUNNEL_ENTRY_VAL` | tunnel entrance |
| `0x59` | `MAP_LANES_TUNNEL_EXIT_VAL` | tunnel exit |
| `0xC1` | `MAP_LANES_DIRTTRACK` | dirt track |
| `0xED` | `MAP_LANES_FORKED` | forked road |

Bit-decode summary (from `Stages.h`):

```
bits 7..0 of lanes_byte:
  all clear                     → 4-lane normal road
  bit 7 set, bit 6 clear        → 3-lane or 3↔4 transition
  bit 7 clear, bit 6 clear, ≠0  → 2-lane or 2↔3 transition
  bit 6 set, bit 7 clear        → tunnel (entry/body/tail call bits 2..4)
  bit 6 set, bit 7 set          → forked road (bit 5 set) or dirt track
```

---

### Right-hand and left-hand object streams

One byte per entry:

```
bits 7..4  duration D
bits 3..0  object type ID
```

The type ID indexes into the stage's `right_hand_graphics_defs[]` / `left_hand_graphics_defs[]` array. Stage 1 object IDs:

| ID  | Macro                       | Object                             |
| --- | --------------------------- | ---------------------------------- |
| 0   | `MAP_OBJ_NONE`              | nothing                            |
| 1   | `MAP_OBJ_TUNNEL_LIGHT`      | tunnel lamp                        |
| 2   | -                           | all-zero table entry; never used   |
| 3   | `MAP_OBJ_SHORT_POLE`        | short pole (common to all stages)  |
| 4   | `MAP_OBJ_S1_TREE`           | tree                               |
| 5   | `MAP_OBJ_S1_BUSH`           | bush                               |
| 6   | `MAP_OBJ_S1_STREET_LAMP`    | street lamp                        |
| 7   | `MAP_OBJ_S1_TELEGRAPH_POLE` | telegraph pole                     |
| 8   | `MAP_OBJ_S1_TURN_SIGN_L`    | left turn sign                     |
| 9   | `MAP_OBJ_S1_TURN_SIGN_R`    | right turn sign                    |

IDs may differ between stages; only IDs 0, 1 and 3 are known to be stage-independent (`MAP_OBJ_NONE`, `MAP_OBJ_TUNNEL_LIGHT`, `MAP_OBJ_SHORT_POLE`, defined once in `Stages.h`).

---

### Hazard / event stream

A mix of plain wait bytes and escape command sequences:

- **Wait byte** (non-zero, non-escape): `MAP_HAZARD_WAIT(T)` — hold for T frames before the next command.
- **Escape sequence**: `MAP_ESC` (0x00) followed by a command code byte and zero or more argument bytes.

---

## Map escape commands

All sequences begin with `MAP_ESC` (0x00). The command code follows immediately.

| Code | Macro | Arguments | Effect |
| --- | --- | --- | --- |
| 0 | `MAP_CMD_GOTO` | lo, hi (u16 Z80 addr) | Jump to address in this stream |
| 1 | `MAP_CMD_FORK_END` | — | End of a fork branch; execution resumes at join |
| 2 | `MAP_CMD_SPLIT` | lo₁,hi₁,lo₂,hi₂ | Fork: all six streams split into left/right branches |
| 3 | `MAP_CMD_STOP_BARRIERS` | — | Stop spawning obstacles and barriers |
| 4 | `MAP_CMD_START_OBSTACLE_L` | — | Spawn one light obstacle, x=50 |
| 5 | `MAP_CMD_START_OBSTACLE_R` | — | Spawn one light obstacle, x=220 |
| 6 | `MAP_CMD_START_TWO_OBSTACLES` | — | Spawn a pair of light obstacles, x=70/180 |
| 7 | `MAP_CMD_START_BARRIERS_L` | — | Spawn one barrier, x=50 |
| 8 | `MAP_CMD_START_BARRIERS_R` | — | Spawn one barrier, x=220 |
| 9 | `MAP_CMD_START_TWO_BARRIERS` | — | Spawn a pair of barriers, x=80/160 |
| 10 | `MAP_CMD_ARROW_OFF` | — | Hide the on-screen direction arrow |
| 11 | `MAP_CMD_ARROW_L` | — | Show left arrow (fork guidance) |
| 12 | `MAP_CMD_ARROW_R` | — | Show right arrow (fork guidance) |
| 13 | `MAP_CMD_START_CARS` | — | Enable NPC car spawning |
| 14 | `MAP_CMD_STOP_CARS` | — | Disable NPC car spawning |
| 15 | `MAP_CMD_HELI_LEAVE` | — | Helicopter flies away (`helicopter_control` = 1) |
| 16 | — | — | `helicopter_control` = 2, "departing"; never in stage data |
| 17 | `MAP_CMD_HELI_TURN_L` | — | Helicopter turns left (`helicopter_control` = 3) |
| 18 | `MAP_CMD_HELI_TURN_R` | — | Helicopter turns right (`helicopter_control` = 4) |

Commands 3 to 9 are not distinct actions: each latches `rm.hazard_byte = code − 3` (`$C02C`), and that byte is stamped into the road buffer for every slice until the next such command. `spawn_hazards` (`$AB9A`) decodes it — 1/2/3 place one obstacle left, one right, or a pair, using the stage's light hittable object (`addrof_hittable_objects[0]`, the tumbleweed on the stage 1 and 3 dirt track); 4/5/6 are the same three placements using the heavy one (`addrof_hittable_objects[1]`, the barrier). Stages 2 and 5 set both table entries to the same sprite, so there the two triples differ only in the pair spacing.

### Fork / split protocol

`MAP_CMD_SPLIT` appears in all six streams simultaneously (at the same logical position). It carries two addresses: one for the left branch, one for the right. `MAP_CMD_FORK_END` terminates each branch; after both branches rejoin the streams continue with the content that follows `FORK_END`.

In the C port the Z80 addresses in `GOTO` and `SPLIT` are resolved to C pointers via a per-stage `lookup_map_goto()` function (e.g. `stage1_lookup_map_goto(state, z80)` in `Stage1Data.c`).

---

## Sprite and bitmap types

```c
bitmap_t     { width_bytes, flags, height, data, shifted }
depthset_t   { bitmaps, pairs[DEPTHSET_MAX] }   // LOD table: depth→bitmap offset
stretchy_t   { type, set }                       // perspective-scaled object root
hittable_t   { width, bitmaps }                  // hazard sprite + hit width
```

`SPRITE_FRAMES` (6) is the number of distance LODs / animation frames per sprite sequence. `BITMAPFLAG_MASKED` and `BITMAPFLAG_FLIPPED` control draw mode.

`stretchy_t.type` constants (`STRETCHY_TYPE_*`) select the scaling percentage applied to the perspective height at each depth level (100%, 75%, 150%, etc.); `STRETCHY_TYPE_FIXED` bypasses perspective entirely.
