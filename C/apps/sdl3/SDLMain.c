/**
 * SDLMain.c
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

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include <SDL3/SDL.h>

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

#include "ZXSpectrum/Macros.h"
#include "ZXSpectrum/Spectrum.h"
#include "ZXSpectrum/Keyboard.h"
#include "ZXSpectrum/Kempston.h"
#include "ZXSpectrum/slopay-chip.h"

#include "ChaseHQ/ChaseHQ.h"

#include "CRTShader.h"

// -----------------------------------------------------------------------------

// Configuration
//
#define GAMEWIDTH          (256)
#define GAMEHEIGHT         (192)
#define BORDER              (16)

#define SCALE_DEFAULT        (4)
#define SCALE_MIN            (1)
#define SCALE_MAX            (4)

#define SPEED_DEFAULT      (100) // percent
#define SPEED_MIN           (25)
#define SPEED_MAX         (1000)
#define SPEED_STEP          (25)

#define VOLUME_DEFAULT     (100) // percent
#define VOLUME_MIN           (0)
#define VOLUME_MAX         (100)
#define VOLUME_STEP         (10)

// Whether the CRT post-effect starts switched on. Both display backends are
// always built: the plain SDL_Renderer blit (any GPU backend,
// nearest-neighbour scaling) and the SDL3 GPU/Metal CRT post-effect pipeline
// (Metal only). F4 switches between them at runtime, so this only chooses
// which one comes up first. Override with -DCHQ_CRT_SHADER=1.
//
// They cannot both be live at once -- SDL_CreateRenderer and
// SDL_ClaimWindowForGPUDevice each want the window's swapchain -- so
// switching tears one down and builds the other. If the CRT backend will not
// start (no Metal), chq_set_crt_enabled falls back to the plain renderer.
#ifndef CHQ_CRT_SHADER
#define CHQ_CRT_SHADER       (0)
#endif

#define MAXSTAMPS            (4) // max depth of timestamps stack
#define MAXDIRTYRECTS        (8) // max dirty rects captured per frame before we coalesce to full-screen

#define AY_CLOCK_FREQ  (1773400) // ZX Spectrum 128K AY-3-8912 clock rate
#define AY_SAMPLE_RATE   (44100)
#define AY_VOLUME_PCT       (20) // 0..AY_MASTER_VOLUME_MAX

#define BEEPER_VOLUME_PCT   (20) // 48K beeper level, percent of full scale
#define BEEPER_AMPLITUDE (32767 * BEEPER_VOLUME_PCT / 100)

// Sampled speech drives the AY DAC by writing a new volume-register value
// every ~120us (~8.4KHz) from the game thread. The audio thread only pulls
// PCM from slopay_chip_get_sample() in bursts whenever SDL wants more data,
// so a naive "read the current register value" approach loses every write
// that happened between bursts. Instead, register writes are queued here
// with a timestamp and replayed one sample at a time against a matching
// virtual audio clock (see chq_apply_due_audio_events), so the reconstructed
// waveform reflects the write history rather than a single stale snapshot.
// The same queue carries 48K beeper level changes, timestamped from the
// game's virtual T-state clock (see chq_speaker_handler).
#define AY_QUEUE_CAPACITY (16384) // ~1.9s of nibble writes at ~8.4KHz; ample headroom

// Replay cursor anchor cushion; see the replay cursor comment in
// chq_sdl_state_t.
#define AY_REPLAY_CUSHION_NS (30000000ULL)

// -----------------------------------------------------------------------------

static int chq_window_width(int scale)
{
  return (GAMEWIDTH + BORDER * 2) * scale;
}

static int chq_window_height(int scale)
{
  return (GAMEHEIGHT + BORDER * 2) * scale;
}

// -----------------------------------------------------------------------------

typedef enum
{
  CHQ_AUDIO_EVENT_AY,     // AY register write (128K music/speech)
  CHQ_AUDIO_EVENT_SPEAKER // beeper EAR level change (48K bit-banged sfx)
}
chq_audio_event_type_t;

typedef struct
{
  Uint64                 time_ns; // when this write happened, relative to SDL_GetTicksNS()
  chq_audio_event_type_t type;
  slopay_chip_reg_t      reg;     // AY events only
  Uint8                  value;   // AY: register value; speaker: level 0/1
}
chq_audio_event_t;

typedef struct
{
  zxspectrum_t      *zx;
  chqstate_t        *game;

  zxkeyset_t         keys;
  zxkempston_t       kempston;

  int                quit;      // bool
  int                paused;    // bool
  int                mode_128k; // bool; 0 selects the 48K entry point

  int                scale;      // window/render scale, SCALE_MIN..SCALE_MAX
  int                fullscreen; // bool; toggled with F11
  int                speed;      // game speed, percent, SPEED_MIN..SPEED_MAX
  int                volume;     // output volume, percent, VOLUME_MIN..VOLUME_MAX

  struct timeval     stamps[MAXSTAMPS];
  int                nstamps;

  slopay_chip_t     *ay;
  slopay_chip_reg_t  ay_latched_reg; // register selected by last port_AY_REGISTER write

  // Timestamped audio event queue (AY register writes and beeper level
  // changes): game thread produces, audio thread consumes. audio_queue_mutex
  // guards head/tail and the slots between them.
  SDL_Mutex         *audio_queue_mutex;
  chq_audio_event_t  audio_queue[AY_QUEUE_CAPACITY];
  int                audio_queue_head;
  int                audio_queue_tail;

  // Speaker (beeper) state. The anchor maps the facade's virtual T-state
  // clock onto wall-clock ns: within a burst of toggles wall time barely
  // advances, so event times extrapolate from the anchor at the T-state
  // rate; when the T-state clock falls behind the wall clock a new burst
  // has begun and the anchor resets. Game thread only.
  Uint64             speaker_anchor_ns;
  Uint64             speaker_anchor_tstates;
  int                speaker_last_level; // last queued level (game thread)
  int                speaker_level;      // current replay level (audio thread)

  // Replay cursor: maps generated samples onto event timestamps. The wall
  // clock and the audio device's sample clock drift apart (measured ~1.4ms/s
  // here), so event times are never compared against a wall-clock-anchored
  // sample time. Instead, whenever the queue runs dry the cursor re-anchors
  // to the next event's timestamp minus AY_REPLAY_CUSHION_NS, then advances
  // by exactly one sample period per generated sample. Spacing within a
  // burst is preserved regardless of drift and every gap in the sound
  // re-syncs the two clocks. The cushion (extra output latency) absorbs
  // consumer-fast drift so a continuous stream (a speech sample) does not
  // starve mid-burst: at the measured drift, 30ms lasts ~21s of continuous
  // writes and speech samples are only ~1-2s long.
  Uint64             samples_played;
  Uint64             replay_anchor_ns;     // event time at the last anchor
  Uint64             replay_anchor_sample; // samples_played at the last anchor
  int                replay_anchored;      // bool; cleared when queue runs dry
  int                audio_muted;          // bool; mute sound if true

  // Dirty-rect overlay: the game (thread) reports each screen region it
  // refreshes via chq_draw_handler; we stash them here and outline them over
  // the rendered frame so refreshed regions are visible on screen. Cleared
  // once drawn. dirty_mutex guards all fields in this group.
  SDL_Mutex         *dirty_mutex;
  zxbox_t            dirty_rects[MAXDIRTYRECTS];
  int                dirty_count;
  int                dirty_full_screen; // bool; a NULL dirty box was reported (whole screen)
  int                show_dirty_overlay; // bool; toggled with F3, off by default

  SDL_Window        *window;

  // Display backend. Exactly one of these is live at a time, selected by
  // crt_enabled and swapped by chq_set_crt_enabled: crt when enabled,
  // renderer/texture when not. The other's handles are NULL.
  int                crt_enabled; // bool; toggled with F4
  chq_CRT_shader_t   crt;
  chq_CRT_params_t   crt_params;
  int                crt_param_index; // which crt_params field +/- adjusts
  SDL_Renderer      *renderer;
  SDL_Texture       *texture;

  SDL_Thread        *game_thread;
  SDL_AudioStream   *audio_stream;
}
chq_sdl_state_t;

/* ----------------------------------------------------------------------- */

