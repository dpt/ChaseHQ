// main.c
//
// Chase H.Q. code model
//
// by dpt

// vim: ts=8 sts=2 sw=2 et

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"

#include "Types.h"
#include "Pixels.h"
#include "Spectrum.h"
#include "Z80.h"
#include "ChaseHQ-Data.h"
#include "ChaseHQ-State.h"
#include "ChaseHQ.h"

/**
 * Return the minimum of (a,b).
 */
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

/**
 * Return the maximum of (a,b).
 */
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

/**
 * Return 'a' clamped to the range [b..c].
 */
#define CLAMP(a,b,c) MIN(MAX(a,b),c)

typedef struct zxbox {
  int x0, y0, x1, y1;
}
zxbox_t;

/* Define to highlight dirty rectangles when they're drawn. */
//#define SHOW_DIRTY_RECTS

#define RGB

#ifdef RGB
/* 0x00RRGGBB */
#define BK_ 0x00000000
#define RD_ 0x00010000
#define GR_ 0x00000100
#define YL_ 0x00010100
#define BL_ 0x00000001
#define MG_ 0x00010001
#define CY_ 0x00000101
#define WH_ 0x00010101
#else
/* 0x00BBGGRR */
#define BK_ 0x00000000
#define RD_ 0x00000001
#define GR_ 0x00000100
#define YL_ 0x00000101
#define BL_ 0x00010000
#define MG_ 0x00010001
#define CY_ 0x00010100
#define WH_ 0x00010101
#endif

#define NORMAL(C) (C * 0xCD)
#define BRIGHT(C) (C * 0xFF)

/* BRIGHT 0 */
#define BKd NORMAL(BK_)
#define RDd NORMAL(RD_)
#define GRd NORMAL(GR_)
#define YLd NORMAL(YL_)
#define BLd NORMAL(BL_)
#define MGd NORMAL(MG_)
#define CYd NORMAL(CY_)
#define WHd NORMAL(WH_)
/* BRIGHT 1 */
#define BKb BRIGHT(BK_)
#define RDb BRIGHT(RD_)
#define GRb BRIGHT(GR_)
#define YLb BRIGHT(YL_)
#define BLb BRIGHT(BL_)
#define MGb BRIGHT(MG_)
#define CYb BRIGHT(CY_)
#define WHb BRIGHT(WH_)

/* Given a set of tokens: A, B, C, which have permutations:
 *   (AA, AB, AC, BA, BB, BC, CA, CB, CC)
 * They can be written out contiguously as:
 *   AAABACBABBBCCACBCC
 * We can remove and index the redundant overlaps:
 *   AABACBBCACC
 * (Note that 'AC' occurs twice in the sequence).
 * Producing a mapping:
 *   (0, 1, 3, 2, 5, 6, 7, 4, 9)
 * This is a De Bruijn sequence.
 *
 * The following palette is laid out in a De Bruijn sequence in a vain attempt
 * to improve performance.
 */
static const unsigned int palette[66 + 65] = {
  BKd, BKd, BLd, BKd, RDd, BKd, MGd, BKd,
  GRd, BKd, CYd, BKd, YLd, BKd, WHd, BLd,
  BLd, RDd, BLd, MGd, BLd, GRd, BLd, CYd,
  BLd, YLd, BLd, WHd, RDd, RDd, MGd, RDd,
  GRd, RDd, CYd, RDd, YLd, RDd, WHd, MGd,
  MGd, GRd, MGd, CYd, MGd, YLd, MGd, WHd,
  GRd, GRd, CYd, GRd, YLd, GRd, WHd, CYd,
  CYd, YLd, CYd, WHd, YLd, YLd, WHd, BKd,
  WHd, WHd,

  /* The zeroth bright entry is shared. */
  BKb, BLb, BKb, RDb, BKb, MGb, BKb,
  GRb, BKb, CYb, BKb, YLb, BKb, WHb, BLb,
  BLb, RDb, BLb, MGb, BLb, GRb, BLb, CYb,
  BLb, YLb, BLb, WHb, RDb, RDb, MGb, RDb,
  GRb, RDb, CYb, RDb, YLb, RDb, WHb, MGb,
  MGb, GRb, MGb, CYb, MGb, YLb, MGb, WHb,
  GRb, GRb, CYb, GRb, YLb, GRb, WHb, CYb,
  CYb, YLb, CYb, WHb, YLb, YLb, WHb, BKb,
  WHb, WHb,
};

static const unsigned char offsets[66 + 65] = {
  0,   1,   3,   5,   7,   9,  11,  13,
  2,  15,  16,  18,  20,  22,  24,  26,
  4,  17,  28,  29,  31,  33,  35,  37,
  6,  19,  30,  39,  40,  42,  44,  46,
  8,  21,  32,  41,  48,  49,  51,  53,
  10,  23,  34,  43,  50,  55,  56,  58,
  12,  25,  36,  45,  52,  57,  60,  61,
  62,  14,  27,  38,  47,  54,  59,  64,
  0,  66,  68,  70,  72,  74,  76,  78,
  67,  80,  81,  83,  85,  87,  89,  91,
  69,  82,  93,  94,  96,  98, 100, 102,
  71,  84,  95, 104, 105, 107, 109, 111,
  73,  86,  97, 106, 113, 114, 116, 118,
  75,  88,  99, 108, 115, 120, 121, 123,
  77,  90, 101, 110, 117, 122, 125, 126,
  127,  79,  92, 103, 112, 119, 124, 129,
};

