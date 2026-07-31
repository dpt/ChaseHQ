# Chase H.Q. — Facts & Trivia

Note: AI "facts" will need checking.

## "Turbo" is actually nitrous oxide

Chase H.Q.'s turbo boost mechanic — finite uses, instant speed surge on button press, refills between stages — is the behaviour of **nitrous oxide injection (NOS)**, not a turbocharger.

A turbocharger is always running under load; it uses exhaust gases to spin a compressor that forces more air into the cylinders. There is no on/off switch — boost builds passively with RPM.

Nitrous oxide injection is the push-a-button-go-faster mechanism: N₂O is injected into the intake, releasing extra oxygen, allowing more fuel to be burned, producing a sudden sharp power spike from a finite tank.

Taito called it "turbo" because in 1988 "turbo" was the glamour word (turbo Ferraris, turbo Saabs). The confusion was endemic to that era of gaming and persists today.

## The pregame meters are fake radio signal meters

The briefing screen is a police radio scanner, not a dashboard, and the two seven-segment bars on it are signal-strength meters: green cells for signal, red for noise.

They are not measuring anything. `animate_meters` (`$860F`) calls the random-number generator once per bar each frame and uses only the sign of the result: negative steps the level down by one, positive steps it up by one, clamped to 0–7. There is no underlying signal value the bar is displaying — the displayed level _is_ a random walk. `am_set_attrs` (`$8646`) then paints the green/red split for whatever level the walk has reached. Both bars run the same code independently, so they drift out of phase and the panel looks like live radio traffic for the cost of two random bytes a frame.

## The Z80 has no multiply instruction, so Chase H.Q. built one from three shifts

The Z80 CPU has no hardware multiply. The perspective calculation needs to multiply a row index by a curvature value, so the game implements `multiply()` (`$CDD6`) using a 3-iteration shift-and-add loop: each iteration rotates the multiplier left, tests the bit that fell out, and if set adds the multiplicand to the accumulator before doubling it. After three iterations a final rounding step is applied. Only the top 3 bits of the multiplier are ever examined — which is sufficient because the perspective table index only ever uses those bits. The `#else` branch in the C port makes this explicit: `(((a & 0xE0) >> 5) * c) >> 2`.

## The helicopter uses a spring and a bob to fake complex movement

`move_helicopter()` (`$AAC6`) produces what looks like convincing hovering behaviour from three independent one-liners:

1. **Descent**: height decreases by 2 each frame until it reaches 97.
2. **Road-tracking spring**: the helicopter's horizontal road position is nudged ±8 pixels per frame toward the road centre (pixel 112), clamping when it overshoots.
3. **Vertical bob**: a direction flag flips every 4 animation frames, toggling a running offset that pushes the sprite up and down.

No floating-point, no trigonometry, no state machine — just comparisons and addition.

## The player's car is (probably) a Porsche 928

Taito never officially named the vehicle in Chase H.Q. It is an unlicensed lookalike, not a branded car. The consensus among fans is that it is based on the **Porsche 928** — the distinctive wide rear haunches, fastback roofline, and proportions match closely. The ZX Spectrum port inherits the same ambiguity. Treat this as the best available reading rather than a confirmed fact until a primary source surfaces.

## The blacked-out playfield edges hide sloppy clipping

The left and right margins of the playfield are painted black in the border, but the columns beneath them are real backbuffer memory, not dead space. Road, sprites and objects are drawn without careful edge-clipping in some paths, so drawing routines regularly overwrite those margin columns in the backbuffer rather than checking bounds every time. It doesn't matter, because those columns are never copied from the backbuffer to the visible ZX screen — only the playfield proper gets blitted out. The black border sits on top and hides the fact that garbage is being written underneath it. Reveal those pixels (e.g. widen the playfield or blit the full backbuffer) and the overdrawn garbage becomes visible.

## The game is spiritually a Miami Vice product

Chase H.Q. (Taito, 1988 arcade) fits squarely in the Miami Vice cultural moment: plainclothes cops, a white European sports car, pursuing criminals rather than racing competitively. The TV show ran 1984–1990 and the "undercover cop in an exotic car" aesthetic dominated that window. Taito never cited Miami Vice as a direct source, but the thematic overlap is hard to attribute to coincidence. Chase H.Q. may also be downstream of a broader trend — Miami Vice itself borrowed heavily from _To Live and Die in L.A._ (1985) and similar 80s material.

## Taking the wrong fork scores you points for the privilege

When the road forks, the game decides which branch you took from your position on the road and compares it against the stage's `correct_fork` byte (`layout_road`, `$B9F4`). Take the correct one and Tony says "let's go". Take the wrong one and Raymond tells you so, the perp's speed is raised from its normal 60 to 95 — and you are awarded a bonus of 40,000 + 10,000 × stage number points.

