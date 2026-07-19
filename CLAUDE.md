# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a disassembly and C port of the ZX Spectrum 128K game "Chase H.Q." by Ocean Software. The project has two parallel strands: reverse engineering via SkoolKit, and a faithful C reimplementation of the game logic.

## Building

### Stage data converter (root directory)

`convert_stage.py` turns a bank skool file into a C stage data skeleton.
See `C/docs/convert-stage.md` for full usage, options and what requires
manual completion afterwards.

### Disassembly (root directory)
```bash
make pristine    # Create pristine snapshot
make skool       # Generate skool file from control file + snapshot
make asm         # Build assembly listing
make tap         # Build TAP file for emulator
make z80         # Build Z80 snapshot
make ctl         # Rebuild control file from skool
make commit      # Commit changes back to control file
```

### C implementation (C/ directory)
CMake is the canonical build system. The `Makefile` references outdated source names and is legacy.

```bash
# From C/
cmake -S . -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build cmake-build-debug
./cmake-build-debug/ChaseHQ
```

Formatting uses clang-format via CMake:
```bash
# From C/
cmake --build cmake-build-debug --target format
```

A unit test binary exists alongside the main build:

```bash
# From C/
cmake --build cmake-build-debug --target ChaseHQ_Tests
./cmake-build-debug/ChaseHQ_Tests
```

Tests live in `C/Tests/` (`TestDrawRoad.c`, `RenderStretchyObject.c`). They are built with `-DCHQ_TESTS`, which compiles in thin wrappers at the bottom of `ChaseHQ/Engine/Main.c` (inside `#ifdef CHQ_TESTS`) that expose static functions for direct testing. Declarations for those wrappers live in `C/libraries/ChaseHQ/Engine/Tests.h`. When adding a new test hook, add the wrapper to `Main.c` and declare it in `Tests.h`.

## C Implementation Architecture

### Source layout
- `C/include/<Module>/`: public headers (the interface other modules or the app consume) — `C99/Types.h`, `ZXSpectrum/*.h` (all but `Screen.h`), `ChaseHQ/ChaseHQ.h`
- `C/libraries/<Module>/`: sources and internal-only headers
  - `libraries/ZXSpectrum/`: ZX facade implementation, plus `Screen.h` (internal)
  - `libraries/ChaseHQ/Engine/`: game code (`Main.c`, `Bank3.c`, `Bank3.h`, `Bank3State.h`, `Create.c`, `State.h`, `Internal.h`, `Tests.h`, `Types.h`)
  - `libraries/ChaseHQ/Data/`: read-only stage/sound/title/bank-3 tables (`Stages.*`, `Stage{1-6}Data.*`, `CommonData.*`, `SoundSamples.*`, `TitleScreenData.*`, `Bank3Data.*`)
- `C/SDLMain.c` and `C/Tests/`: the app entry point and test driver, not modules themselves

### Entry point and lifecycle
`C/SDLMain.c` owns the SDL window and a dedicated game thread. The runtime lifecycle is:

```
chq_create → chq_setup (blocks via longjmp until quit signal) → chq_stop → chq_destroy
```

`chq_setup()` calls `bootstrap()` which runs the internal game loop via `longjmp` until a quit signal arrives. `chq_main()` is declared in the public API but not wired into the runtime path — it exists for future modular use.

### Layers
- **Host** (`C/SDLMain.c`): SDL2 window, event loop, `zxconfig_t` callbacks wired to game
- **ZX emulation facade** (`C/libraries/ZXSpectrum/Spectrum.*`): exposes `in`/`out`/`draw`/`stamp`/`sleep` callbacks; game code never calls SDL directly
- **Game** (`C/libraries/ChaseHQ/Engine/Main.c`): translation-oriented, heavily commented with Z80 addresses (~1194 `$`-prefixed lines); ~17 TODO/stub markers concentrated in SFX and title-screen code
- **State** (`C/libraries/ChaseHQ/Engine/State.h`): `struct chqstate` — the single source of mutable game state, fields ordered by original Z80 memory addresses
- **Stage data** (`C/libraries/ChaseHQ/Data/Stages.h`, `Stage{1-6}Data.*`, `CommonData.*`): read-only game tables consumed by game logic