#define WRITE8PIX(shift)                            \
do {                                                \
  pal = &palette[offsets[(attrs >> shift) & 0x7F]]; \
  *poutput++ = pal[(input >> (shift + 7)) & 1];     \
  *poutput++ = pal[(input >> (shift + 6)) & 1];     \
  *poutput++ = pal[(input >> (shift + 5)) & 1];     \
  *poutput++ = pal[(input >> (shift + 4)) & 1];     \
  *poutput++ = pal[(input >> (shift + 3)) & 1];     \
  *poutput++ = pal[(input >> (shift + 2)) & 1];     \
  *poutput++ = pal[(input >> (shift + 1)) & 1];     \
  *poutput++ = pal[(input >> (shift + 0)) & 1];     \
} while (0)

/* For reference:
 *
 * Spectrum screen memory has the arrangement:
 * 0b010BBLLLRRRCCCCC (B = band, L = line, R = row, C = column)
 *
 * Attribute bytes have the format:
 * 0bLRBBBFFF (L = flash, R = bright, B = paper (background), F = ink (foreground))
 */

void zxscreen_convert(const void    *vscr,
                      unsigned int  *poutput,
                      const zxbox_t *dirty)
{
  zxbox_t              box;
  int                  height;
  const unsigned int  *pattrs;
  int                  width;
  int                  x, linear_y;
  const unsigned int  *pinput;
  unsigned int         input;
  unsigned int         attrs;
  const unsigned int  *pal;

  assert(dirty);

#ifdef SHOW_DIRTY_RECTS
  static int dirtybits;
  dirtybits = 0x20202020 - dirtybits;
#endif

  /* Clamp the dirty rectangle to the screen dimensions. */
  box.x0 = CLAMP(dirty->x0, 0, 255);
  box.y0 = CLAMP(dirty->y0, 0, 191);
  box.x1 = CLAMP(dirty->x1, 1, 256);
  box.y1 = CLAMP(dirty->y1, 1, 192);

  /* The inner loop processes 32 pixels at a time, so we need to convert x
   * coordinates into chunks four attributes wide while rounding up and down as
   * required. */
  box.x0 = (box.x0) / 32;      /* divide to 0..7 rounding down */
  box.x1 = (box.x1 + 31) / 32; /* divide to 0..7 rounding up */

  /* Convert y coordinates into screen space - (0,0) is top left. */
  height = box.y1 - box.y0;
  box.y0 = 192 - box.y1;
  box.y1 = box.y0 + height;

  pattrs = (const unsigned int *) vscr
           + (SCREEN_BITMAP_LENGTH
              + box.y0 / 8 * 32  /* 8 scanlines/row, 32 attrs/row */
              + box.x0 * 4) / 4; /* 4 bytes/chunk, 4 bytes/word */

  poutput += box.y0 * 256 /* 256 pixels/row (256 words) for output */
             + box.x0 * 32; /* 32 pixels/chunk (32 words) */

  width = box.x1 - box.x0; /* hoisted out of loop */

  for (linear_y = box.y0; linear_y < box.y1; linear_y++) {
    /* Transpose fields using XOR */
    unsigned int tmp = (linear_y ^ (linear_y >> 3)) & 7;
    int          y   = linear_y ^ (tmp | (tmp << 3));

    pinput = (const unsigned int *) vscr
             + (y     * 32           /* 32 bytes/row */
                + box.x0 * 32 / 8) / 4; /* 32 bytes/row, 8 pixels/byte, 4 bytes/word */
    for (x = width; x > 0; x--) { /* x is unused in the loop body */
      input = *pinput++;
      attrs = *pattrs++;
#ifdef SHOW_DIRTY_RECTS
      attrs ^= dirtybits; /* force colour attrs to show redrawn areas */
#endif

      WRITE8PIX(0);
      WRITE8PIX(8);
      WRITE8PIX(16);
      WRITE8PIX(24);
    }

    /* Skip to the start of the next row. */
    pattrs  += 8   - width;
    poutput += 256 - width * 32;

    /* Rewind pattrs except at the end of an attribute row. */
    if ((linear_y & 7) != 7)
      pattrs -= 8;
  }
}

/* ----------------------------------------------------------------------- */

#define GAMEWIDTH  (256)
#define GAMEHEIGHT (192)
#define GAMESCALE  (4)

static const SDL_Rect dstrect = { 0, 0, GAMEWIDTH * GAMESCALE, GAMEHEIGHT * GAMESCALE };
unsigned int pixels[GAMEWIDTH * GAMEHEIGHT];

