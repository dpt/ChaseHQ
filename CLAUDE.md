# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a disassembly and C port of the ZX Spectrum 128K game "Chase H.Q." by Ocean Software. The project has two parallel strands: reverse engineering via SkoolKit, and a faithful C reimplementation of the game logic.

## Building

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
cmake -S . -B cmake-build-debug
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

Tests live in `C/Tests/TestDrawRoad.c`. They are built with `-DCHQ_TESTS`, which compiles in thin wrappers at the bottom of `ChaseHQ.c` (inside `#ifdef CHQ_TESTS`) that expose static functions for direct testing. Declarations for those wrappers live in `C/ChaseHQ/ChaseHQ-Tests.h`. When adding a new test hook, add the wrapper to `ChaseHQ.c` and declare it in `ChaseHQ-Tests.h`.

## C Implementation Architecture

### Entry point and lifecycle
`C/Main.c` owns the SDL window and event loop. Each frame it calls `chq_main(state->game)`. The public game API is in `C/ChaseHQ/ChaseHQ.h`:

```
chq_create → chq_setup → chq_main (repeated) → chq_destroy
```

### Layers
- **Host** (`C/Main.c`): SDL2 window, event loop, `zxconfig_t` callbacks wired to game
- **ZX emulation facade** (`C/ZXSpectrum/Spectrum.*`): exposes `in`/`out`/`draw`/`stamp`/`sleep` callbacks; game code never calls SDL directly
- **Game** (`C/ChaseHQ/ChaseHQ.c`): translation-oriented, heavily commented with Z80 addresses; many TODOs and partial stubs
- **State** (`C/ChaseHQ/ChaseHQ-State.h`): `struct chqstate` — the single source of mutable game state, fields ordered by original Z80 memory addresses
- **Stage data** (`C/ChaseHQ/ChaseHQ-Stages.h`, `ChaseHQ-Stage1Data.*`, `ChaseHQ-CommonData.*`): read-only game tables consumed by game logic

### Data flow
- **Rendering**: game mutates `state->speccy->screen` → ZX facade tracks dirty regions → `draw_handler` in `Main.c` calls `zxspectrum_claim_screen` → SDL texture update
- **Input**: SDL keys → `zxkeyset_t`/`zxkempston_t` → `key_handler` → Spectrum IN ports (`port_KEYBOARD_*`, `port_KEMPSTON_JOYSTICK`)
- **Audio/border**: hooks are connected but most are placeholder NOPs in `Main.c`

### Stage status
All 5 stages currently map to `stage1` data in `ChaseHQ-Stages.c`. Per-stage data beyond stage 1 does not exist yet.

## Coding Conventions

- **Types**: use `u8`, `u16`, `s8`, `s16` from `C/C99/Types.h` in game and state code
- **State**: never introduce globals; pass and mutate `chqstate_t *state` throughout
- **`(SM)` fields**: each field annotated `(SM)` in `chqstate` corresponds to a Z80 self-modifying instruction at the given address — these are correctness-critical; do not remove or rename carelessly
- **`Conv:` comments**: mark where the C version intentionally diverges from a direct Z80 translation; preserve them
- **Address semantics**: macros like `ADDRTOSCREEN`, `BACKBUFTOOFFSET`, `ROADBUFPTR` are correctness-critical
- **Formatting**: K&R style, 2-space indent, 80 columns, pointer aligned to name (`.astylerc`)
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

## Verifying translations

When a C translation looks wrong or a variable appears uninitialised, consult the skool file (`ChaseHQ.skool` or the bank files). The skool is the authoritative disassembly. Pay particular attention to:

- Which register holds what value at each Z80 address — registers are reused and a "was B" comment tells you the register name, not which logical value it held at that moment.
- `EX AF,AF'` / `EXX` banking: a value banked before a branch may arrive at a label with a different register than you expect. Be especially careful when a shadow register is used as a shuttle (e.g. `EX AF,AF'` passes a value through A' so that the main A can hold something else on the other side of a block). The two sides of the exchange hold logically different values even though both are named `A`.
- Self-modifying instructions (`SM $xxxx` annotations in `chqstate`): these are the C equivalent of Z80 code patching itself at runtime.
- **Pointer arithmetic writes are invisible to literal-address grep.** A `LD (HL),A` after `INC L` (or `INC HL`, `ADD HL,DE`, etc.) will not appear in search results for that address. When "nothing writes `$XXXX`" from a grep, check whether any loop or sequence advances a pointer past the end of a named table and lands on the target. The `build_height_table` write to `$E34C` is a worked example.

## Common translation bugs

See `C/translation-pitfalls.md` for the full catalogue with commit references.
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
`ChaseHQ-Create.c`. Zero from `calloc` is not always the correct default.

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
C: `memset(ptr − 2*N, value, 2*N)`, not `memset(ptr, …)`.

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

`edge_markings` in `C/ChaseHQ/Data/ChaseHQ-CommonData.c` is a single 256-byte array that represents the entire `$E4xx` Z80 memory page:

| Offset | Z80 address | Content |
|--------|------------|---------|
| `0x00..0x0F` | `$E400..$E40F` | Zeros — Z80 "draw nothing" sink (stripe offset = 0 points here) |
| `0x10..0xCF` | `$E410..$E4CF` | Edge markings: six 32-byte masked variants (widest→thinnest, white/black) |
| `0xD0..0xFF` | `$E4D0..$E4FF` | Lane markings: three 16-byte unmasked variants (widest→thinnest) |

All road-marking access sites use `&edge_markings[((0xE4 << 8) | Ldash) - 0xE400]` = `&edge_markings[Ldash]`. There is no separate `lane_markings` array. `dr_edge_graphic_offset` ∈ {`0x10`, `0x30`, `0x50`, `0x70`, `0x90`, `0xB0`} and `dr_stripe_table_offset` ∈ {`0x00`, `0xD0`, `0xE0`, `0xF0`} are low bytes of Z80 HL and index directly into this array.

## Known test failure

All five tests pass.

## Safe Editing

- Prefer narrow edits in data files or isolated helpers; avoid broad rewrites inside `ChaseHQ/ChaseHQ.c`
- When modifying `chqstate_t`, update initialisation in `ChaseHQ/ChaseHQ-Create.c` (`chq_initialise`)
- When touching `ZXSpectrum/Spectrum.c` locking or dirty-rect code, validate both correctness and host callback behaviour — it is cross-thread glue