// The title bar doubles as the status display for the F-key controls: the
// game itself has nowhere to show them.
static void chq_update_window_title(const chq_sdl_state_t *state)
{
  char title[64];

  SDL_snprintf(title, sizeof(title),
               "Chase H.Q. - Speed: %d%% - Volume: %d%%%s%s",
               state->speed,
               state->volume,
               state->audio_muted ? " (Muted)" : "",
               state->paused ? " - Paused" : "");
  SDL_SetWindowTitle(state->window, title);
}

static void chq_draw_handler(const zxbox_t *dirty,
                             void          *opaque)
{
  chq_sdl_state_t *state = opaque;

  // SDL_UpdateTexture must be called from the main thread (Metal requirement).
  // The main loop picks up changes via zxspectrum_claim_screen. Here we only
  // stash the dirty region (game thread) for the main loop to outline once it
  // renders the frame.
  SDL_LockMutex(state->dirty_mutex);
  if (dirty == NULL)
    state->dirty_full_screen = 1;
  else if (state->dirty_count < MAXDIRTYRECTS)
    state->dirty_rects[state->dirty_count++] = *dirty;
  SDL_UnlockMutex(state->dirty_mutex);
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
      // Adjust the game speed: higher speed -> shorter sleep
      duration = duration * 100 / state->speed;

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
  NOT_USED(colour);
  NOT_USED(opaque);
}

static void chq_audio_queue_push(chq_sdl_state_t       *state,
                                 Uint64                 time_ns,
                                 chq_audio_event_type_t type,
                                 slopay_chip_reg_t      reg,
                                 Uint8                  value)
{
  SDL_LockMutex(state->audio_queue_mutex);

  {
    int next_tail = (state->audio_queue_tail + 1) % AY_QUEUE_CAPACITY;

    if (next_tail != state->audio_queue_head) // drop event if the queue is full
    {
      chq_audio_event_t *ev = &state->audio_queue[state->audio_queue_tail];

      ev->time_ns = time_ns;
      ev->type    = type;
      ev->reg     = reg;
      ev->value   = value;

      state->audio_queue_tail = next_tail;
    }
  }

  SDL_UnlockMutex(state->audio_queue_mutex);
}

