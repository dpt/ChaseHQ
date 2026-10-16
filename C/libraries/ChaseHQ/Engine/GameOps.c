/**
 * GameOps.c
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

#include <stddef.h>

#include "ChaseHQ/ChaseHQ.h"
#include "ChaseHQ/GameOps.h"

#include "ChaseHQ/Data/CommonData.h"

/* ----------------------------------------------------------------------- */

static void *ops_create(zxspectrum_t *zx)
{
  return chq_create(zx);
}

static void ops_destroy(void *game)
{
  chq_destroy(game);
}

static void ops_run(void *game, const zxgame_launch_t *launch)
{
  chq_start(game, launch->model == ZXGAME_MODEL_128K);
}

static void ops_stop(void *game)
{
  chq_stop(game);
}

static int ops_toggle_debug(void *game)
{
  return chq_toggle_test_mode(game);
}

/* Classifies an OSD character into a font[] glyph index, mirroring the
 * width-class ladder in font_glyph_for_char (Bank3.c) / ascii_to_glyph_id
 * (Bank7.c). Duplicated locally (rather than shared) because those are
 * static to their own translation units. Returns NULL for space.
 */
static const u8 *ops_osd_glyph(char c)
{
  u8 A_diff;  /* char - $20; classification input (was A) */
  u8 C_class; /* width-class index (was C) */

  if (c == ' ')
    return NULL;

  if (c >= 'a' && c <= 'z') /* font[] only has uppercase */
    c = (char) (c - 'a' + 'A');

  A_diff = (u8) (c - 0x20);

  if (A_diff >= 0x21)
    C_class = (u8) (A_diff - 18);
  else if (A_diff >= 0x10)
    C_class = (u8) (A_diff - 11);
  else if (A_diff == 1)
    C_class = 0;
  else if (A_diff == 8)
    C_class = 1;
  else if (A_diff == 9)
    C_class = 2;
  else if (A_diff == 12)
    C_class = 3;
  else
    C_class = 4;

  if (C_class >= 41) /* 41 glyphs in font[]; catches control chars wrapping */
    return NULL;

  return &font[C_class * 7];
}

/* Builds a linear SCREEN_WIDTH x SCREEN_HEIGHT one-bit view of the game's raw
 * $F000 backbuffer. Its address format is not the hardware screen's
 * interleave; per draw_object_clipped's comment (Main.c, DE_backbuf
 * assembly), backbuffer addresses pack as 0b1111LLLLRRRCCCCC -- scanline
 * low nibble (L) in bits 8-11, row-group (R) in bits 5-7, column byte (C) in
 * bits 0-4. So the native storage row for logical row y is
 * (y&0x0F)*8 + ((y>>4)&0x07), not a linear row index.
 *
 * Set bits are black. ponytail: debug view only, tears against the game
 * thread (no lock on chqstate_t::backbuffer).
 */
static const u8 *ops_debug_buffer(void *game)
{
  static u8 linear[SCREEN_BITMAP_LENGTH];
  const u8 *backbuf;
  int       bbwidth, bbheight, rowbytes, linear_y, col;

  backbuf  = chq_get_backbuffer(game, &bbwidth, &bbheight);
  rowbytes = bbwidth / 8;

  /* The buffer is shorter than the screen (128 rows vs 192) and covers the
   * playfield, which sits at the bottom of the screen -- so top-align its
   * row 0 to display row (SCREEN_HEIGHT - bbheight), not display row 0. */
  for (linear_y = 0; linear_y < SCREEN_HEIGHT; linear_y++)
  {
    int buf_y = linear_y - (SCREEN_HEIGHT - bbheight);
    int y     = (buf_y & 0x0F) * 8 + ((buf_y >> 4) & 0x07);

    for (col = 0; col < SCREEN_WIDTH; col++)
    {
      int on, mask;

      on   = buf_y >= 0 && buf_y < bbheight &&
             (backbuf[y * rowbytes + col / 8] & (0x80 >> (col % 8))) != 0;
      mask = 0x80 >> (col % 8);
      if (on)
        linear[linear_y * (SCREEN_WIDTH / 8) + col / 8] |= (u8) mask;
      else
        linear[linear_y * (SCREEN_WIDTH / 8) + col / 8] &= (u8) ~mask;
    }
  }

  return linear;
}

/* ----------------------------------------------------------------------- */

const zxgame_ops_t chq_game_ops =
{
  "Chase H.Q.",
  ZXGAME_MODEL_48K | ZXGAME_MODEL_128K,
  ZXGAME_MODEL_128K,
  ops_create,
  ops_destroy,
  ops_run,
  ops_stop,
  NULL, /* step */
  NULL, /* save */
  NULL, /* load */
  NULL, /* dispose_error */
  NULL, /* save_extension */
  NULL, /* scripted_key */
  ops_osd_glyph,
  ops_toggle_debug,
  ops_debug_buffer,
  NULL /* in_menu */
};
