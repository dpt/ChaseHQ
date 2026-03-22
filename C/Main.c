/**
 * Main.c
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
 * The original game and design is copyright (c) 1988 Taito Corporation
 * The ZX Spectrum version is copyright (c) 1989 Ocean Software Limited
 * The recreated version is copyright (c) 2023-2026 David Thomas
 */

#include <stdint.h>
#include <stdio.h>

#include "SDL.h"

#include "ZXSpectrum/Spectrum.h"
#include "ZXSpectrum/Keyboard.h"
#include "ZXSpectrum/Kempston.h"

#include "ChaseHQ/ChaseHQ.h"

// -----------------------------------------------------------------------------

// Configuration
//
#define FPS           15
#define GAMEWIDTH     256
#define GAMEHEIGHT    192
#define BORDER        32
#define GAMESCALE     4

#define SCALEDWIDTH   (GAMEWIDTH  * GAMESCALE)
#define SCALEDHEIGHT  (GAMEHEIGHT * GAMESCALE)
#define SCALEDBORDER  (BORDER * GAMESCALE)

#define WINDOWWIDTH   (SCALEDWIDTH  + (SCALEDBORDER * 2))
#define WINDOWHEIGHT  (SCALEDHEIGHT + (SCALEDBORDER * 2))

// -----------------------------------------------------------------------------

typedef struct
{
  zxspectrum_t *zx;
  chqstate_t   *game;

  zxkeyset_t    keys;
  zxkempston_t  kempston;

  int           paused; // bool

  int           quit; // bool

  SDL_Renderer *renderer;
  SDL_Texture  *texture;
  // int           menu; // bool

  int           sleep_us; // us to sleep for on the next loop
}
state_t;

/* ----------------------------------------------------------------------- */

static void draw_handler(const zxbox_t *dirty,
                         void          *opaque)
{
  state_t  *state = opaque;
  uint32_t *pixels;

  pixels = zxspectrum_claim_screen(state->zx);
  SDL_UpdateTexture(state->texture, NULL, pixels, GAMEWIDTH * 4);
  zxspectrum_release_screen(state->zx);
}

static void stamp_handler(void *opaque)
{
  state_t *state = opaque;

  // TODO: Save timestamps.
}

static int sleep_handler(int durationTStates, void *opaque)
{
  state_t *state = opaque;

  // TODO: Sleep.
  //
  //usleep(durationTStates * 1000000 / 3500000);
  //emscripten_sleep(durationTStates * 1000000 / 3500000);
  //
  // Note that we can't sleep here: in this single-threaded model it would
  // stall the UI.  Instead we could store the timing details here and apply
  // them in the main_loop. Though that would still create lumpy effects due
  // to the way the game does not currently yield to its caller during
  // periods when it wants to sleep.

  return 0;
}

static int key_handler(uint16_t port, void *opaque)
{
  state_t *state = opaque;

  if (port == port_KEMPSTON_JOYSTICK)
    return state->kempston;
  else
    return zxkeyset_for_port(port, &state->keys);
}

static void border_handler(int colour, void *opaque)
{
  state_t *state = opaque;

  // TODO: Set border colour.
}

static void speaker_handler(int on_off, void *opaque)
{
  state_t *state = opaque;

  // TODO: All sound.
}

/* ----------------------------------------------------------------------- */

static void sdl_key_pressed(state_t *state, const SDL_KeyboardEvent *k)
{
  SDL_Keycode  sym;
  int          down;
  zxjoystick_t j;

  sym = k->keysym.sym;
  switch (sym)
  {
    case SDLK_LEFT:  j = zxjoystick_LEFT;    break;
    case SDLK_RIGHT: j = zxjoystick_RIGHT;   break;
    case SDLK_UP:    j = zxjoystick_UP;      break;
    case SDLK_DOWN:  j = zxjoystick_DOWN;    break;
    case '.':        j = zxjoystick_FIRE;    break;
    default:         j = zxjoystick_UNKNOWN; break;
  }

  down = (k->type == SDL_KEYDOWN);

  if (j != zxjoystick_UNKNOWN)
  {
    zxkempston_assign(&state->kempston, j, down);
  }
  else
  {
    zxkeyset_t *keys;

    keys = &state->keys;
    if (down)
      zxkeyset_setchar(keys, k->keysym.sym);
    else
      zxkeyset_clearchar(keys, k->keysym.sym);
  }
}