static void chq_speaker_handler(int on_off, uint64_t tstates, void *opaque)
{
  chq_sdl_state_t *state = opaque;
  Uint64           now_ns;
  Uint64           event_ns;

  if (on_off == state->speaker_last_level)
    return; // level unchanged: no edge to reproduce

  state->speaker_last_level = on_off;

  // Map the game's virtual T-state clock onto wall time (see the speaker
  // state comment in chq_sdl_state_t). 1 T-state = 1e9/3.5e6 = 2000/7 ns.
  now_ns   = SDL_GetTicksNS();
  event_ns = state->speaker_anchor_ns +
             (tstates - state->speaker_anchor_tstates) * 2000 / 7;
  if (event_ns < now_ns) // T-state clock fell behind wall clock: new burst
  {
    state->speaker_anchor_ns      = now_ns;
    state->speaker_anchor_tstates = tstates;
    event_ns = now_ns;
  }

  chq_audio_queue_push(state, event_ns, CHQ_AUDIO_EVENT_SPEAKER, 0, on_off);
}

static void chq_ay_out_handler(uint16_t port, uint8_t byte, void *opaque)
{
  chq_sdl_state_t *state = opaque;

  if (port == port_AY_REGISTER)
  {
    // Register select: only ever touched from the game thread, immediately
    // followed by the paired data write below, so no queuing needed here.
    state->ay_latched_reg = byte;
    return;
  }

  // port_AY_DATA: queue the write with a timestamp instead of applying it
  // immediately, so chq_audio_callback can replay it at the right sample
  // position (see the AY_QUEUE_CAPACITY comment above).
  chq_audio_queue_push(state,
                       SDL_GetTicksNS(),
                       CHQ_AUDIO_EVENT_AY,
                       state->ay_latched_reg,
                       byte);
}

// Applies any queued audio events (AY register writes and beeper level
// changes) due within the current output sample's period, and returns the
// beeper contribution for that sample. Called once per generated sample
// from chq_audio_callback so rapid writes (e.g. sampled speech) land on the
// correct output sample rather than all being collapsed into whichever
// value was current when the audio callback happened to run. See the replay
// cursor comment in chq_sdl_state_t for how clock drift is handled.
//
// Beeper level changes can arrive faster than the sample rate (the 48K
// engine tone toggles every ~60 T-states, under one 44.1kHz sample), so
// sampling the instantaneous level would alias or silence them entirely.
// Instead the level is integrated over the sample period (a box filter):
// the returned value is BEEPER_AMPLITUDE scaled by the fraction of the
// period the speaker spent high.
static int chq_apply_due_audio_events(chq_sdl_state_t *state)
{
  Uint64 start_ns; // sample period start on the replay cursor timeline
  Uint64 end_ns;   // sample period end
  Uint64 level_ns; // start of the current beeper level within the period
  Uint64 high_ns;  // time spent high within the period
  int    beeper;

  SDL_LockMutex(state->audio_queue_mutex);

  if (state->audio_queue_head == state->audio_queue_tail)
  {
    state->replay_anchored = 0;

    // No pending edges: the speaker holds its level for the whole period.
    beeper = state->speaker_level ? BEEPER_AMPLITUDE : 0;
  }
  else
  {
    if (!state->replay_anchored)
    {
      Uint64 head_time_ns = state->audio_queue[state->audio_queue_head].time_ns;

      // SDL_GetTicksNS() starts near zero, so guard the cushion subtraction
      // against underflow for writes made just after launch.
      state->replay_anchor_ns =
        (head_time_ns > AY_REPLAY_CUSHION_NS) ?
          head_time_ns - AY_REPLAY_CUSHION_NS : 0;
      state->replay_anchor_sample = state->samples_played;
      state->replay_anchored = 1;
    }

    // Computed fresh from the anchor rather than accumulated, so truncation
    // never drifts the cursor away from the sample count.
    start_ns = state->replay_anchor_ns +
               ((state->samples_played - state->replay_anchor_sample) *
                1000000000ULL) / AY_SAMPLE_RATE;
    end_ns   = state->replay_anchor_ns +
               ((state->samples_played + 1 - state->replay_anchor_sample) *
                1000000000ULL) / AY_SAMPLE_RATE;

    level_ns = start_ns;
    high_ns  = 0;

    while (state->audio_queue_head != state->audio_queue_tail)
    {
      chq_audio_event_t *ev = &state->audio_queue[state->audio_queue_head];

      if (ev->time_ns > end_ns)
        break;

      if (ev->type == CHQ_AUDIO_EVENT_AY)
      {
        slopay_chip_write_register(state->ay, ev->reg, ev->value);
      }
      else
      {
        Uint64 edge_ns = ev->time_ns;

        if (edge_ns < start_ns) // overdue edge: takes effect at period start
          edge_ns = start_ns;
        if (state->speaker_level)
          high_ns += edge_ns - level_ns;
        level_ns = edge_ns;
        state->speaker_level = ev->value;
      }

      state->audio_queue_head = (state->audio_queue_head + 1) %
                                AY_QUEUE_CAPACITY;
    }

    if (state->speaker_level)
      high_ns += end_ns - level_ns;

    beeper = (int) ((Uint64) BEEPER_AMPLITUDE * high_ns /
                    (end_ns - start_ns));
  }

  SDL_UnlockMutex(state->audio_queue_mutex);

  return beeper;
}