The asymmetry is in the original binary, not a porting slip: `$BA95` jumps the correct-fork case straight to the chatter at `$BAAA`, skipping the `add_bonus` call at `$BAA4` entirely. Only the wrong-fork path falls through it. So the punishment for a wrong turn comes with a score bonus, and the correct turn scores nothing.

## Bonuses containing an interior zero print wrong

`add_bonus` (`$9CD6`) formats a six-digit packed-BCD bonus into a string via its digit-printing helper `bonus_digit` (`$9CFC`), which suppresses leading and trailing zeros. The zero-suppression walks the digits and terminates the chain on the first non-zero-to-zero transition — which means a bonus with a zero _inside_ it, such as 50,500, is not rendered correctly. The score itself is added properly; only the on-screen figure is mangled. Original bug, faithfully preserved in the port.

## Ramming the perp again while already spinning shortens the crash

A crash ends when the hero car's speed decays below `ahc.crash_speed_threshold`, which `scenery_hit` (`$A4B8`) sets to min(cap, speed at impact) — so a fast crash spins longer. But `scenery_hit` returns immediately if you are already crashed, while its caller, `perp_behaviour` (`$A637`), keeps going and adds 40 to the threshold anyway (`$A7A8`). Every extra impact during a spin therefore raises the bar the speed has to fall under, ending the spin sooner. The cap itself is also softer when boosting (230 rather than 200), so a crash taken under turbo recovers faster than the same crash taken without.

## Continuing a stage cuts your smash bonuses to a tenth

The perp-smash bonus is assembled as packed BCD in registers D (the ten-thousands pair) and E (the hundreds pair), inside the same `perp_behaviour` (`$A637`) routine that handles the crash above. When `retry_count` is non-zero — you continued rather than cleared the stage first time — the code moves D's value into E with a nibble swap and zeroes D (`$A7C2`). That single shuffle divides the points awarded by ten. The stage-clear bonus at the end of a stage does the same thing, dropping from ×100,000 to ×10,000.

## The game rewrites its own code 133 times

`chqstate_t` carries 133 fields marked `(SM)`, each standing in for a Z80 instruction that patches its own operand bytes at runtime. Self-modifying code was the standard trick for making an 8-bit CPU fast: writing a new address into a `LD (nn),HL` costs less than keeping the pointer in a variable and reloading it every pass. It also makes the disassembly hostile to read, because the instruction you are looking at is not the instruction that executes.

## The original could write off the end of the screen and nobody noticed

Several routines clamp one edge of something and forget the other. In bank 3, `compute_glyph_blit_params_fg` clips where a title-screen glyph starts drawing if it would begin above the screen — but the routine that actually draws each scanline, `blit_glyph_rows` (`$C9D5`), has no matching check at the far end. A fast-moving title-screen object can walk its screen address off the bottom of the display file with nothing to stop it.

The hit-wobble table has the same shape of gap. `hazard_hit` (`$AC3C`) drives a barrier or tumbleweed's shudder after a collision by stepping an index through a 40-byte table of wobble amplitudes (`$ACDB`) once per frame. The slowest speed bracket starts that index at 20 with a countdown already at 0, which wraps to 255 on the very first frame and marches the index straight past the end of the table. The road drawing code does it too. `draw_road`'s unfilled-scanline writer (`$C565`, `dr_write_scanline_unfilled` in `draw_road`, `$C452`) takes a back-buffer row address in DE and writes to it without checking that the address is still inside the back buffer, so a sufficiently extreme road layout aims the write below the buffer entirely.

None of it mattered on real hardware: a stray write into `$0000–$3FFF` lands in ROM and is a no-op, a stray read past the end of a table just picks up whatever byte happens to sit after it in memory, and a stray write elsewhere in RAM is a byte of garbage that gets overwritten next frame. The C port cannot be so relaxed — its screen is a struct, not flat memory, and reading past the end of an array is undefined behaviour rather than a harmless neighbouring page — so each of these sites is explicitly bounds-checked with a `Conv:` note explaining what the Z80 got away with.

## A dead branch in the title screen means objects can never go idle

The title-screen object scripts use opcodes `$C9–$CF`. Object 0 of every scene emits `OSS_OP_DEAD`, which is outside that range, and the dispatch inside `object_script_step` (`$C72F`) jumps past the countdown code for any unrecognised opcode. The object's wait counter is therefore never decremented, so it can never reach zero and return the object to idle. Another original-game quirk, reproduced rather than fixed.
