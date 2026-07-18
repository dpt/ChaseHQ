/**
 * SoundSamples.h
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

#ifndef CHASEHQ_SOUNDSAMPLES_H
#define CHASEHQ_SOUNDSAMPLES_H

#include "C99/Types.h"

/* Speech sample indices (1-indexed, matching the Z80 original at $F32E).
 * Each sample occupies a contiguous slice of the 16 KiB sound_samples[] array
 * packed consecutively from offset 0. Callers pass these values directly;
 * play_speech_128k subtracts 1 when indexing the internal table. */
#define SOUND_SAMPLES_Z80_BASE (0xC000) /* bank 4 base address where samples are mapped */

#define SAMPLE_GIDDY_UP (1) /* "Giddy up boy!"        - $C000..$CA8B - 2700 bytes */
#define SAMPLE_LETS_GO  (2) /* "Let's go Mr. Driver!" - $CA8C..$DF71 - 5350 bytes */
#define SAMPLE_HOLD_ON  (3) /* "Hold on man!"         - $DF72..$E9CB - 2650 bytes */
#define SAMPLE_TIME_UP  (4) /* "Your time's up"       - $E9CC..$F489 - 2750 bytes */
#define SAMPLE_START    (5) /* Start noise            - $F48A..$FF65 - 2780 bytes */
#define SAMPLE__LIMIT   (6) /* one past last valid index */

const u8 sound_samples[16384];

#endif /* CHASEHQ_SOUNDSAMPLES_H */

