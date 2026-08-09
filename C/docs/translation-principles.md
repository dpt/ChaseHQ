# Z80 → C Translation Principles

Guiding rules for translating ZX Spectrum Chase H.Q. Z80 assembly into C.

---

## Purpose

This is a teaching project. The goal is a C version of Chase H.Q. that a
sixteen-year-old could read and learn from — someone curious about how games
worked, not a professional programmer. Every decision should serve that goal.

The C code exists to explain the original Z80 game. It is not a rewrite or an
improvement; it is a translation. Where the Z80 does something surprising, the C
should explain it in plain British English, not hide it.

---

## Variable types

Use the provided short form types like `s8`, `u16` and so on.

Use `int` for local variables unless the value **relies on 8-bit wraparound**.
Wraparound is a correctness requirement when:

- The Z80 branches on carry or overflow (`JP C`, `JP NC`, `JP M`, `JP P`) and
  the branch depends on the value wrapping.
- The value is written to a Z80 byte-sized location (a state field, a table
  entry) that is later read as a byte.

When wraparound is needed, apply `& 0xFF` explicitly at the wrap point rather
than relying on `u8` truncation — this makes the wrapping visible to the reader.

Use `s8` (not `u8`) when the Z80 treats the byte as signed. See
`translation-pitfalls.md` §1.

---

## Code order

**Keep the C in the same order as the Z80**, instruction by instruction. Do not
reorder for readability or efficiency unless keeping the original order would
require contortions that actively obscure the meaning (e.g. a deeply nested
forward reference). When order must change, add a
`/* Conv: reordered because ... */` comment.

The reader will be cross-referencing the C against the skool disassembly.
Unexpected reordering breaks that cross-reference.

---

## Control flow

**Prefer `if`/`else if`/`else` chains** that mirror the Z80 branching structure.
Nested `if-else` is preferable to guard clauses, early returns, or inverted
conditions, because the nesting makes the branch structure visible.

**`DJNZ` loops become `do { ... } while (--B);`** — not `for` loops. The Z80
`DJNZ` always executes at least once; `do-while` preserves that property and
names the register explicitly. `for` loops are idiomatic C but disguise the Z80
structure.

The Z80 may use sequences of decrements and tests to replicate a `switch`
statement. Converting that may be risky. Perhaps leave those?

---

## Factoring and macros

Repeated code sequences may be factored into a macro **when the sequence appears
three or more times and the pattern is mechanically identical**. Name the macro
after what the Z80 is doing, not after the C operation.

Do not factor for style or conciseness alone. A repeated five-line sequence that
appears twice is better left as two copies with matching comments than collapsed
into an abstraction that the reader must look up.

---

## Bugs and quirks

**Bugs in the original game must be preserved.** The C port documents the game
as it was, not as it should have been. Where a bug is known, add a comment:

```c
/* Conv: Z80 bug preserved — ... */
```

Where the Z80 has an effect that cannot be expressed cleanly in C (e.g. a
self-modifying instruction, a stack trick, a timing dependency), implement a
faithful approximation and mark it:

```c
/* Conv: workaround for ... */
```

If a workaround diverges intentionally from the Z80 behaviour, explain why in
the comment. Do not silently fix things.

---

## Backporting

Remember to backport findings to the skool file disassembly.