// Runs on SDL's audio thread. Called whenever SDL wants more data queued.
static void chq_audio_callback(void            *opaque,
                               SDL_AudioStream *stream,
                               int              additional_amount,
                               int              total_amount)
{
  chq_sdl_state_t     *state = opaque;
  int16_t              buf[512]; // 256 stereo pairs per chunk
  int                  npairs;
  int                  framebytes; // bytes per stereo frame (L + R, 16-bit)
  int                  chunkbytes;
  int                  i;
  slopay_chip_sample_t sample;

  NOT_USED(total_amount);

  framebytes = 2 * (int) sizeof(*buf);

  while (additional_amount > 0)
  {
    npairs = additional_amount / framebytes;
    if (npairs > (int) (sizeof(buf) / sizeof(*buf) / 2))
      npairs = sizeof(buf) / sizeof(*buf) / 2;
    if (npairs <= 0)
      break;

    for (i = 0; i < npairs; i++)
    {
      int beeper;
      int left;
      int right;

      beeper = chq_apply_due_audio_events(state);

      sample = slopay_chip_get_sample(state->ay);
      left   = (int16_t) ((sample >>  0) & 0xFFFF) + beeper;
      right  = (int16_t) ((sample >> 16) & 0xFFFF) + beeper;
      if (state->audio_muted)
      {
        left = right = 0;
      }
      else
      {
        left  = left  * state->volume / 100;
        right = right * state->volume / 100;
      }
      buf[i * 2 + 0] = (int16_t) CLAMP(left,  INT16_MIN, INT16_MAX);
      buf[i * 2 + 1] = (int16_t) CLAMP(right, INT16_MIN, INT16_MAX);
      state->samples_played++;
    }

    chunkbytes = npairs * framebytes;
    SDL_PutAudioStreamData(stream, buf, chunkbytes);
    additional_amount -= chunkbytes;
  }
}

static int chq_game_thread(void *opaque)
{
  chq_sdl_state_t *state = opaque;

  chq_setup(state->game, state->mode_128k);
  state->quit = 1;
  return 0;
}

// CRT shader tuning knobs, cycled with TAB (Shift-TAB steps backwards) and
// adjusted with PAGEUP/PAGEDOWN (see chq_sdl_key_pressed).
//
// [offset] indexes into chq_CRT_params_t so one table drives all the controls
// instead of one keybinding per field.
typedef struct
{
  const char *name;
  size_t      offset;
  float       step;
  float       min;
  float       max;
}
chq_crt_param_desc_t;

static const chq_crt_param_desc_t chq_crt_param_descs[] =
{
  { "curvature",          offsetof(chq_CRT_params_t, curvature),          0.005f, 0.0f, 0.2f },
  { "bloom threshold",    offsetof(chq_CRT_params_t, bloom_threshold),    0.05f,  0.0f, 1.0f },
  { "bloom intensity",    offsetof(chq_CRT_params_t, bloom_intensity),    0.005f, 0.0f, 0.2f },
  { "brightness",         offsetof(chq_CRT_params_t, brightness),         0.05f,  0.5f, 2.0f },
  { "contrast",           offsetof(chq_CRT_params_t, contrast),           0.05f,  0.5f, 2.0f },
  { "saturation",         offsetof(chq_CRT_params_t, saturation),         0.05f,  0.0f, 2.0f },
  { "scanline intensity", offsetof(chq_CRT_params_t, scanline_intensity), 0.05f,  0.0f, 1.0f },
  { "vignette strength",  offsetof(chq_CRT_params_t, vignette_strength),  0.05f,  0.0f, 1.0f },
  { "chroma bleed",       offsetof(chq_CRT_params_t, chroma_bleed),       0.05f,  0.0f, 1.0f },
  { "glitch",             offsetof(chq_CRT_params_t, glitch),             0.05f,  0.0f, 1.0f },
};

#define CHQ_CRT_PARAM_COUNT \
  (int) (sizeof(chq_crt_param_descs) / sizeof(chq_crt_param_descs[0]))

static float *chq_crt_param_field(chq_CRT_params_t           *params,
                                  const chq_crt_param_desc_t *desc)
{
  return (float *) ((char *) params + desc->offset);
}

/* ----------------------------------------------------------------------- */

// Display backend setup. Only one backend may hold the window at a time, so
// each of these tears its own down completely before the other is built.

static void chq_renderer_destroy(chq_sdl_state_t *state)
{
  if (state->texture != NULL)
  {
    SDL_DestroyTexture(state->texture);
    state->texture = NULL;
  }
  if (state->renderer != NULL)
  {
    SDL_DestroyRenderer(state->renderer);
    state->renderer = NULL;
  }
}