// type: em_arg_callback_func
static void my_main_loop(void *opaque)
{
  static const SDL_Rect dstrect = { SCALEDBORDER, SCALEDBORDER, SCALEDWIDTH, SCALEDHEIGHT };

  state_t *state = opaque;

  {
    SDL_Event event;

    // Consume all pending events
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
        case SDL_QUIT:
          state->quit = 1;
          SDL_Log("Quitting after %i ticks", event.quit.timestamp);
          break;

        case SDL_WINDOWEVENT:
          // PrintEvent(&event);
          break;

        case SDL_KEYDOWN:
        case SDL_KEYUP:
          sdl_key_pressed(state, &event.key);
          break;

        case SDL_TEXTEDITING:
        case SDL_TEXTINPUT:
          break;

        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEWHEEL:
          break;

        default:
          printf("Unhandled event code {%d}\n", event.type);
          break;
      }
    }

    if (state->quit)
      return;

    // if (state->menu)
    // {
    //   if (chq_menu(state->game) > 0)
    //   {
    //     chq_setup2(state->game);
    //     state->menu = 0;
    //   }
    // }
    // else
    {
      chq_main(state->game);
    }

    /* Update the texture and render it */

    /* Clear screen */
    // TODO: This ought to be the border colour, but CHQ's is always black.
    SDL_SetRenderDrawColor(state->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(state->renderer);

    /* Offset the image */
    // Note that this will inhibit image stretching.

    SDL_RenderCopy(state->renderer, state->texture, NULL, &dstrect);
    SDL_RenderPresent(state->renderer);

    SDL_Delay(1000 / FPS);
  }
}

int main(void)
{
  state_t          state;
  const zxconfig_t zxconfig =
  {
    GAMEWIDTH / 8, GAMEHEIGHT / 8,
    &state, /* opaque */
    &draw_handler,
    &stamp_handler,
    &sleep_handler,
    &key_handler,
    &border_handler,
    &speaker_handler
  };
  SDL_Window      *window;

  printf("CHASE H.Q.\n");
  printf("==========\n");

  printf("Initialising...\n");

  zxkeyset_clear(&state.keys);
  state.kempston  = 0;
  state.paused    = 0;
  state.quit      = 0;
  // state.menu      = 1;

  state.zx = zxspectrum_create(&zxconfig);
  if (state.zx == NULL)
    goto failure;

  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    fprintf(stderr, "Error: SDL_Init: %s\n", SDL_GetError());
    goto failure;
  }

  window = SDL_CreateWindow("Chase H.Q.",
                            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            WINDOWWIDTH, WINDOWHEIGHT,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (window == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateWindow: %s\n", SDL_GetError());
    goto failure;
  }

  state.renderer = SDL_CreateRenderer(window,
                                      -1,
                                      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (state.renderer == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateRenderer: %s\n", SDL_GetError());
    goto failure;
  }

  // native_format = SDL_GetWindowPixelFormat(window);

  state.texture = SDL_CreateTexture(state.renderer,
                                    SDL_PIXELFORMAT_ARGB8888, // fastest?
                                    SDL_TEXTUREACCESS_STREAMING,
                                    GAMEWIDTH, GAMEHEIGHT);
  if (state.texture == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateTexture: %s\n", SDL_GetError());
    goto failure;
  }

  if (SDL_SetTextureBlendMode(state.texture, SDL_BLENDMODE_NONE) < 0)
  {
    fprintf(stderr, "Error: SDL_SetTextureBlendMode: %s\n", SDL_GetError());
    goto failure;
  }

  state.game = chq_create(state.zx);
  if (state.game == NULL)
    goto failure;

  chq_setup(state.game);

  while (!state.quit)
    my_main_loop(&state);

  chq_destroy(state.game);
  zxspectrum_destroy(state.zx);

  SDL_DestroyTexture(state.texture);
  SDL_DestroyRenderer(state.renderer);
  SDL_DestroyWindow(window);

  SDL_Quit();

  printf("(quit)\n");

  exit(EXIT_SUCCESS);


failure:

  exit(EXIT_FAILURE);
}
