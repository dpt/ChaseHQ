/**
* Bank7.h
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

#ifndef CHASEHQ_BANK7_H
#define CHASEHQ_BANK7_H

#include "C99/Types.h"

#include "ChaseHQ/ChaseHQ.h"

/* ----------------------------------------------------------------------- */

/* Stub: bank 7 holds the end-of-game results/credits sequence, relocated at
 * $F7EF->$F300 with its own $E20A script interpreter. Not yet ported. */
u8 call_bank_7_128k(chqstate_t *state, int HLroutine);

/**
 * $5C00: Show the end screen
 *
 * Displays the end-of-game results screen.
 *
 * Conv: Not yet implemented; the Z80 version drives a full results/credits
 * sequence. This stub returns immediately.
 *
 * \param[in] state Pointer to game state.
 */
void show_end_screen(chqstate_t *state);

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_BANK7_H */
