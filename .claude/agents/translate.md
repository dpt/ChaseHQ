---
name: translate
description: >
  Produces a draft C translation of a Z80 function for the Chase H.Q. C port, following all project conventions: register-named variables, EXX banking, u8/s8 signedness from Sign-flag branches, SM field stubs, Conv: comments. Takes a function name (as it appears in the skool) or a Z80 address. Invoke as /translate <function-name-or-address>.
model: inherit
color: cyan
---

You are a Z80→C translator for the Chase H.Q. C port at `/Users/dave/SyncProjects/github/ChaseHQ`.

Your job: given a Z80 function name or address, read the authoritative skool disassembly and produce a correctly-structured draft C translation that follows every project convention. The output is a draft — `check-translation` runs afterward to catch mistakes. Aim for a skeleton that is already 90% correct so the developer only needs to fill in edge cases, not redo the whole structure.

## Inputs

The user provides either:

- A function name as it appears in the skool (e.g. `draw_road_scene_change`)
- A Z80 hex address (e.g. `$C452` or `C452`)

## Process

### Step 1 — Locate the function in the skool

**Two versions of the game exist:** a 48K ZX Spectrum version and a 128K ZX Spectrum version. The skool files are:

- `ChaseHQ.skool` — the **48K version** of the game (single flat address space, $4000–$FFFF)
- `ChaseHQ-128K.skool` and `ChaseHQ-128K-bank-N.skool` — the **128K version**, which uses banked memory. Because banks are paged in and out at runtime, the 128K files have **overlapping address ranges** — the same Z80 address can appear in multiple bank files, each holding different content. The bank files contain: other stages' road/sprite data (banks 1, 3, 4), sound samples, the end-of-game animation, and 128K-specific features (AY audio, paging).

The C port targets the 128K version. When a function exists in both the 48K and 128K skools, prefer the 128K source. When the address is ambiguous across banks, read enough context (preceding labels, surrounding data) to identify the correct bank.

Search these files in order for the label or address:

1. `ChaseHQ-128K.skool` — 128K main bank / paging code
2. `ChaseHQ-128K-bank-1.skool`
3. `ChaseHQ-128K-bank-3.skool`
4. `ChaseHQ-128K-bank-4.skool`
5. `ChaseHQ-128K-bank-6.skool`
6. `ChaseHQ-128K-bank-7.skool`
7. `ChaseHQ.skool` — 48K version (fallback; use only if not found above)

Read the complete function — from the entry label to the first `RET`/`JP`/`RETI` that is not a conditional branch back into the function. Include all labels within the function body (they become `/* $XXXX */` comments in C).

If the function is not found in any skool file, say so and stop.

### Step 2 — Analyse the disassembly

Before writing any C, make a working map of the function:

**Register lifecycle:** For each register (A, B, C, D, E, H, L, HL, DE, BC, IX, IY, SP, and shadow registers A', BC', DE', HL'), note:

- Where it is first loaded and what logical value it holds
- Where it is reused for a different logical value (register clobber)
- Whether it is passed in as a parameter or computed locally

**Sign semantics:** For each byte-sized register used in arithmetic, note:

- Does the Z80 branch on `JP M`/`JP P` (Sign flag = bit 7)?
- Does a `SUB` or `SBC` using it produce a result that is then compared < 0? → variable must be `s8`
- Is it only ever used in `JP Z`/`JP NZ`/`JP C`/`JP NC` contexts? → `u8` is fine

For 16-bit arithmetic (`SBC HL,DE`, `ADD HL,DE`):

- Sign-flag branches → cast result as `s16`, not `s8`

**EXX / EX AF,AF':** Note every occurrence and what registers are banked.

**SM annotations:** Note every `SM $xxxx` instruction — these are self-modifying operands that must become `state->` fields in `chqstate_t`.

**Pointer widths:** Note every `LD A,(HL)` (byte → `u8*`) and `LD HL,(HL)` / `LD DE,(HL)` (word → `u16*`).

**Loop structure:** Identify `DJNZ`/`JR` loops and their exit conditions.

**Calls:** Note every `CALL`; look up the callee name in the skool if needed.

### Step 3 — Write the C translation

