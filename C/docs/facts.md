# Chase H.Q. — Facts & Trivia

Note: AI "facts" will need checking.

## "Turbo" is actually nitrous oxide

Chase H.Q.'s turbo boost mechanic — finite uses, instant speed surge on button press, refills between stages — is the behaviour of **nitrous oxide injection (NOS)**, not a turbocharger.

A turbocharger is always running under load; it uses exhaust gases to spin a compressor that forces more air into the cylinders. There is no on/off switch — boost builds passively with RPM.

Nitrous oxide injection is the push-a-button-go-faster mechanism: N₂O is injected into the intake, releasing extra oxygen, allowing more fuel to be burned, producing a sudden sharp power spike from a finite tank.

Taito called it "turbo" because in 1988 "turbo" was the glamour word (turbo Ferraris, turbo Saabs). The confusion was endemic to that era of gaming and persists today.

## The speedometers are deliberately jittery

The two dashboard meters (`$860F: animate_meters`) do not display the true underlying meter level. Every frame, the game generates a signed random byte; if negative the meter decreases by 1, if positive it increases by 1 (clamped to 0–7). The actual speed state does not change — only the display flickers. The result is that the dials look alive and responsive without any extra complexity in the speed model. Both meters duplicate the same logic independently, so they jitter out of phase with each other.

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

## The game is spiritually a Miami Vice product

Chase H.Q. (Taito, 1988 arcade) fits squarely in the Miami Vice cultural moment: plainclothes cops, a white European sports car, pursuing criminals rather than racing competitively. The TV show ran 1984–1990 and the "undercover cop in an exotic car" aesthetic dominated that window. Taito never cited Miami Vice as a direct source, but the thematic overlap is hard to attribute to coincidence. Chase H.Q. may also be downstream of a broader trend — Miami Vice itself borrowed heavily from _To Live and Die in L.A._ (1985) and similar 80s material.