### Data flow
- **Rendering**: game mutates `state->speccy->screen` → ZX facade tracks dirty regions → `draw_handler` in `Main.c` calls `zxspectrum_claim_screen` → SDL texture update
- **Input**: SDL keys → `zxkeyset_t`/`zxkempston_t` → `key_handler` → Spectrum IN ports (`port_KEYBOARD_*`, `port_KEMPSTON_JOYSTICK`)
- **Audio/border**: audio hooks (`play_regular_sfx_hook`, `play_engine_sfx_hook`, `play_speech_hook`) are functional with sample-accurate playback. Border colour handling is a TODO.

### Stage status
Six per-stage data structs exist (`stage1`–`stage6`), each with its own `.c`/`.h` pair in `C/libraries/ChaseHQ/Data/`. `Stages.c` maps them into the `stages[]` array: index 1 = `stage1`, 2 = `stage2`, 3 = `stage3`, 4 = `stage4`, 5 = `stage5`, 6 = `stage6`, 7 = end-sequence reload (falls back to `stage5`). Per-stage road layouts, objects, and sprites are present for all six stages.

## Coding Conventions

- **Types**: use `u8`, `u16`, `s8`, `s16` from `C/include/C99/Types.h` in game and state code
- **State**: never introduce globals; pass and mutate `chqstate_t *state` throughout
- **`(SM)` fields**: each field annotated `(SM)` in `chqstate` corresponds to a Z80 self-modifying instruction at the given address — these are correctness-critical; do not remove or rename carelessly
- **`Conv:` comments**: mark where the C version intentionally diverges from a direct Z80 translation; preserve them
- **Address semantics**: macros like `ADDRTOSCREEN`, `BACKBUFTOOFFSET`, `ROADBUFPTR` are correctness-critical
- **Formatting**: K&R style, 2-space indent, 80 columns, pointer aligned to name (`.clang-format`)
- **C standard**: Target C89/C90. Avoid C99 constructs: no compound literals `(T){…}`, no VLAs, no in-loop declarations (`for (int i = …)`). Single-line `//` comments and `<stdint.h>` types are accepted as widely-supported extensions.

### Variable naming in Z80 translations

This C code is a model of a Z80 program, written to educate. Each C variable
represents an individual register use, named after the register it came from.
Variable names deliberately include the register name to make it easy to
cross-reference the C code against the disassembly when debugging.

All local variables in translated functions must follow this pattern:

- **Name**: `RegisterName_description` — the Z80 register that held the value (e.g. `A_prev_height`, `C_acc`, `HLbackdrop`, `DEscr`). Use the full pair name (`HL`, `DE`, `BC`) when the variable represents a 16-bit quantity. Use a plain noun when there is no single source register (e.g. `carry`).
- **Placement**: declare all variables at the top of their scope, one per line, before any statements. Never combine declaration with initialisation in the same line.
- **Order**: ordered by first use, top to bottom.
- **Comment**: each declaration ends with a brief comment: intent first, then `(was X)` to record the Z80 register. Example:

```c
int        D;                  /* screen address high byte (was D) */
int        A_col;              /* backdrop source column; reset per row (was A') */
const u8  *HLbackdrop;         /* pointer to first byte of the current backdrop row (was HL) */
```

### Modelling EXX / EX AF,AF' (register banking)

