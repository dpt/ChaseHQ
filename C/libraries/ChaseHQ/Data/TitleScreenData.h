/**
 * TitleScreenData.h
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

#ifndef TITLESCREENDATA_H
#define TITLESCREENDATA_H

#include "C99/Types.h"

#define TITLE_SCENE_COUNT          (5)
#define TITLE_SCENE_DATA_BASE (0xCCB7)
#define TITLE_GLYPH_COUNT        (115)

typedef struct title_glyph {
  u8        height_pairs; /* number of scanline-pairs to blit */
  u8        width_bytes;  /* glyph width in screen bytes (1-7) */
  const u8 *bitmap;      /* source pixel bytes, OR-blitted verbatim */
} title_glyph_t;

extern const u8 title_scene_data[1467];
extern const u16 title_scene_table_offset[TITLE_SCENE_COUNT];
extern const u8 title_speed_curve[36];
extern const u8 title_glyph_bitmaps[5948];
extern const title_glyph_t title_glyph_table[TITLE_GLYPH_COUNT];

#endif /* TITLESCREENDATA_H */