static int chq_renderer_create(chq_sdl_state_t *state)
{
  // Every failure below tears down what it got so far, so the caller can
  // read "renderer == NULL" as "no plain renderer" without having to know
  // how far this got.
  state->renderer = SDL_CreateRenderer(state->window, NULL);
  if (state->renderer == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateRenderer: %s\n", SDL_GetError());
    return 0;
  }

  SDL_SetRenderVSync(state->renderer, 1);

  // The screen buffer is always converted with R in the lowest memory byte
  // (zxconfig.bgr_pixels below), because that is what the CRT backend's
  // R8G8B8A8 GPU texture requires and the backend can change at any time.
  // SDL_PIXELFORMAT_ABGR8888 is the same order for this texture; SDL
  // converts on upload if the renderer would rather have something else.
  state->texture = SDL_CreateTexture(state->renderer,
                                     SDL_PIXELFORMAT_ABGR8888,
                                     SDL_TEXTUREACCESS_STREAMING,
                                     GAMEWIDTH, GAMEHEIGHT);
  if (state->texture == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateTexture: %s\n", SDL_GetError());
    chq_renderer_destroy(state);
    return 0;
  }

  if (!SDL_SetTextureBlendMode(state->texture, SDL_BLENDMODE_NONE))
  {
    fprintf(stderr, "Error: SDL_SetTextureBlendMode: %s\n", SDL_GetError());
    chq_renderer_destroy(state);
    return 0;
  }

  // Conv: nearest-neighbour keeps ZX Spectrum pixels crisp when the window
  // is scaled up; SDL3's default is linear, which blurs them.
  SDL_SetTextureScaleMode(state->texture, SDL_SCALEMODE_NEAREST);

  return 1;
}

// Switches the display backend. Returns 1 if the requested backend is now
// live, 0 if it could not be created and the other one was restored instead
// (the CRT backend needs Metal, so this is the normal result elsewhere).
static int chq_set_crt_enabled(chq_sdl_state_t *state, int enable)
{
  if (enable == state->crt_enabled &&
      (state->crt_enabled || state->renderer != NULL))
    return 1; // already in the requested state

  if (enable)
  {
    chq_renderer_destroy(state);

    if (chq_CRT_shader_create(&state->crt, state->window,
                              GAMEWIDTH, GAMEHEIGHT))
    {
      state->crt_enabled = 1;
      return 1;
    }

    // chq_CRT_shader_create zeroes the struct before it starts, so a NULL
    // device means it failed on the very first step and there is nothing to
    // release; anything later leaves a device that must be given back before
    // the plain renderer can claim the window.
    if (state->crt.gpu != NULL)
      chq_CRT_shader_destroy(&state->crt, state->window);
    memset(&state->crt, 0, sizeof(state->crt));

    fprintf(stderr, "CRT shader unavailable; using the plain renderer\n");
  }
  else if (state->crt_enabled)
  {
    chq_CRT_shader_destroy(&state->crt, state->window);
    memset(&state->crt, 0, sizeof(state->crt));
  }

  state->crt_enabled = 0;

  if (!chq_renderer_create(state))
    return 0; // neither backend is up; the caller has to give up

  return !enable; // the plain renderer is only a success if it was asked for
}

/* ----------------------------------------------------------------------- */