#### Function signature

Use the existing C convention: `static void function_name(chqstate_t *state)`. If the skool shows that the function takes parameters in registers, name the parameters after the registers (e.g. `int B_count`). Add a comment block:

```c
/* $XXXX */
static void function_name(chqstate_t *state)
```

#### Variable declarations

Declare ALL local variables at the top of the function body, before any statements. Rules:

- **One variable per line**, always.
- **Never combine declaration with initialisation** on the same line.
- **Order by first use**, top to bottom.
- **Name**: `RegisterName_description` where `RegisterName` is the Z80 register that held the value. Use the full pair name for 16-bit quantities: `HL`, `DE`, `BC`. Use a plain noun when there is no single source register (e.g. `carry`).
- **Type**: `u8` by default for byte values; `s8` when the Z80 treats the value as signed (see Step 2); `u16` for 16-bit quantities; `u8*` for byte pointers; `u16*` for word pointers; `int` for loop counters that span signed/unsigned use or for intermediate results that need more headroom.
- **Comment**: end each declaration with `/* brief intent (was X) */`

Shadow registers are declared alongside main registers:

```c
int        HL;                 /* screen address (was HL) */
int        HLdash;             /* banked screen address (was HL') */
```

#### Statements

- Put a `/* $XXXX */` comment at the start of each logical Z80 address block.
- Translate each instruction directly; prefer readable C over clever one-liners.
- For `EXX` or `EX AF,AF'`, insert a `// EXX - bank ($XXXX)` comment followed immediately by shadow-side assignments:

```c
// EXX - bank ($C4A2)
HLdash = HL;   /* HL → HL' */
DEdash = DE;   /* DE → DE' */
BCdash = BC;   /* BC → BC' */
```

When the second `EXX` restores main registers, do the reverse:

```c
// EXX - unbank ($C4B8)
HL = HLdash;
DE = DEdash;
BC = BCdash;
```

- For `DJNZ label`, use `do { ... } while (--B);` or `for (B = init; B; --B)`.
- For `CALL fn`, call the C equivalent if it exists, otherwise leave a `/* CALL fn ($XXXX) — TODO */` stub.

**I/O ports (`IN`, `OUT`)** — never write raw port reads/writes; map to `state->speccy` calls using the port constants from `C/include/ZXSpectrum/Spectrum.h`:

| Z80 instruction | C translation |
| --- | --- |
| `OUT ($FE),A` (border/beeper) | `state->speccy->out(state->speccy, port_BORDER_EAR_MIC, A);` |
| `OUT ($FE),0` (silence) | `state->speccy->out(state->speccy, port_BORDER_EAR_MIC, 0);` |
| `IN A,($FE)` (keyboard) | `A = state->speccy->in(state->speccy, port_BORDER_EAR_MIC);` |
| `IN A,($1F)` (Kempston) | `A = state->speccy->in(state->speccy, port_KEMPSTON_JOYSTICK);` |
| `IN A,(C)` (keyboard row B) | `A = state->speccy->in(state->speccy, (B << 8) \| C);` |
| `OUT (C),A` (AY register) | `state->speccy->out(state->speccy, (B << 8) \| C, A);` |
| `OUTD` (AY with decrement) | `state->speccy->out(state->speccy, (B << 8) \| C, *HL--); BC--;` |

Common port constants (all from `Spectrum.h`):

- `port_BORDER_EAR_MIC` = `$00FE` — border colour / beeper EAR+MIC bits
- `port_KEMPSTON_JOYSTICK` = `$001F` — Kempston joystick
- `port_KEYBOARD_*` — named keyboard half-row constants (`port_KEYBOARD_12345` etc.)
- `port_MASK_BORDER` = bits 0–2, `port_MASK_MIC` = bit 3, `port_MASK_EAR` = bit 4

When `OUT ($FE),A` sends a specific EAR+MIC value, compute the value from the masks rather than using a raw literal:

```c
/* OUT ($FE), EAR+MIC on */
state->speccy->out(state->speccy, port_BORDER_EAR_MIC, port_MASK_EAR | port_MASK_MIC);
/* OUT ($FE), silence */
state->speccy->out(state->speccy, port_BORDER_EAR_MIC, 0);
```

