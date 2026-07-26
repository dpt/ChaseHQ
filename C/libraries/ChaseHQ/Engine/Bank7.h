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

/**
 * $E000: Show the end screen
 *
 * Displays the end-of-game results screen: the entry loop and script
 * dispatch skeleton are ported; the draw/glyph-plot handler bodies and
 * script_data content are not yet (see Bank7.c).
 *
 */
void show_end_screen(chqstate_t *state);

/**
 * Allocates and initialises state->bank7.
 * \return 0 on success, -1 on allocation failure.
 */
int bank7_state_create(chqstate_t *state);

/**
 * Frees state->bank7 and clears the pointer.
 */
void bank7_state_destroy(chqstate_t *state);

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_BANK7_H */