static void chq_sdl_key_pressed(chq_sdl_state_t         *state,
                                const SDL_KeyboardEvent *k)
{
  SDL_Keycode  sym;
  int          down;
  zxjoystick_t j;

  sym = k->key;

  switch (sym)
  {
  case SDLK_F1:
    if (k->down && !k->repeat)
    {
      state->paused = !state->paused;
      chq_update_window_title(state);
    }
    return;

  case SDLK_F2:
    if (k->down && !k->repeat)
    {
      state->audio_muted = !state->audio_muted;
      chq_update_window_title(state);
    }
    return;

  case SDLK_F3:
    if (k->down && !k->repeat)
      state->show_dirty_overlay = !state->show_dirty_overlay;
    return;

  case SDLK_F4:
    if (k->down && !k->repeat)
    {
      chq_set_crt_enabled(state, !state->crt_enabled);
      printf("CRT shader: %s\n", state->crt_enabled ? "on" : "off");
    }
    return;

  case SDLK_F11:
    if (k->down && !k->repeat)
    {
      state->fullscreen = !state->fullscreen;
      SDL_SetWindowFullscreen(state->window, state->fullscreen);
      printf("Fullscreen: %s\n", state->fullscreen ? "on" : "off");
    }
    return;

  case SDLK_MINUS:
  case SDLK_EQUALS:
    if (k->down && !k->repeat)
    {
      int scale;

      scale = CLAMP(state->scale + (sym == SDLK_MINUS ? -1 : 1),
                    SCALE_MIN, SCALE_MAX);

      if (scale != state->scale)
      {
        state->scale = scale;
        SDL_SetWindowSize(state->window,
                          chq_window_width(scale),
                          chq_window_height(scale));
      }
    }
    return;

  case SDLK_LEFTBRACKET:
  case SDLK_RIGHTBRACKET:
  case SDLK_BACKSLASH:
    if (k->down && !k->repeat)
    {
      int speed;

      if (sym == SDLK_BACKSLASH)
        speed = SPEED_DEFAULT;
      else
        speed = CLAMP(state->speed + (sym == SDLK_LEFTBRACKET ? -SPEED_STEP : SPEED_STEP),
                      SPEED_MIN, SPEED_MAX);

      state->speed = speed;
      chq_update_window_title(state);
      printf("Speed: %d%%\n", speed);
    }
    return;

  case SDLK_F5:
  case SDLK_F6:
    if (k->down && !k->repeat)
    {
      int volume;

      volume = CLAMP(state->volume + (sym == SDLK_F5 ? -VOLUME_STEP : VOLUME_STEP),
                     VOLUME_MIN, VOLUME_MAX);

      state->volume = volume;
      chq_update_window_title(state);
      printf("Volume: %d%%\n", volume);
    }
    return;

  // The shader tuning keys only belong to the shader. With the plain
  // renderer up they fall through to the game like any other key.
  case SDLK_TAB:
    if (state->crt_enabled)
    {
      if (k->down && !k->repeat)
      {
        const chq_crt_param_desc_t *desc;
        int                         step;

        // Shift-TAB steps backwards. Adding COUNT keeps the modulus
        // operand positive, since C's % on a negative left operand would
        // give -1 rather than the last index.
        step = (k->mod & SDL_KMOD_SHIFT) ? CHQ_CRT_PARAM_COUNT - 1 : 1;

        state->crt_param_index = (state->crt_param_index + step) % CHQ_CRT_PARAM_COUNT;
        desc = &chq_crt_param_descs[state->crt_param_index];
        printf("CRT param: %s = %g\n", desc->name,
              *chq_crt_param_field(&state->crt_params, desc));
      }
      return;
    }
    j = zxjoystick_UNKNOWN;
    break;

  case SDLK_PAGEUP:
  case SDLK_PAGEDOWN:
    if (state->crt_enabled)
    {
      if (k->down)
      {
        const chq_crt_param_desc_t *desc;
        float                      *field;

        desc  = &chq_crt_param_descs[state->crt_param_index];
        field = chq_crt_param_field(&state->crt_params, desc);
        *field = CLAMP(*field + (sym == SDLK_PAGEDOWN ? -desc->step : desc->step),
                       desc->min, desc->max);
        printf("CRT param: %s = %g\n", desc->name, *field);
      }
      return;
    }
    j = zxjoystick_UNKNOWN;
    break;

  case SDLK_R:
    if (state->crt_enabled)
    {
      if (k->down && !k->repeat)
      {
        chq_CRT_params_t defaults = CHQ_CRT_PARAMS_DEFAULT;

        state->crt_params = defaults;
        printf("CRT params reset to defaults\n");
      }
      return;
    }
    j = zxjoystick_UNKNOWN;
    break;

  case SDLK_LEFT:  j = zxjoystick_LEFT;    break;
  case SDLK_RIGHT: j = zxjoystick_RIGHT;   break;
  case SDLK_UP:    j = zxjoystick_UP;      break;
  case SDLK_DOWN:  j = zxjoystick_DOWN;    break;
  case '.':        j = zxjoystick_FIRE;    break;
  default:         j = zxjoystick_UNKNOWN; break;
  }

  down = k->down;

  if (j != zxjoystick_UNKNOWN)
  {
    zxkempston_assign(&state->kempston, j, down);
  }
  else
  {
    zxkeyset_t *keys;

    keys = &state->keys;
    if (down)
      zxkeyset_setchar(keys, k->key);
    else
      zxkeyset_clearchar(keys, k->key);
  }
}

// Outlines the screen regions chq_draw_handler reported dirty since the last
// frame, so refreshed areas are visible over the rendered texture. Rects are
// in game pixel space (256x192, bottom-left origin); (x, y) is the top-left
// of the game view within the window, already scaled.
//
// Called every frame whichever backend is up, because it also drains the
// list the game thread keeps filling. Only the plain renderer can draw it;
// under the CRT shader it just empties the list and returns.
#define DIRTYOVERLAY_THICKNESS (8) // outline thickness in window pixels

// Draws 'rect' as a filled-in outline DIRTYOVERLAY_THICKNESS pixels thick by
// insetting and stroking it repeatedly (SDL_RenderRect has no line-width).
static void chq_render_thick_rect(SDL_Renderer *renderer, const SDL_FRect *rect)
{
  int i;

  for (i = 0; i < DIRTYOVERLAY_THICKNESS; i++)
  {
    SDL_FRect inset;

    inset.x = rect->x + i;
    inset.y = rect->y + i;
    inset.w = rect->w - i * 2;
    inset.h = rect->h - i * 2;
    if (inset.w <= 0 || inset.h <= 0)
      break;

    SDL_RenderRect(renderer, &inset);
  }
}

