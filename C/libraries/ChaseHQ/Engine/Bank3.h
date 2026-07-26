/**
 * Bank3.h
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

#ifndef CHASEHQ_BANK3_H
#define CHASEHQ_BANK3_H

#include "C99/Types.h"

#include "ChaseHQ/ChaseHQ.h"

/* ----------------------------------------------------------------------- */

/**
 * Allocates and initialises state->bank3.
 * \return 0 on success, -1 on allocation failure.
 */
int bank3_state_create(chqstate_t *state);

/**
 * Frees state->bank3 and clears the pointer.
 */
void bank3_state_destroy(chqstate_t *state);

/* ----------------------------------------------------------------------- */

#define BANK3_TITLE_SCREEN    (0xC000)
#define BANK3_HI_SCORE        (0xC003)
#define BANK3_SUCCESS_MUSIC   (0xC006)
#define BANK3_INPUT_SELECTION (0xC009)

u8 bank3_call(chqstate_t *state, int routine);

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_BANK3_H */
