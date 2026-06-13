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

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

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
#define GAMESCALE     2

#define SCALEDWIDTH   (GAMEWIDTH  * GAMESCALE)
#define SCALEDHEIGHT  (GAMEHEIGHT * GAMESCALE)
#define SCALEDBORDER  (BORDER * GAMESCALE)

#define WINDOWWIDTH   (SCALEDWIDTH  + (SCALEDBORDER * 2))
#define WINDOWHEIGHT  (SCALEDHEIGHT + (SCALEDBORDER * 2))

#define MAXSTAMPS       (4)     // max depth of timestamps stack

// -----------------------------------------------------------------------------

typedef struct
{
  zxspectrum_t *zx;
  chqstate_t   *game;

  zxkeyset_t    keys;
  zxkempston_t  kempston;

  int           quit; // bool
  int           paused; // bool
  // int           menu; // bool

  struct timeval stamps[MAXSTAMPS];
  int            nstamps;

  SDL_Renderer *renderer;
  SDL_Texture  *texture;
  SDL_Thread   *game_thread;

  int           sleep_us; // us to sleep for on the next loop
}
chq_sdl_state_t;

/* ----------------------------------------------------------------------- */

static void chq_draw_handler(const zxbox_t *dirty,
                             void          *opaque)
{
  // SDL_UpdateTexture must be called from the main thread (Metal requirement).
  // The main loop picks up changes via zxspectrum_claim_screen.
  (void) dirty;
  (void) opaque;
}

static void chq_stamp_handler(void *opaque)
{
  chq_sdl_state_t *state = opaque;

  // Stack timestamps as they arrive
  assert(state->nstamps < MAXSTAMPS);
  if (state->nstamps >= MAXSTAMPS)
    return;
  gettimeofday(&state->stamps[state->nstamps++], NULL);
}

static int chq_sleep_handler(int durationTStates, void *opaque)
{
  chq_sdl_state_t *state = opaque;
  int              paused;

  // Unstack timestamps (even if we're paused)
  assert(state->nstamps > 0);
  if (state->nstamps <= 0)
    return state->quit;
  --state->nstamps;

  // Quit straight away if signalled
  if (state->quit)
    return 1;

  paused = state->paused;
  if (paused)
  {
    // If paused, sit in this loop, checking twice per second for unpausing
    for (;;)
    {
      paused = state->paused;
      if (!paused)
        break;

      usleep(500000); // 0.5s
    }
  }
  else
  {
    // A Spectrum 48K has 69,888 T-states per frame and its Z80 runs at
    // 3.5MHz (~50Hz) for a total of 3,500,000 T-states per second.
    const double          tstatesPerSec = 3.5e6;

    struct timeval        now;
    double                duration; // seconds
    const struct timeval *then;
    struct timeval        delta;
    double                consumed; // seconds

    gettimeofday(&now, NULL); // get time now before anything else

    {
      // 'duration' tells us how long the operation should take since the previous mark call.
      // Turn T-state duration into seconds
      duration = durationTStates / tstatesPerSec;
      // Adjust the game speed
      //duration = duration * 100 / state->speed;

      then = &state->stamps[state->nstamps];
    }

    delta.tv_sec  = now.tv_sec  - then->tv_sec;
    delta.tv_usec = now.tv_usec - then->tv_usec;

    consumed = delta.tv_sec + delta.tv_usec / 1e6;
    if (consumed < duration)
    {
      double     delay; // seconds
      useconds_t udelay;

      // We didn't take enough time - sleep for the remainder of our duration
      delay = duration - consumed;
      udelay = delay * 1e6;
      usleep(udelay);
    }
  }

  return 0;
}

static int chq_key_handler(uint16_t port, void *opaque)
{
  chq_sdl_state_t *state = opaque;

  if (port == port_KEMPSTON_JOYSTICK)
    return state->kempston;
  else
    return zxkeyset_for_port(port, &state->keys);
}