static void chq_draw_dirty_overlay(chq_sdl_state_t *state, int x, int y)
{
  zxbox_t rects[MAXDIRTYRECTS];
  int     count;
  int     full_screen;
  int     scale;
  int     i;

  SDL_LockMutex(state->dirty_mutex);
  count       = state->dirty_count;
  full_screen = state->dirty_full_screen;
  for (i = 0; i < count; i++)
    rects[i] = state->dirty_rects[i];
  state->dirty_count       = 0;
  state->dirty_full_screen = 0;
  SDL_UnlockMutex(state->dirty_mutex);

  if (!state->show_dirty_overlay || state->renderer == NULL)
    return;

  if (count > 0 || full_screen)
    fprintf(stderr, "[dirty] count=%d full_screen=%d\n", count, full_screen); // TEMP debug

  scale = state->scale;

  if (full_screen)
  {
    SDL_FRect rect;

    rect.x = (float) x;
    rect.y = (float) y;
    rect.w = (float) (GAMEWIDTH  * scale);
    rect.h = (float) (GAMEHEIGHT * scale);

    SDL_SetRenderDrawColor(state->renderer, 0xFF, 0x00, 0x00, 0xFF); // red: full-screen refresh
    chq_render_thick_rect(state->renderer, &rect);
  }

  SDL_SetRenderDrawColor(state->renderer, 0x00, 0xFF, 0x00, 0xFF); // green: partial refresh
  for (i = 0; i < count; i++)
  {
    const zxbox_t *box = &rects[i];
    SDL_FRect      rect;

    // box is bottom-left origin; flip to the window's top-down space.
    rect.x = (float) (x + box->x0 * scale);
    rect.y = (float) (y + (GAMEHEIGHT - box->y1) * scale);
    rect.w = (float) ((box->x1 - box->x0) * scale);
    rect.h = (float) ((box->y1 - box->y0) * scale);

    chq_render_thick_rect(state->renderer, &rect);
  }
}

// type: em_arg_callback_func
static void chq_sdl_main_loop(void *opaque)
{
  chq_sdl_state_t *state = opaque;
  int              x, y, w, h; // destination rect: game view within the window
  int              ww, wh;     // actual window size (may exceed scale*game size in fullscreen)

  w = GAMEWIDTH  * state->scale;
  h = GAMEHEIGHT * state->scale;
  SDL_GetWindowSize(state->window, &ww, &wh);
  x = (ww - w) / 2; // centred; equals BORDER*scale in windowed mode, letterboxes in fullscreen
  y = (wh - h) / 2;

  {
    SDL_Event event;

    // Consume all pending events
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_EVENT_QUIT:
        state->quit = 1;
        SDL_Log("Quitting after %llu ns", (unsigned long long) event.quit.timestamp);
        break;

      case SDL_EVENT_KEY_DOWN:
      case SDL_EVENT_KEY_UP:
        chq_sdl_key_pressed(state, &event.key);
        break;

      case SDL_EVENT_TEXT_EDITING:
      case SDL_EVENT_TEXT_INPUT:
        break;

      case SDL_EVENT_MOUSE_MOTION:
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
      case SDL_EVENT_MOUSE_BUTTON_UP:
      case SDL_EVENT_MOUSE_WHEEL:
        break;

      default:
        break;
      }
    }

    if (state->quit)
      return;

    if (state->crt_enabled)
    {
      chq_CRT_shader_render(&state->crt, state->window, state->zx,
                            x, y, w, h, GAMEWIDTH, GAMEHEIGHT,
                            &state->crt_params);
      chq_draw_dirty_overlay(state, x, y); // drains the list; draws nothing
    }
    else
    {
      /* Update the texture from the game's converted screen buffer. */
      uint32_t  *pixels;
      SDL_FRect  dstrect;

      dstrect.x = (float) x;
      dstrect.y = (float) y;
      dstrect.w = (float) w;
      dstrect.h = (float) h;

      pixels = zxspectrum_claim_screen(state->zx);
      SDL_UpdateTexture(state->texture, NULL, pixels, GAMEWIDTH * 4);
      zxspectrum_release_screen(state->zx);

      /* Clear screen */
      // TODO: This ought to be the border colour, but CHQ's is always black.
      SDL_SetRenderDrawColor(state->renderer, 0x00, 0x00, 0x00, 0xFF);
      SDL_RenderClear(state->renderer);

      /* Offset the image */
      // Note that this will inhibit image stretching.

      SDL_RenderTexture(state->renderer, state->texture, NULL, &dstrect);
      chq_draw_dirty_overlay(state, x, y);
      SDL_RenderPresent(state->renderer);
    }
  }
}