int main(void)
{
#if 0
  // testing the multiplier routine

  static const T a[5] = { 0xA0, 0xE7, 0x20, 0xC0, 0xE6 };
  static const T c[5] = { 0x05, 0x02, 0x05, 0x03, 0xFE };
  static const T r[5] = { 0x03, 0x02, 0x01, 0x02, 0xFE };

  for (int i = 0; i < 5; i++) {
    T n = multiply(a[i], c[i]);
    if (n == r[i])
      printf("ok: %d x %d = %d\n", a[i], c[i], r[i]);
    else
      printf("error: %d x %d = %d, got %d\n", a[i], c[i], r[i], n);
  }

  return 0;
#else
  SDL_Event     event;
  SDL_Window   *window;
  SDL_Renderer *renderer;
  SDL_Texture  *texture;
  chqstate_t   *state;
  int           quit = 0;
  int           mx = 0, my = 0;
  int           t = 0;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }

  window = SDL_CreateWindow("Chase H.Q.",
                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            GAMEWIDTH * GAMESCALE, GAMEHEIGHT * GAMESCALE,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (window == NULL) {
    printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  renderer = SDL_CreateRenderer(window,
                                -1,
                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL) {
    printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  texture = SDL_CreateTexture(renderer,
                              SDL_PIXELFORMAT_ARGB8888, // fastest?
                              SDL_TEXTUREACCESS_STREAMING,
                              GAMEWIDTH, GAMEHEIGHT);
  if (texture == NULL) {
    printf("SDL_CreateTexture failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  if (SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE) < 0) {
    printf("SDL_SetTextureBlendMode failed: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  posix_memalign((void **) &state, 4096, sizeof(*state));

  uintptr_t p = (1 << 12) - ((uintptr_t)&state->backbuffer[0] & ((1 << 12) -1));
  printf("state=%p backbuffer=%p p=%ld\n", state, &state->backbuffer[0], p);

  chasehq_reset_state(state);
  entrypt_128k(state);

  state->current_stage_number = -1; // force load
  state->wanted_stage_number = 0;
  load_stage(state);

  while (!quit) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        quit = 1;
        break;

      //case SDL_KEYDOWN:
      case SDL_KEYUP: {
        static const char msg[] = "WOOT, NOW WE HAVE A SMASH BAR";

        for (int i = 0; i < sizeof(msg) - 1; i++) {
          if (t)
            plot_mini_font_cursor_off(state, i, msg[i]);
          else
            plot_mini_font_cursor_on(state, i, msg[i]);
        }
      }
      break;

      case SDL_MOUSEMOTION:
        mx = event.motion.x;
        my = event.motion.y;
        //printf("my=%d\n",my);
        // ledfont_plot(state, 1 + my % 10,
        //              &state->screen[(0x4000 + mx / 8) - SCREEN_START_ADDRESS]);
        clear_playfield_set_attrs(state);
        memcpy(&state->backbuffer[0], backbufexample, sizeof(backbufexample));
        draw_mugshots(state);
        state->sighted_flag = 1;
        state->perp_caught_phase = PERPCAUGHTPHASE_2;
        state->smash_counter = my / 16;
        draw_smash_bar(state);
        transition(state);
        draw_screen(state);
        break;

      case SDL_MOUSEBUTTONUP:
      case SDL_MOUSEBUTTONDOWN:
        t = event.button.state == SDL_PRESSED;
        if (t) {
          //state->sighted_flag = 0;
          setup_transition(state, 8);
        }
        break;

        // main_loop(state);

        //plot_face(state, 0x4036, &bitmap_faces[FACEBYTES*0]);
        //plot_face(state, 0x4836, &bitmap_faces[FACEBYTES*1]);
        //plot_face(state, 0x5036, &bitmap_faces[FACEBYTES*2]);

#if 0
        // Handle mouse motion event
        for (int i = 0; i < 22; i++) // we read 20/21 entries
          state->road_buffer[i] = 0x70 + (event.motion.x * 0x10 / 256);
        build_curve_table(state, /*forked=*/0);

        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));

        for (int y = 0; y < 104; y++) {
          int yp = y; // 0 is top

          int r = state->table_ec00[0x30 + y] >> 0;
          int l = state->table_e800[0x30 + y] >> 0;
          SDL_Rect rrect = {255 - r, yp, r, 1};
          SDL_Rect lrect = {      0, yp, l, 1};
          SDL_FillRect(surface, &rrect, SDL_MapRGB(surface->format, 0x00, 0x00, 0xFF));
          SDL_FillRect(surface, &lrect, SDL_MapRGB(surface->format, 0xFF, 0x00, 0x00));

          SDL_Rect rect = {0, yp, 1, 1};
          SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0x00));
        }
        SDL_UpdateWindowSurface(window);
        state->fast_counter++;
#endif
        break;
      }
    }

    if (quit)
      break;

    {
      zxbox_t dirty = {0, 0, GAMEWIDTH, GAMEHEIGHT};

      zxscreen_convert(&state->screen[0], pixels, &dirty);
      SDL_UpdateTexture(texture, NULL, pixels, GAMEWIDTH * 4);
    }

    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_RenderPresent(renderer);

    SDL_Delay(1000 / 60); //fps
  }

  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();
#endif
}