static void chq_border_handler(int colour, void *opaque)
{
  chq_sdl_state_t *state = opaque;

  // TODO: Set border colour.
}

static void chq_speaker_handler(int on_off, void *opaque)
{
  chq_sdl_state_t *state = opaque;

  // TODO: All sound.
}

static void chq_ay_out_handler(uint16_t port, uint8_t byte, void *opaque)
{
  chq_sdl_state_t *state = opaque;

  // TODO: AY-3-8912 audio.
}

static int chq_game_thread(void *opaque)
{
  chq_sdl_state_t *state = opaque;

  chq_setup(state->game);
  chq_main(state->game);
  state->quit = 1;
  return 0;
}

/* ----------------------------------------------------------------------- */

static void chq_sdl_key_pressed(chq_sdl_state_t         *state,
                                const SDL_KeyboardEvent *k)
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
static void chq_sdl_main_loop(void *opaque)
{
  static const SDL_Rect dstrect = { SCALEDBORDER, SCALEDBORDER, SCALEDWIDTH, SCALEDHEIGHT };

  chq_sdl_state_t *state = opaque;

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
          chq_sdl_key_pressed(state, &event.key);
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

    // This was the TGE loop setup:
    //
    // if (state->menu)
    // {
    //   if (run_menu(state->game) > 0)
    //   {
    //     ruin_setup2(state->game);
    //     state->menu = 0;
    //   }
    // }
    // else
    // {
    //   run_main(state->game);
    // }

    /* Update the texture from the game's converted screen buffer. */
    {
      uint32_t *pixels;

      pixels = zxspectrum_claim_screen(state->zx);
      SDL_UpdateTexture(state->texture, NULL, pixels, GAMEWIDTH * 4);
      zxspectrum_release_screen(state->zx);
    }

    /* Clear screen */
    // TODO: This ought to be the border colour, but CHQ's is always black.
    SDL_SetRenderDrawColor(state->renderer, 0x1F, 0x1F, 0x1F, 0xFF);
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
  chq_sdl_state_t  state;
  zxconfig_t       zxconfig;
  SDL_Window      *window;
  SDL_RendererInfo rinfo;
  Uint32           native_fmt;
  Uint32           Rmask, Gmask, Bmask, Amask;
  int              bpp;

  printf("CHASE H.Q.\n");
  printf("==========\n");

  printf("Initialising...\n");

  zxkeyset_clear(&state.keys);
  state.kempston  = 0;
  state.paused    = 0;
  state.quit      = 0;
  // state.menu      = 1;

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

  SDL_GetRendererInfo(state.renderer, &rinfo);
  native_fmt = rinfo.texture_formats[0];
  SDL_PixelFormatEnumToMasks(native_fmt, &bpp, &Rmask, &Gmask, &Bmask, &Amask);

  zxconfig.width    = GAMEWIDTH / 8;
  zxconfig.height   = GAMEHEIGHT / 8;
  zxconfig.opaque   = &state;
  zxconfig.draw     = &chq_draw_handler;
  zxconfig.stamp    = &chq_stamp_handler;
  zxconfig.sleep    = &chq_sleep_handler;
  zxconfig.key      = &chq_key_handler;
  zxconfig.border   = &chq_border_handler;
  zxconfig.speaker  = &chq_speaker_handler;
  zxconfig.ay_out   = &chq_ay_out_handler;
  zxconfig.bgr_pixels = (Rmask < Bmask); /* R in lower byte = BGR format */

  state.zx = zxspectrum_create(&zxconfig);
  if (state.zx == NULL)
    goto failure;

  state.texture = SDL_CreateTexture(state.renderer,
                                    native_fmt,
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

  state.game_thread = SDL_CreateThread(chq_game_thread, "game", &state);
  if (state.game_thread == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateThread: %s\n", SDL_GetError());
    goto failure;
  }

  while (!state.quit)
    chq_sdl_main_loop(&state);

  chq_stop(state.game);
  SDL_WaitThread(state.game_thread, NULL);

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