int main(int argc, char *argv[])
{
  chq_sdl_state_t state;
  zxconfig_t      zxconfig;
  SDL_Window     *window;
  int             arg;
  int             mode_128k;

  mode_128k = 1;

  for (arg = 1; arg < argc; arg++)
  {
    if (strcmp(argv[arg], "-48k") == 0)
      mode_128k = 0;
    else if (strcmp(argv[arg], "-128k") == 0)
      mode_128k = 1;
    else
    {
      fprintf(stderr, "Usage: %s [-48k | -128k]\n", argv[0]);
      return EXIT_FAILURE;
    }
  }

  printf("CHASE H.Q.\n");
  printf("==========\n");

  printf("Initialising in %s mode...\n", mode_128k ? "128K" : "48K");

  memset(&state, 0, sizeof(state));

  zxkeyset_clear(&state.keys);
  state.kempston  = 0;
  state.paused    = 0;
  state.quit      = 0;
  state.mode_128k = mode_128k;
  state.scale     = SCALE_DEFAULT;
  state.speed     = SPEED_DEFAULT;
  state.volume    = VOLUME_DEFAULT;
  {
    chq_CRT_params_t defaults = CHQ_CRT_PARAMS_DEFAULT;

    state.crt_params = defaults;
  }
  state.crt_param_index = 0;

#ifdef __APPLE__
  // Conv: disable macOS press-and-hold accent popover so held keys repeat
  // instead of opening the accent picker.
  CFPreferencesSetAppValue(CFSTR("ApplePressAndHoldEnabled"),
                            kCFBooleanFalse,
                            kCFPreferencesCurrentApplication);
  CFPreferencesAppSynchronize(kCFPreferencesCurrentApplication);
#endif

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
  {
    fprintf(stderr, "Error: SDL_Init: %s\n", SDL_GetError());
    goto failure;
  }

  window = SDL_CreateWindow("Chase H.Q.",
                            chq_window_width(state.scale),
                            chq_window_height(state.scale),
                            0);
  if (window == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateWindow: %s\n", SDL_GetError());
    goto failure;
  }

  state.window = window;

  chq_update_window_title(&state);

  // The GPU texture is always SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM (R in the
  // lowest memory byte), which is what Screen.c's palette_abgr table packs
  // when bgr_pixels is true - despite the name, "bgr_pixels" selects which
  // palette table to use, not which byte order it produces. See the
  // 0x00RRGGBB/0x00BBGGRR comments in Screen.c.
  //
  // Conv: this is fixed rather than matched to the renderer's preferred
  // format, as it was when the backend was chosen at compile time. The
  // backend can now change at any keypress but the conversion palette is
  // picked once, so both backends have to agree; the CRT one cannot bend, so
  // the plain renderer takes an ABGR8888 texture to suit (see
  // chq_renderer_create).
  zxconfig.width      = GAMEWIDTH / 8;
  zxconfig.height     = GAMEHEIGHT / 8;
  zxconfig.opaque     = &state;
  zxconfig.draw       = &chq_draw_handler;
  zxconfig.stamp      = &chq_stamp_handler;
  zxconfig.sleep      = &chq_sleep_handler;
  zxconfig.key        = &chq_key_handler;
  zxconfig.border     = &chq_border_handler;
  zxconfig.speaker    = &chq_speaker_handler;
  zxconfig.ay_out     = &chq_ay_out_handler;
  zxconfig.bgr_pixels = true; /* R in lower byte */

  state.zx = zxspectrum_create(&zxconfig);
  if (state.zx == NULL)
    goto failure;

  state.ay = slopay_chip_create(AY_CLOCK_FREQ, AY_SAMPLE_RATE);
  if (state.ay == NULL)
    goto failure;

  state.audio_queue_mutex = SDL_CreateMutex();
  if (state.audio_queue_mutex == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateMutex: %s\n", SDL_GetError());
    goto failure;
  }

  state.dirty_mutex = SDL_CreateMutex();
  if (state.dirty_mutex == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateMutex: %s\n", SDL_GetError());
    goto failure;
  }

  // Conv: force mono output. The chip defaults to ABC stereo separation
  // (left = A+B, right = B+C), which sounds right-heavy or left-heavy
  // depending on which channels a given tune favours; we don't know whether
  // this game's music assumes ABC, ACB, or no separation at all, so mono
  // sidesteps the question. Volume is turned down from the chip's own
  // default (10%) as three channels plus envelope can otherwise clip loud.
  slopay_chip_set_stereo_mode(state.ay, SLOPAY_CHIP_STEREO_MODE_MONO);
  slopay_chip_set_volume(state.ay, AY_VOLUME_PCT);

  {
    SDL_AudioSpec desired = {0};

    desired.freq     = AY_SAMPLE_RATE;
    desired.format   = SDL_AUDIO_S16;
    desired.channels = 2;

    state.audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                                   &desired,
                                                   &chq_audio_callback,
                                                   &state);
    if (state.audio_stream == NULL)
    {
      fprintf(stderr, "Error: SDL_OpenAudioDeviceStream: %s\n", SDL_GetError());
      goto failure;
    }

    SDL_ResumeAudioStreamDevice(state.audio_stream);
  }

  // Bring up the starting backend. A CRT request that cannot be met falls
  // back to the plain renderer rather than failing to start; only losing
  // both is fatal.
  chq_set_crt_enabled(&state, CHQ_CRT_SHADER);
  if (!state.crt_enabled && state.renderer == NULL)
    goto failure;

  printf("CRT shader: %s (F4 toggles)\n", state.crt_enabled ? "on" : "off");

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

#ifdef CHQ_GRACEFUL_SHUTDOWN
  SDL_WaitThread(state.game_thread, NULL);

  chq_destroy(state.game);
  zxspectrum_destroy(state.zx);

  SDL_DestroyAudioStream(state.audio_stream);
  slopay_chip_destroy(state.ay);
  SDL_DestroyMutex(state.audio_queue_mutex);
  SDL_DestroyMutex(state.dirty_mutex);

  if (state.crt_enabled)
    chq_CRT_shader_destroy(&state.crt, window);
  else
    chq_renderer_destroy(&state);
  SDL_DestroyWindow(window);

  SDL_Quit();
#else
  // Give the game thread 500ms to exit cleanly, then bail out. A hung game
  // thread (translation bug in an inner loop that never calls sleep) would
  // block SDL_WaitThread indefinitely.
  SDL_DetachThread(state.game_thread);
  usleep(500000);
#endif

  printf("(quit)\n");

  exit(EXIT_SUCCESS);


failure:

  exit(EXIT_FAILURE);
}