The `EXX` instruction swaps main BC/DE/HL with their shadow (BC'/DE'/HL') and
`EX AF,AF'` swaps A and F with A' and F'. In C we **ignore the swap itself**:
the main-register variables are left unchanged. Instead, at the `// EXX` (or
`// EX AF,AF'`) comment, we assign the shadow-side variables from the
main-side variables to record what the Z80 banked into shadow:

```c
// EXX - bank ($xxxx)
HLdash = HL;   /* HL → HL' */
DEdash = DE;   /* DE → DE' */
```

When a second `EXX` restores the main registers from shadow (an "unbank"), the
same pattern applies in reverse — main-register variables are assigned from the
shadow variables that hold the banked values. The key points:

- Shadow variables (`HLdash`, `DEdash`, `BCdash`, `Adash`, `Aflip`, …) are a
  separate set; they are declared at the top of scope like any other variable.
- The `// EXX` comment marks exactly where the banking occurs; the assignment
  lines immediately follow it.
- Because we ignore the swap direction for main registers, a main variable like
  `HL` may appear to retain its old value past an EXX — that is intentional.
  Only the shadow side needs updating at each EXX point.

## High Quality Functions

A high quality translated function satisfies all of the following criteria.
See `C/docs/function_comment_template_example.c` for a worked example.

1. **Correct translation** — behaviour matches the original Z80 precisely,
   verified against the skool.
2. **Well-named function** — the name describes what it does, not where it
   lives (e.g. `draw_road_stripe`, not `sub_C5A0`).
3. **Well-named variables, labels, and constants** — locals follow the
   `RegisterName_description` pattern; labels use plain English (`dt_exit`,
   not `label_1`); magic numbers are replaced by named constants.
4. **Documented variables** — every local variable declaration carries a
   `/* intent (was X) */` comment. All declarations appear at the top of the
   outermost scope block (never inside an inner `if`/`for`/etc.), one per
   line, and are ordered by first use within the function.
5. **Has a prologue** — the function is preceded by a `/** ... */` Doxygen
   block.
6. **Prologue has a description** — the first line gives the Z80 address and
   function name; a following paragraph describes what the function does and
   any important context.
7. **Prologue documents parameters** — each parameter has a `\param[in]` (or
   `[out]`/`[in,out]`) line; include the originating Z80 register in
   parentheses where applicable.
8. **Conv: comments** — every deliberate departure from a literal Z80
   translation is marked `// Conv: ...` explaining what changed and why.

Once all eight criteria are met, add `[Conv: HQ]` to the end of the first
line of the prologue (the `$XXXX: name` line). This makes HQ status
greppable: `grep "\[Conv: HQ\]" ChaseHQ/Engine/Main.c`.

## Verifying translations

When a C translation looks wrong or a variable appears uninitialised, consult the skool file (`ChaseHQ.skool` or the bank files). The skool is the authoritative disassembly. Pay particular attention to:

- Which register holds what value at each Z80 address — registers are reused and a "was B" comment tells you the register name, not which logical value it held at that moment.
- `EX AF,AF'` / `EXX` banking: a value banked before a branch may arrive at a label with a different register than you expect. Be especially careful when a shadow register is used as a shuttle (e.g. `EX AF,AF'` passes a value through A' so that the main A can hold something else on the other side of a block). The two sides of the exchange hold logically different values even though both are named `A`.
- Self-modifying instructions (`SM $xxxx` annotations in `chqstate`): these are the C equivalent of Z80 code patching itself at runtime.
- **Pointer arithmetic writes are invisible to literal-address grep.** A `LD (HL),A` after `INC L` (or `INC HL`, `ADD HL,DE`, etc.) will not appear in search results for that address. When "nothing writes `$XXXX`" from a grep, check whether any loop or sequence advances a pointer past the end of a named table and lands on the target. The `build_height_table` write to `$E34C` is a worked example.

## Common translation bugs

See `C/docs/translation-pitfalls.md` for the full catalogue with commit references.
The most frequently recurring mistakes:

**Signed/unsigned type mismatch** — use `s8` (not `u8`) whenever the Z80
treats a byte as signed: subtraction can underflow, or a `JP M`/`JP P`
branches on the result. A `u8` variable can never be `< 0`, so
`if (X < 0)` on `u8` is always false and the negative-direction branch
is dead code. When in doubt, check whether the Z80 sets the Sign flag on
this value.

**Stale register / wrong variable** — Z80 reuses A (and others) for
different logical values within one function. Give each logical value its
own C name (`Aheight_diff`, not just `A`) and reference it at all sites
where the original register is still in scope. Using the generic name
after it has been overwritten is a silent bug.

**Missing SM field initialisation** — every `(SM)` field added to
`chqstate_t` must also be initialised in `chq_initialise` in
`Create.c`. Zero from `calloc` is not always the correct default.

**Pointer arithmetic direction** — Z80 SP decrements on PUSH; `SUB $20`
subtracts (not adds) 32; `JR NC` skips on _no_ carry (i.e. the
no-borrow branch). Confirm the direction before translating any
backbuffer or screen-pointer arithmetic.

**Pointer type → element stride** — Z80 `LD A,(HL)` is always a byte
load. The C pointer must be `u8*`. A `u16*` pointer doubles the offset
and reads out of bounds.

**Accumulate vs. assign** — `ADD A,IXl; LD IXl,A` accumulates A into
IXl across iterations. The C translation must keep a running sum; a bare
`IXl = A` (overwrite) discards the accumulated value.

**Borrow detection via bit 7** — `(result & 0x80)` only reliably detects
borrow when the difference fits in −128..+127. For differences above 128
the u8 wraps and bit 7 is wrong. Use a direct comparison instead:
`if (a <= b)` rather than `if ((a - b) & 0x80)`.

**Macro used as expression** — a macro defined as a pure expression
(`((base)[...])`) is a no-op when used as a statement. Mutating macros
must assign back to their argument: `((ptr) = ...)`.

**`JR NZ` / `JR Z` branch direction** — `JR NZ, label` skips *to* label
when non-zero, so the fallthrough code runs when zero. Translate as
`if (reg == 0) { ... }`, not `if (reg != 0) { ... }`. Getting this
backwards produces dead code on one branch and spurious execution on the
other (`update_road_level` jump launch: `if (Ay_offset)` → `if (!Ay_offset)`).

**SBC carry chain** — Z80 `SBC HL,DE` takes carry as borrow input. A
sequence of two `SBC` instructions means the carry from the first feeds
the second. In C, update `carry` after every subtraction that feeds a
subsequent `if (carry)` test; the C variable never propagates
automatically between statements.

**`INC A; INC A; JP NZ` loop** — terminates when A wraps (u8) to zero,
not when it decrements to zero. Use `do { … A += 2; } while (A != 0)`
with `u8 A`. Never add `--A` to the condition; that turns a +2 step into
a net +1 step and doubles the iteration count.

**`LD A,E` swap before computation** — when the Z80 does `LD A,E` just
before a processing block, it is using the *old* E value as the working
quantity, not whatever A currently holds. Identify which logical value the
block operates on and use that C variable throughout.

**`LD SP,HL; PUSH × N` backward fill** — `PUSH` decrements SP before
writing, so N pushes fill 2N bytes *before* the pointer, not after it. In
C: `memset(ptr − 2*N, value, 2*N)`, not `memset(ptr, …)`. When the Z80
uses `JP (IX)` to enter a PUSH chain at position `start` (variable-N),
compute `n = max − start` then `memset(ptr − 2*n, value, 2*n)` — no
switch needed (see pitfall #21).

**`LD SP,HL; POP × N` sprite copy** — the Z80 uses SP = bitmap source and
POP to load sprite bytes two at a time. When bytes are written verbatim
(no mask, no flip table), the C equivalent is `memcpy(dst, src, n)` where
n is derived from the jump-table entry index. Masked or flipped sprites
are not candidates (see pitfall #21).

**`JP M` / `JP P` as conditional skip, not loop** — `JP M, addr` jumps
*forward* to `addr` when the Sign flag is set (result negative). A
sequence of two CALLs linked by `JP M` is two sequential operations with
a conditional skip over the first, not a loop. Translating it as
`for(;;)` produces an infinite loop when the skipped call's row-count
is 0 (`draw_object_clipped` `$9404–$941D`).

**`Conv: NOT a loop` — verify back-edges before annotating** — before
writing a `Conv: NOT a loop` comment, grep the skool for every `JP`,
`JR`, and `DJNZ` that targets an address inside the block. A single
unconditional back-jump makes the block a genuine loop regardless of how
sequential the surrounding code looks. `draw_object_clipped` `$9417 JP
$9404` was missed this way; the block is structurally identical to the
adjacent masked-rows loop (see pitfall #20).

**`JR Z` + `JR NC` two-exit sequence — combined condition is `<= 0`** —
when `SUB C` is followed by `JR Z,exit` (clamp to 1 on zero) then `JR
NC,keep` (keep value when positive), the block that falls through runs
only when the result is negative. Together the two exits mean clamp when
result ≤ 0. Translating as `if ((s8)Avertical < 0)` drops the zero case
(`draw_stretchy_object_common` `$9220–$9222`). Fix: `if (Avertical <= 0)`
(see pitfall #8).

**`DEC HL` adjusts the pointer, not the value** — Z80 `LD A,(HL); DEC HL;
DEC HL` reads the value at HL then moves the pointer back 2 bytes. The
C equivalent is `A = *HL; HL -= 2;`. Writing `A = *HL - 2` or
`*HL -= 2` instead modifies the *value* and leaves the pointer unchanged
(`draw_stretchy_object_common` `$9237`: `doc_rows_2nd = width_bytes - 2`
instead of `doc_rows_2nd = *ptr; ptr -= 2`).

**Signed Z80 register used in arithmetic — cast to `(s8)` at the use site**
— when a Z80 register holds a signed offset (e.g. a self-modified `D`
representing a column adjustment), `ADD A,D` in Z80 is a signed addition.
In C, if `D_col_pos` is declared `u8`, `+= D_col_pos` treats it as
unsigned (0–255). Declare such fields `s8`, or cast at the use site:
`Arange += (s8)D_col_pos`. Do not substitute a bit-7 conditional — that
only works for values in [−128, +127]; `(s8)` cast is always correct
(`draw_object_clipped` `$941F` `D_col_pos` sign-extension bug).

**`ADD A,B; RET C` is an overflow guard, not a comparison** — `RET C`
returns when the 8-bit addition overflows (`A + B > 255`). Translating
this as `if (A < B) return` fires on a completely different (and nearly
opposite) condition. Fix: add first, then check `if (result > 255) return`
(`draw_object_right_stretchy_entrypt` `$9306–$9307`).

**`RET Z` / `RET NZ` early-exit polarity** — `RET Z` returns when the
register is zero. `if (value) return` inverts this: the function exits
when there is work to do and only falls through when there is nothing to
do. Fix: `if (!value) return` (`scroll_horizon` `$B8A6`).

**`EX AF,AF'` banked accumulator — assign to the correct variable** — after
a loop that banks one accumulator in A' via `EX AF,AF'`, the paired `EX`
at exit hands A' back to the main register. The C variable that was
accumulating in A' must be assigned from the retrieved A, not from the
variable that was live in A just before the swap. Getting this wrong silently
swaps two state-field updates (`scroll_horizon` `$B8CB`: `horizon_y_accum`
and `horizon_y_step` were exchanged).

**`EX AF,AF'` restores flags — `JP P`/`JP M` tests the banked value's sign** —
`EX AF,AF'` swaps the full flag register F as well as A. A `JP P` or `JP M`
immediately after the EX tests the *restored* flags, not flags from any
instruction that ran since the last bank. When a new value is loaded into A
between the EX and the branch (`LD A,C; JP P`), the branch is independent of
that value. Trace back to the flag-setting instruction before the original
`EX AF,AF'` to find what is actually tested (`scroll_horizon` `$B874`:
`JP P` tests the sign of `current_curvature`, not the table byte in C).

## Known data layout: $E34B–$E34D horizon attribute scroll

`state->horizon_attr[3]` (Z80 `$E34B–$E34D`) drives the per-frame sky/ground colour boundary update in `update_screen` (`ds_attributes`, `$BD5A`):

| Field | Z80 | Role |
|-------|-----|------|
| `horizon_attr[0]` | `$E34B` | Previous rounded Cmin (written by `build_height_table` each frame) |
| `horizon_attr[1]` | `$E34C` | Current frame's delta: `new_round − old_round`, multiples of 8 |
| `horizon_attr[2]` | `$E34D` | Previous frame's delta (one-frame lag used by `ds_attributes`) |

**Who writes `$E34C`:** `build_height_table` (`$CDD1–$CDD4`) — the "final bytes" that follow the 21-entry `bht_loop2`. After writing the 21 clamped heights to `$E336–$E34A`, HL points to `$E34B`; the code then writes `C` (rounded Cmin) to `$E34B` and `A = C − old_$E34B` to `$E34C`. In the C port this happens via `pdst = &clamped_heights[21]` which aliases `horizon_attr[0]` by struct layout (both `u8`, consecutive, no padding).

**This write is invisible to a literal-address grep for `$E34C`** — it is reached through pointer arithmetic ending at that address, not a hardcoded `LD ($E34C),A`. Any similar "who writes X?" search must check for pointer arithmetic that ends at the target address, not just literal-constant stores.

**`ds_attributes` logic:** reads the *previous* frame's delta (`$E34D`/`horizon_attr[2]`) to decide whether to act; uses the *current* frame's delta (`$E34C`/`horizon_attr[1]`) as the movement amount for the *next* frame (one-frame lag). The pointer `$A186` (`session.horizon_attribute`) is a Z80 address pointing to the **last byte** (byte 31) of the current sky/ground boundary row. The Z80 fills 30 bytes backward via `LD SP,HL; PUSH BC × 15`; in C this is `memset(ptr − 30, colour, 30)`.

**Initialisation:** `load_scene` (around `$880A`) sets `horizon_attr[0] = 8`, `[1] = 0`, `[2] = 0`. `build_height_table` overwrites `[0]` and `[1]` on the first call; the `8` only matters as the reference point for that first delta.

## Known data layout: $E4xx road graphics page

`edge_markings` in `C/libraries/ChaseHQ/Data/CommonData.c` is a single 256-byte array that represents the entire `$E4xx` Z80 memory page:

| Offset | Z80 address | Content |
|--------|------------|---------|
| `0x00..0x0F` | `$E400..$E40F` | Zeros — Z80 "draw nothing" sink (stripe offset = 0 points here) |
| `0x10..0xCF` | `$E410..$E4CF` | Edge markings: six 32-byte masked variants (widest→thinnest, white/black) |
| `0xD0..0xFF` | `$E4D0..$E4FF` | Lane markings: three 16-byte unmasked variants (widest→thinnest) |

All road-marking access sites use `&edge_markings[((0xE4 << 8) | Ldash) - 0xE400]` = `&edge_markings[Ldash]`. There is no separate `lane_markings` array. `dr_edge_graphic_offset` ∈ {`0x10`, `0x30`, `0x50`, `0x70`, `0x90`, `0xB0`} and `dr_stripe_table_offset` ∈ {`0x00`, `0xD0`, `0xE0`, `0xF0`} are low bytes of Z80 HL and index directly into this array.

## Tests

All tests pass.

## Safe Editing

- Prefer narrow edits in data files or isolated helpers; avoid broad rewrites inside `ChaseHQ/Engine/Main.c`
- When modifying `chqstate_t`, update initialisation in `ChaseHQ/Engine/Create.c` (`chq_initialise`)
- When touching `libraries/ZXSpectrum/Spectrum.c` locking or dirty-rect code, validate both correctness and host callback behaviour — it is cross-thread glue
