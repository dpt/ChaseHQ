/**
 * Bank7.c
 *
 * This file is part of "Chase H.Q. in C".
 *
 * This project recreates the ZX Spectrum version of the chase-and-smash game
 * "Chase H.Q." in portable C code. It is free software provided without
 * warranty in the interests of education and software preservation.
 *
 * The arcade original was created by Taito Corporation in 1988. It was then
 * ported to the ZX Spectrum by Ocean Software Limited and released in 1989.
 *
 * The original game and design is copyright (c) 1988 Taito Corporation.
 * The ZX Spectrum version is copyright (c) 1989 Ocean Software Limited.
 * The recreated version is copyright (c) 2023-2026 David Thomas.
 */

/* Notes
 *
 * This file will hold the C translation of ZX Spectrum 128K "bank 7" -- the
 * end-of-game results/credits sequence. Paged into $C000-$FFFF and reached
 * via an inner relocation from $F7EF to $F300, driven by a script
 * interpreter at $E20A. Not yet ported; see show_end_screen() in Main.c.
 */

#include "C99/Types.h"

#include "ZXSpectrum/Macros.h"

#include "ChaseHQ/ChaseHQ.h"

#include "Bank7.h"

/* ----------------------------------------------------------------------- */

u8 call_bank_7_128k(chqstate_t *state, int HLroutine)
{
  NOT_USED(state);
  NOT_USED(HLroutine);

  return 0;
}

void show_end_screen(chqstate_t *state)
{
  NOT_USED(state);
}