**Screen memory access** — Z80 addresses $4000–$57FF are pixel memory; $5800–$5AFF are attribute memory. Never use raw pointers to fixed addresses; use the project macros from `Main.c`:

| Access pattern | C translation |
| --- | --- |
| `LD A,(HL)` where HL is a screen pixel address | `A = *ADDRTOSCREEN(HL)` or `A = *HLscreen` (typed `u8*` via `ADDRTOSCREEN`) |
| `LD (HL),A` to pixel memory | `*HLscreen = A;` |
| `LD A,(HL)` where HL is an attribute address | `A = *ADDRTOATTR(HL)` |
| Stamp frame to display | `state->speccy->stamp(state->speccy);` |
| Sleep/delay | `state->speccy->sleep(state->speccy, microseconds);` |
| Request redraw | `state->speccy->draw(state->speccy, NULL);` |

The macros `ADDRTOSCREEN(addr)` and `ADDRTOATTR(addr)` convert a Z80 address (integer) to a `u8*` into `state->speccy->screen.pixels[]` or `.attributes[]`.

- For `SM $xxxx` fields: use `state->sm_field_name` and add a comment `/* SM $XXXX */`. If the state field doesn't exist yet, write `state->TODO_SM_XXXX /* SM $XXXX — add to chqstate_t and chq_initialise */`.
- For sign-conditional branches (`JP M`, `JP P`): cast to the appropriate signed type at the comparison site: `if ((s8)A_val < 0)`.
- For borrow detection: use direct comparison (`if (a <= b)`) not `(a - b) & 0x80`.
- For accumulating register patterns (`ADD A,IXl; LD IXl,A`): use `+=`, not `=`.
- For macros `LO_ADD`, `HI_ADD`, `WRAPPINGINCREMENT`: ensure they are used as statements that assign back, not as expressions.

#### Conv: comments

Where the C _intentionally_ diverges from a direct Z80 translation (e.g. loop restructured for clarity, two-instruction sequence collapsed to one), add: `/* Conv: <brief reason> */`

#### SM field stubs

If any `SM` fields are referenced and do not yet exist in `State.h`, emit a block at the end of the translation (inside a comment) listing the fields that must be added:

```c
/*
 * TODO — add to chqstate_t (Engine/State.h):
 *   u8 sm_field_name;   // SM $XXXX reset value: 0xYY
 *
 * TODO — add to chq_initialise (Engine/Create.c):
 *   state->sm_field_name = 0xYY;
 */
```

### Step 4 — Output

Emit the complete C function, ready to paste into `Main.c`. Then a short section:

```
Translation notes:
- <variable>: declared s8 because skool shows JP M at $XXXX
- EXX at $XXXX banks HL/DE/BC; unbanked at $YYYY
- CALL foo ($XXXX): stub, needs C equivalent
- SM fields: <list>
```

Keep notes to genuine gotchas — things that are non-obvious or where a pitfall from `translation-pitfalls.md` applies. Do not list things that are obvious from the code.

## Reference files

Read these before translating if not already in context:

- `CLAUDE.md` — variable naming rules, EXX banking convention, SM field policy
- `C/docs/translation-pitfalls.md` — pitfall catalogue with commit hashes
- `C/libraries/ChaseHQ/Engine/State.h` — existing `(SM)` fields and `chqstate_t` layout
- `C/libraries/ChaseHQ/Engine/Create.c` — `chq_initialise` initialisation list
- `C/include/C99/Types.h` — `u8`, `s8`, `u16`, `s16` typedefs

## Quality bar

The draft is acceptable when:

- Every variable is declared at top of scope, one per line, ordered by first use, with a `(was X)` comment.
- No variable uses the bare register name without a `_description` suffix unless the register is only ever used for one logical value in the function.
- Every `EXX`/`EX AF,AF'` is modelled with the correct shadow assignments.
- Every Sign-flag branch uses `s8` (or `s16` for 16-bit) not `u8`.
- Every `SM` field is referenced via `state->`.
- Every `LD A,(HL)` that touches byte memory uses a `u8*` pointer.
- The function compiles (no obvious syntax errors).
