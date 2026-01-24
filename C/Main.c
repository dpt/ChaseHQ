// Main.c
//
// Chase H.Q. code model
//
// by dpt

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include "ZXSpectrum/Spectrum.h"
#include "ZXSpectrum/Screen.h"

#include "ChaseHQ/ChaseHQ-State.h"
#include "ChaseHQ/ChaseHQ.h"

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

  state = calloc(sizeof(*state), 1);

  chasehq_reset_state(state);
  entrypt_128k(state);

  state->current_stage_number = -1; // force load
  state->wanted_stage_number = 0;
  load_stage(state);
  run_pregame_screen(state);

  while (!quit) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        quit = 1;
        break;

      //case SDL_KEYDOWN:
      case SDL_KEYUP: {
        int i;

        static const char msg[] = "PREGAME SCREEN RENDERING IS WORKING!";

        for (i = 0; i < sizeof(msg) - 1; i++) {
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
        run_pregame_screen_loop(state);
        break;

      case SDL_MOUSEBUTTONUP:
      case SDL_MOUSEBUTTONDOWN:
        t = event.button.state == SDL_PRESSED;
        if (t) {
          //state->sighted_flag = 0;
          setup_transition(state, 8);
        }
        break;

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
