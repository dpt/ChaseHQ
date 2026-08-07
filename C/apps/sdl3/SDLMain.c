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

/* Configuration
 *
 */
#define GAMEWIDTH          (256)
#define GAMEHEIGHT         (192)
#define BORDER              (16)

#define SCALE_DEFAULT        (4)
#define SCALE_MIN            (1)
#define SCALE_MAX            (4)

#define SPEED_DEFAULT      (100) // percent
#define SPEED_MIN           (25)
#define SPEED_MAX         (1000)
#define SPEED_STEP           (5)

#define VOLUME_DEFAULT     (100) // percent
#define VOLUME_MIN           (0)
#define VOLUME_MAX         (100)
#define VOLUME_STEP         (10)

/* Whether the CRT post-effect starts switched on. Both display backends are
 * always built: the plain SDL_Renderer blit (any GPU backend,
 * nearest-neighbour scaling) and the SDL3 GPU/Metal CRT post-effect pipeline
 * (Metal only). F4 switches between them at runtime, so this only chooses
 * which one comes up first. Override with -DCHQ_CRT_SHADER=1.
 *
 * They cannot both be live at once -- SDL_CreateRenderer and
 * SDL_ClaimWindowForGPUDevice each want the window's swapchain -- so
 * switching tears one down and builds the other. If the CRT backend will not
 * start (no Metal), chq_set_crt_enabled falls back to the plain renderer.
 */
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

/* SlopAY-style beeper/AY mix: the beeper is a hard on/off level, so summing
 * it straight into the AY signal biases the waveform upward whenever it is
 * high. AY_GAIN_WITH_BEEPER headroom keeps that sum inside int16 range, and
 * the DC-block one-pole highpass removes the resulting bias before the sum.
 */
#define AY_GAIN_WITH_BEEPER (0.90f)
#define BEEPER_DC_BLOCK_R   (0.995f)

/* Sampled speech drives the AY DAC by writing a new volume-register value
 * every ~120us (~8.4KHz) from the game thread. The audio thread only pulls
 * PCM from slopay_chip_get_sample() in bursts whenever SDL wants more data,
 * so a naive "read the current register value" approach loses every write
 * that happened between bursts. Instead, register writes are queued here
 * with a timestamp and replayed one sample at a time against a matching
 * virtual audio clock (see chq_apply_due_audio_events), so the reconstructed
 * waveform reflects the write history rather than a single stale snapshot.
 * The same queue carries 48K beeper level changes, timestamped from the
 * game's virtual T-state clock (see chq_speaker_handler).
 */
#define AY_QUEUE_CAPACITY (16384) // ~1.9s of nibble writes at ~8.4KHz; ample headroom

/* Replay cursor anchor cushion; see the replay cursor comment in
 * chq_sdl_state_t.
 */
#define AY_REPLAY_CUSHION_NS (30000000ULL)

// -----------------------------------------------------------------------------

// Defaults re-tuned by eye against this game's screen; see CRTShader.c.
static const chq_CRT_params_t crt_default_params = {
  0.025f,
  0.5f,
  0.025f,
  1.1f,
  1.1f,
  1.0f,
  0.75f,
  0.3f,
  0.6f,
  0.0f,
  0.0f
};

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

// chq_sdl_state_t.flags bits
#define CHQ_FLAG_QUIT      (1u << 0)
#define CHQ_FLAG_PAUSED    (1u << 1)
#define CHQ_FLAG_MODE_128K (1u << 2) // 0 selects the 48K entry point

#define CHQ_FLAG_TEST(state, flag)   (((state)->flags & (flag)) != 0)
#define CHQ_FLAG_SET(state, flag)    ((state)->flags |= (flag))
#define CHQ_FLAG_CLEAR(state, flag)  ((state)->flags &= ~(flag))
#define CHQ_FLAG_ASSIGN(state, flag, on) \
  ((on) ? CHQ_FLAG_SET(state, flag) : CHQ_FLAG_CLEAR(state, flag))

typedef struct chq_sdl_state
{
  zxspectrum_t     *zx;
  chqstate_t       *game;

  zxkeyset_t        keys;
  zxkempston_t      kempston;

  unsigned int      flags;      // CHQ_FLAG_* bits: quit, paused, mode_128k

  int               speed;      // game speed, percent, SPEED_MIN..SPEED_MAX

  struct timeval    stamps[MAXSTAMPS];
  int               nstamps;

  /* Absolute wall-clock deadline for the next sleep, advanced by each call's
   * nominal duration rather than re-anchored from "now" -- see
   * chq_sleep_handler for why. */
  double            next_deadline;  // seconds, gettimeofday-epoch
  int               deadline_valid; // bool

  struct
  {
    int             volume; // output volume, percent, VOLUME_MIN..VOLUME_MAX

    slopay_chip_t  *ay;
    slopay_chip_reg_t ay_latched_reg; // register selected by last port_AY_REGISTER write

    /* Timestamped audio event queue (AY register writes and beeper level
     * changes): game thread produces, audio thread consumes. queue_mutex
     * guards head/tail and the slots between them.
     */
    SDL_Mutex      *queue_mutex;
    chq_audio_event_t queue[AY_QUEUE_CAPACITY];
    int             queue_head;
    int             queue_tail;

    /* Audio clock anchor, mapping the facade's virtual T-state clock onto
     * wall-clock ns. The game thread emits a frame's worth of audio in a
     * fraction of that frame's wall time, so event times extrapolate from
     * the anchor at the T-state rate rather than being read off the wall
     * clock; if the T-state clock falls behind wall time the game thread
     * has lost ground and the anchor resets to catch up. Game thread only.
     *
     * Both event types must go through this: they share one queue and one
     * replay cursor, so timestamping AY writes off the wall clock while the
     * beeper used the T-state clock made every frame's drums land wherever
     * the two clocks happened to disagree.
     */
    Uint64          anchor_ns;
    Uint64          anchor_tstates;
    int             speaker_last_level;  // last queued level (game thread)
    int             speaker_level;       // current replay level (audio thread)
    float           speaker_dc_prev_in;  // DC-block filter state (audio thread)
    float           speaker_dc_prev_out; // DC-block filter state (audio thread)

    /* Replay cursor: maps generated samples onto event timestamps. The wall
     * clock and the audio device's sample clock drift apart (measured
     * ~1.4ms/s here), so event times are never compared against a
     * wall-clock-anchored sample time. Instead, whenever the queue runs dry
     * the cursor re-anchors to the next event's timestamp minus
     * AY_REPLAY_CUSHION_NS, then advances by exactly one sample period per
     * generated sample. Spacing within a burst is preserved regardless of
     * drift and every gap in the audio re-syncs the two clocks. The cushion
     * (extra output latency) absorbs consumer-fast drift so a continuous
     * stream (a speech sample) does not starve mid-burst: at the measured
     * drift, 30ms lasts ~21s of continuous writes and speech samples are
     * only ~1-2s long.
     */
    Uint64          samples_played;
    Uint64          replay_anchor_ns;     // event time at the last anchor
    Uint64          replay_anchor_sample; // samples_played at the last anchor
    int             replay_anchored;      // bool; cleared when queue runs dry
    int             muted;                // bool; mute audio if true
    int             ay_channel_muted[3];  // bool per AY channel A/B/C; toggled with F7/F8/F9
    int             speaker_muted;        // bool; beeper sfx muted; toggled with F10

    SDL_AudioStream *stream;
  }
  audio;

  struct
  {
    int             scale;      // window/render scale, SCALE_MIN..SCALE_MAX
    int             fullscreen; // bool; toggled with F11

    /* Dirty-rect overlay: the game (thread) reports each screen region it
     * refreshes via chq_draw_handler; we stash them here and outline them
     * over the rendered frame so refreshed regions are visible on screen.
     * Cleared once drawn. dirty_mutex guards all fields in this group.
     */
    SDL_Mutex      *dirty_mutex;
    zxbox_t         dirty_rects[MAXDIRTYRECTS];
    int             dirty_count;
    int             dirty_full_screen; // bool; a NULL dirty box was reported (whole screen)
    int             show_dirty_overlay; // bool; toggled with F3, off by default

    SDL_Window     *window;

    /* Display backend. Exactly one of these is live at a time, selected by
     * crt_enabled and swapped by chq_set_crt_enabled: crt when enabled,
     * renderer/texture when not. The other's handles are NULL.
     */
    int             crt_enabled; // bool; toggled with F4
    chq_CRT_shader_t crt;
    chq_CRT_params_t crt_params;
    int             crt_param_index; // which crt_params field +/- adjusts
    SDL_Renderer   *renderer;
    SDL_Texture    *texture;
  }
  video;

  SDL_Thread       *game_thread;
}
chq_sdl_state_t;

/* ----------------------------------------------------------------------- */

/* The title bar doubles as the status display for the F-key controls: the
 * game itself has nowhere to show them.
 */
static void chq_update_window_title(const chq_sdl_state_t *state)
{
  char title[64];

  SDL_snprintf(title, sizeof(title),
               "Chase H.Q. - Speed: %d%%%s - Volume: %d%%%s",
               state->speed,
               CHQ_FLAG_TEST(state, CHQ_FLAG_PAUSED) ? " (Paused)" : "",
               state->audio.volume,
               state->audio.muted ? " (Muted)" : "");
  SDL_SetWindowTitle(state->video.window, title);
}

static void chq_draw_handler(const zxbox_t *dirty,
                             void          *opaque)
{
  chq_sdl_state_t *state = opaque;

  /* SDL_UpdateTexture must be called from the main thread (Metal requirement).
   * The main loop picks up changes via zxspectrum_claim_screen. Here we only
   * stash the dirty region (game thread) for the main loop to outline once it
   * renders the frame.
   */
  SDL_LockMutex(state->video.dirty_mutex);
  if (dirty == NULL)
    state->video.dirty_full_screen = 1;
  else if (state->video.dirty_count < MAXDIRTYRECTS)
    state->video.dirty_rects[state->video.dirty_count++] = *dirty;
  SDL_UnlockMutex(state->video.dirty_mutex);
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
    return CHQ_FLAG_TEST(state, CHQ_FLAG_QUIT);
  --state->nstamps;

  // Quit straight away if signalled
  if (CHQ_FLAG_TEST(state, CHQ_FLAG_QUIT))
    return 1;

  paused = CHQ_FLAG_TEST(state, CHQ_FLAG_PAUSED);
  if (paused)
  {
    // If paused, sit in this loop, checking twice per second for unpausing
    for (;;)
    {
      paused = CHQ_FLAG_TEST(state, CHQ_FLAG_PAUSED);
      if (!paused)
        break;

      usleep(500000); // 0.5s
    }
  }
  else
  {
    /* A Spectrum 48K's Z80 runs at 3.5MHz (3,500,000 T-states per second).
     * A 128K runs its Z80 faster, at 3.5469MHz (3,546,900 T-states per
     * second) -- pacing every mode at the 48K rate throttles wall-clock time
     * per T-state on a 128K, which slows the beeper's real-time toggle rate
     * (timed by how fast the game thread actually runs) without affecting
     * the AY chip's own tone pitch (computed from its own fixed clock,
     * independent of this loop) -- the two drift ~1.3% apart.
     */
    const double   tstatesPerSec = CHQ_FLAG_TEST(state, CHQ_FLAG_MODE_128K) ? 3546900.0 : 3.5e6;
    const double   maxLagFrames  = 4.0; // cap catch-up burst after a stall/pause

    struct timeval now;
    double         nowSecs;
    double         duration; // seconds

    gettimeofday(&now, NULL); // get time now before anything else
    nowSecs = now.tv_sec + now.tv_usec / 1e6;

    /* 'duration' tells us how long the operation should take. Turn T-state
     * duration into seconds. */
    duration = durationTStates / tstatesPerSec;
    // Adjust the game speed: higher speed -> shorter sleep
    duration = duration * 100 / state->speed;

    /* Pace off an absolute deadline that advances by 'duration' every call,
     * rather than re-anchoring from 'now' each time. usleep() on this host
     * routinely overshoots its requested delay by a few ms (OS scheduler
     * granularity); re-anchoring from 'now' every call bakes that overshoot
     * into every single frame with nothing to claw it back, producing a
     * steady-state frame rate well under 50Hz (measured ~42fps) even though
     * each call's own math looks correct in isolation. Advancing a
     * fixed schedule instead means an overshoot simply shortens (or zeroes)
     * the next call's sleep, so the long-run average rate is right.
     */
    if (!state->deadline_valid)
    {
      state->next_deadline  = nowSecs + duration;
      state->deadline_valid = 1;
    }
    else
    {
      state->next_deadline += duration;

      // Behind by more than a few frames (paused, breakpoint, host stall) --
      // don't try to fast-forward through the whole backlog.
      if (state->next_deadline < nowSecs - duration * maxLagFrames)
        state->next_deadline = nowSecs - duration * maxLagFrames;
    }

    if (state->next_deadline > nowSecs)
    {
      double     delay = state->next_deadline - nowSecs; // seconds
      useconds_t udelay = (useconds_t) (delay * 1e6);

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
  SDL_LockMutex(state->audio.queue_mutex);

  {
    int next_tail = (state->audio.queue_tail + 1) % AY_QUEUE_CAPACITY;

    if (next_tail != state->audio.queue_head) // drop event if the queue is full
    {
      chq_audio_event_t *ev = &state->audio.queue[state->audio.queue_tail];

      ev->time_ns = time_ns;
      ev->type    = type;
      ev->reg     = reg;
      ev->value   = value;

      state->audio.queue_tail = next_tail;
    }
  }

  SDL_UnlockMutex(state->audio.queue_mutex);
}

// Maps a virtual T-state to the wall-clock ns the event should be heard at.
// 1 T-state = 1e9 / tstatesPerSec ns, where tstatesPerSec matches the Z80
// clock chq_sleep_handler paces the game thread against (3.5MHz on 48K,
// 3.5469MHz on 128K) — using the wrong one here mis-schedules every AY and
// beeper event's real playback time by the same ratio chq_sleep_handler
// would otherwise mis-pace the game thread, and this is the function that
// actually drives it, not the pacing loop. Game thread only; see the anchor
// comment in chq_sdl_state_t.
static Uint64 chq_tstates_to_ns(chq_sdl_state_t *state, uint64_t tstates)
{
  const double tstatesPerSec = CHQ_FLAG_TEST(state, CHQ_FLAG_MODE_128K) ? 3546900.0 : 3.5e6;
  const double nsPerTstate   = 1.0e9 / tstatesPerSec;
  Uint64       now_ns;
  Uint64       event_ns;

  now_ns = SDL_GetTicksNS();

  // Scaled by the speed setting for the same reason chq_sleep_handler scales
  // its sleep: at 200% the game emits two frames of T-states in one frame of
  // wall time, so a fixed rate would date every event further into the
  // future than the last and the anchor would never catch up.
  event_ns = state->audio.anchor_ns +
             (Uint64) ((tstates - state->audio.anchor_tstates) * nsPerTstate *
                       100 / state->speed);
  if (event_ns < now_ns) // T-state clock fell behind wall clock: re-anchor
  {
    state->audio.anchor_ns      = now_ns;
    state->audio.anchor_tstates = tstates;
    event_ns = now_ns;
  }

  return event_ns;
}

static void chq_speaker_handler(int on_off, uint64_t tstates, void *opaque)
{
  chq_sdl_state_t *state = opaque;

  if (on_off == state->audio.speaker_last_level)
    return; // level unchanged: no edge to reproduce

  state->audio.speaker_last_level = on_off;

  chq_audio_queue_push(state, chq_tstates_to_ns(state, tstates),
                       CHQ_AUDIO_EVENT_SPEAKER, 0, on_off);
}

static void chq_ay_out_handler(uint16_t port,
                               uint8_t  byte,
                               uint64_t tstates,
                               void    *opaque)
{
  chq_sdl_state_t *state = opaque;

  if (port == port_AY_REGISTER)
  {
    /* Register select: only ever touched from the game thread, immediately
     * followed by the paired data write below, so no queuing needed here.
     */
    state->audio.ay_latched_reg = byte;
    return;
  }

  /* port_AY_DATA: queue the write with a timestamp instead of applying it
   * immediately, so chq_audio_callback can replay it at the right sample
   * position (see the AY_QUEUE_CAPACITY comment above). Timestamped from the
   * same T-state clock as the beeper: the queue is a FIFO drained in order,
   * so two clocks here means one stream's events are ordered against times
   * the other's were never measured on.
   */
  chq_audio_queue_push(state,
                       chq_tstates_to_ns(state, tstates),
                       CHQ_AUDIO_EVENT_AY,
                       state->audio.ay_latched_reg,
                       byte);
}

/* Applies any queued audio events (AY register writes and beeper level
 * changes) due within the current output sample's period, and returns the
 * beeper contribution for that sample. Called once per generated sample
 * from chq_audio_callback so rapid writes (e.g. sampled speech) land on the
 * correct output sample rather than all being collapsed into whichever
 * value was current when the audio callback happened to run. See the replay
 * cursor comment in chq_sdl_state_t for how clock drift is handled.
 *
 * Beeper level changes can arrive faster than the sample rate (the 48K
 * engine tone toggles every ~60 T-states, under one 44.1kHz sample), so
 * sampling the instantaneous level would alias or silence them entirely.
 * Instead the level is integrated over the sample period (a box filter):
 * the returned value is BEEPER_AMPLITUDE scaled by the fraction of the
 * period the speaker spent high.
 */
static int chq_apply_due_audio_events(chq_sdl_state_t *state)
{
  Uint64 start_ns; // sample period start on the replay cursor timeline
  Uint64 end_ns;   // sample period end
  Uint64 level_ns; // start of the current beeper level within the period
  Uint64 high_ns;  // time spent high within the period
  int    beeper;

  SDL_LockMutex(state->audio.queue_mutex);

  if (state->audio.queue_head == state->audio.queue_tail)
  {
    state->audio.replay_anchored = 0;

    // No pending edges: the speaker holds its level for the whole period.
    beeper = state->audio.speaker_level ? BEEPER_AMPLITUDE : 0;
  }
  else
  {
    if (!state->audio.replay_anchored)
    {
      Uint64 head_time_ns = state->audio.queue[state->audio.queue_head].time_ns;

      /* SDL_GetTicksNS() starts near zero, so guard the cushion subtraction
       * against underflow for writes made just after launch.
       */
      state->audio.replay_anchor_ns =
        (head_time_ns > AY_REPLAY_CUSHION_NS) ?
          head_time_ns - AY_REPLAY_CUSHION_NS : 0;
      state->audio.replay_anchor_sample = state->audio.samples_played;
      state->audio.replay_anchored = 1;
    }

    /* Computed fresh from the anchor rather than accumulated, so truncation
     * never drifts the cursor away from the sample count.
     */
    start_ns = state->audio.replay_anchor_ns +
               ((state->audio.samples_played - state->audio.replay_anchor_sample) *
                1000000000ULL) / AY_SAMPLE_RATE;
    end_ns   = state->audio.replay_anchor_ns +
               ((state->audio.samples_played + 1 - state->audio.replay_anchor_sample) *
                1000000000ULL) / AY_SAMPLE_RATE;

    level_ns = start_ns;
    high_ns  = 0;

    while (state->audio.queue_head != state->audio.queue_tail)
    {
      chq_audio_event_t *ev = &state->audio.queue[state->audio.queue_head];

      if (ev->time_ns > end_ns)
        break;

      if (ev->type == CHQ_AUDIO_EVENT_AY)
      {
        slopay_chip_write_register(state->audio.ay, ev->reg, ev->value);
      }
      else
      {
        Uint64 edge_ns = ev->time_ns;

        if (edge_ns < start_ns) // overdue edge: takes effect at period start
          edge_ns = start_ns;
        if (state->audio.speaker_level)
          high_ns += edge_ns - level_ns;
        level_ns = edge_ns;
        state->audio.speaker_level = ev->value;
      }

      state->audio.queue_head = (state->audio.queue_head + 1) %
                                AY_QUEUE_CAPACITY;
    }

    if (state->audio.speaker_level)
      high_ns += end_ns - level_ns;

    beeper = (int) ((Uint64) BEEPER_AMPLITUDE * high_ns /
                    (end_ns - start_ns));
  }

  SDL_UnlockMutex(state->audio.queue_mutex);

  return state->audio.speaker_muted ? 0 : beeper;
}

// One-pole highpass removing the DC bias the box-filtered beeper level
// (chq_apply_due_audio_events) introduces before it is summed with the AY
// signal. Filter state lives on the audio thread only.
static float chq_beeper_dc_block(chq_sdl_state_t *state, int beeper_raw)
{
  float in;
  float out;

  in  = (float) beeper_raw;
  out = (in - state->audio.speaker_dc_prev_in) +
        (BEEPER_DC_BLOCK_R * state->audio.speaker_dc_prev_out);

  state->audio.speaker_dc_prev_in  = in;
  state->audio.speaker_dc_prev_out = out;

  return out;
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
    if (npairs > (int) (NELEMS(buf) / 2))
      npairs = NELEMS(buf) / 2;
    if (npairs <= 0)
      break;

    for (i = 0; i < npairs; i++)
    {
      int   beeper_raw;
      float beeper;
      int   left;
      int   right;

      beeper_raw = chq_apply_due_audio_events(state);
      beeper     = chq_beeper_dc_block(state, beeper_raw);

      sample = slopay_chip_get_sample(state->audio.ay);
      left   = (int) ((int16_t) ((sample >>  0) & 0xFFFF) * AY_GAIN_WITH_BEEPER + beeper);
      right  = (int) ((int16_t) ((sample >> 16) & 0xFFFF) * AY_GAIN_WITH_BEEPER + beeper);
      if (state->audio.muted)
      {
        left = right = 0;
      }
      else
      {
        left  = left  * state->audio.volume / 100;
        right = right * state->audio.volume / 100;
      }
      buf[i * 2 + 0] = CLAMP(left,  INT16_MIN, INT16_MAX);
      buf[i * 2 + 1] = CLAMP(right, INT16_MIN, INT16_MAX);
      state->audio.samples_played++;
    }

    chunkbytes = npairs * framebytes;
    SDL_PutAudioStreamData(stream, buf, chunkbytes);
    additional_amount -= chunkbytes;
  }
}

static int chq_game_thread(void *opaque)
{
  chq_sdl_state_t *state = opaque;

  chq_start(state->game, CHQ_FLAG_TEST(state, CHQ_FLAG_MODE_128K));
  CHQ_FLAG_SET(state, CHQ_FLAG_QUIT);
  return 0;
}

/* CRT shader tuning knobs, cycled with TAB (Shift-TAB steps backwards) and
 * adjusted with PAGEUP/PAGEDOWN (see chq_sdl_key_pressed).
 *
 * [offset] indexes into chq_CRT_params_t so one table drives all the controls
 * instead of one keybinding per field.
 */
typedef struct chq_crt_param_desc
{
  const char *name;
  size_t      offset;
  float       step;
  float       min, max;
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

#define CHQ_CRT_PARAM_COUNT (int) NELEMS(chq_crt_param_descs)

static float *chq_crt_param_field(chq_CRT_params_t           *params,
                                  const chq_crt_param_desc_t *desc)
{
  return (float *) ((char *) params + desc->offset);
}

/* ----------------------------------------------------------------------- */

/* Display backend setup. Only one backend may hold the window at a time, so
 * each of these tears its own down completely before the other is built.
 */

static void chq_renderer_destroy(chq_sdl_state_t *state)
{
  if (state->video.texture != NULL)
  {
    SDL_DestroyTexture(state->video.texture);
    state->video.texture = NULL;
  }
  if (state->video.renderer != NULL)
  {
    SDL_DestroyRenderer(state->video.renderer);
    state->video.renderer = NULL;
  }
}

static int chq_renderer_create(chq_sdl_state_t *state)
{
  /* Every failure below tears down what it got so far, so the caller can
   * read "renderer == NULL" as "no plain renderer" without having to know
   * how far this got.
   */
  state->video.renderer = SDL_CreateRenderer(state->video.window, NULL);
  if (state->video.renderer == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateRenderer: %s\n", SDL_GetError());
    return 0;
  }

  SDL_SetRenderVSync(state->video.renderer, 1);

  /* The screen buffer is always converted with R in the lowest memory byte
   * (zxconfig.bgr_pixels below), because that is what the CRT backend's
   * R8G8B8A8 GPU texture requires and the backend can change at any time.
   * SDL_PIXELFORMAT_ABGR8888 is the same order for this texture; SDL
   * converts on upload if the renderer would rather have something else.
   */
  state->video.texture = SDL_CreateTexture(state->video.renderer,
                                           SDL_PIXELFORMAT_ABGR8888,
                                           SDL_TEXTUREACCESS_STREAMING,
                                           GAMEWIDTH, GAMEHEIGHT);
  if (state->video.texture == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateTexture: %s\n", SDL_GetError());
    chq_renderer_destroy(state);
    return 0;
  }

  if (!SDL_SetTextureBlendMode(state->video.texture, SDL_BLENDMODE_NONE))
  {
    fprintf(stderr, "Error: SDL_SetTextureBlendMode: %s\n", SDL_GetError());
    chq_renderer_destroy(state);
    return 0;
  }

  /* Conv: nearest-neighbour keeps ZX Spectrum pixels crisp when the window
   * is scaled up; SDL3's default is linear, which blurs them.
   */
  SDL_SetTextureScaleMode(state->video.texture, SDL_SCALEMODE_NEAREST);

  return 1;
}

/* Switches the display backend. Returns 1 if the requested backend is now
 * live, 0 if it could not be created and the other one was restored instead
 * (the CRT backend needs Metal, so this is the normal result elsewhere).
 */
static int chq_set_crt_enabled(chq_sdl_state_t *state, int enable)
{
  if (enable == state->video.crt_enabled &&
      (state->video.crt_enabled || state->video.renderer != NULL))
    return 1; // already in the requested state

  if (enable)
  {
    chq_renderer_destroy(state);

    if (chq_CRT_shader_create(&state->video.crt, state->video.window,
                              GAMEWIDTH, GAMEHEIGHT))
    {
      state->video.crt_enabled = 1;
      return 1;
    }

    /* chq_CRT_shader_create zeroes the struct before it starts, so a NULL
     * device means it failed on the very first step and there is nothing to
     * release; anything later leaves a device that must be given back before
     * the plain renderer can claim the window.
     */
    if (state->video.crt.gpu != NULL)
      chq_CRT_shader_destroy(&state->video.crt, state->video.window);
    memset(&state->video.crt, 0, sizeof(state->video.crt));

    fprintf(stderr, "CRT shader unavailable; using the plain renderer\n");
  }
  else if (state->video.crt_enabled)
  {
    chq_CRT_shader_destroy(&state->video.crt, state->video.window);
    memset(&state->video.crt, 0, sizeof(state->video.crt));
  }

  state->video.crt_enabled = 0;

  if (!chq_renderer_create(state))
    return 0; // neither backend is up; the caller has to give up

  return !enable; // the plain renderer is only a success if it was asked for
}

/* ----------------------------------------------------------------------- */

static void chq_action_toggle_pause(chq_sdl_state_t *state)
{
  CHQ_FLAG_ASSIGN(state, CHQ_FLAG_PAUSED, !CHQ_FLAG_TEST(state, CHQ_FLAG_PAUSED));
  chq_update_window_title(state);
}

static void chq_action_toggle_mute(chq_sdl_state_t *state)
{
  state->audio.muted = !state->audio.muted;
  chq_update_window_title(state);
}

static void chq_action_toggle_dirty_overlay(chq_sdl_state_t *state)
{
  state->video.show_dirty_overlay = !state->video.show_dirty_overlay;
}

static void chq_action_toggle_crt(chq_sdl_state_t *state)
{
  chq_set_crt_enabled(state, !state->video.crt_enabled);
  printf("CRT shader: %s\n", state->video.crt_enabled ? "on" : "off");
}

static void chq_action_toggle_ay_channel(chq_sdl_state_t *state, int ch)
{
  state->audio.ay_channel_muted[ch] = !state->audio.ay_channel_muted[ch];
  slopay_chip_enable_channel(state->audio.ay, ch, !state->audio.ay_channel_muted[ch]);
  printf("AY channel %c: %s\n", "ABC"[ch],
         state->audio.ay_channel_muted[ch] ? "muted" : "on");
}

static void chq_action_toggle_speaker_mute(chq_sdl_state_t *state)
{
  state->audio.speaker_muted = !state->audio.speaker_muted;
  printf("Speaker: %s\n", state->audio.speaker_muted ? "muted" : "on");
}

static void chq_action_toggle_fullscreen(chq_sdl_state_t *state)
{
  state->video.fullscreen = !state->video.fullscreen;
  SDL_SetWindowFullscreen(state->video.window, state->video.fullscreen);
  printf("Fullscreen: %s\n", state->video.fullscreen ? "on" : "off");
}

static void chq_action_adjust_scale(chq_sdl_state_t *state, SDL_Keycode sym)
{
  int scale;

  scale = CLAMP(state->video.scale + (sym == SDLK_MINUS ? -1 : 1),
                SCALE_MIN, SCALE_MAX);

  if (scale != state->video.scale)
  {
    state->video.scale = scale;
    SDL_SetWindowSize(state->video.window,
                      chq_window_width(scale),
                      chq_window_height(scale));
  }
}

static void chq_action_adjust_speed(chq_sdl_state_t *state, SDL_Keycode sym, int shift)
{
  int speed;

  if (sym == SDLK_BACKSLASH)
    speed = SPEED_DEFAULT;
  else if (shift)
    speed = sym == SDLK_LEFTBRACKET ? SPEED_MIN : SPEED_MAX;
  else
    speed = CLAMP(state->speed + (sym == SDLK_LEFTBRACKET ? -SPEED_STEP : SPEED_STEP),
                  SPEED_MIN, SPEED_MAX);

  state->speed = speed;
  chq_update_window_title(state);
  printf("Speed: %d%%\n", speed);
}

static void chq_action_adjust_volume(chq_sdl_state_t *state, SDL_Keycode sym)
{
  int volume;

  volume = CLAMP(state->audio.volume + (sym == SDLK_F5 ? -VOLUME_STEP : VOLUME_STEP),
                 VOLUME_MIN, VOLUME_MAX);

  state->audio.volume = volume;
  chq_update_window_title(state);
  printf("Volume: %d%%\n", volume);
}

/* Shift-TAB steps backwards. Adding COUNT keeps the modulus operand
 * positive, since C's % on a negative left operand would give -1 rather
 * than the last index.
 */
static void chq_action_crt_param_next(chq_sdl_state_t *state, int shift)
{
  const chq_crt_param_desc_t *desc;
  int                         step;

  step = shift ? CHQ_CRT_PARAM_COUNT - 1 : 1;

  state->video.crt_param_index = (state->video.crt_param_index + step) % CHQ_CRT_PARAM_COUNT;
  desc = &chq_crt_param_descs[state->video.crt_param_index];
  printf("CRT param: %s = %g\n", desc->name,
        *chq_crt_param_field(&state->video.crt_params, desc));
}

static void chq_action_crt_param_adjust(chq_sdl_state_t *state, SDL_Keycode sym)
{
  const chq_crt_param_desc_t *desc;
  float                      *field;

  desc  = &chq_crt_param_descs[state->video.crt_param_index];
  field = chq_crt_param_field(&state->video.crt_params, desc);
  *field = CLAMP(*field + (sym == SDLK_PAGEDOWN ? -desc->step : desc->step),
                 desc->min, desc->max);
  printf("CRT param: %s = %g\n", desc->name, *field);
}

static void chq_action_crt_params_reset(chq_sdl_state_t *state)
{
  state->video.crt_params = crt_default_params;
  printf("CRT params reset to defaults\n");
}

static void chq_action_joystick_or_key(chq_sdl_state_t *state,
                                       const SDL_KeyboardEvent *k,
                                       zxjoystick_t             j)
{
  int down;

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

static void chq_sdl_key_pressed(chq_sdl_state_t         *state,
                                const SDL_KeyboardEvent *k)
{
  SDL_Keycode  sym;
  zxjoystick_t j;

  sym = k->key;

  switch (sym)
  {
  case SDLK_F1:
    if (k->down && !k->repeat)
      chq_action_toggle_pause(state);
    return;

  case SDLK_F2:
    if (k->down && !k->repeat)
      chq_action_toggle_mute(state);
    return;

  case SDLK_F3:
    if (k->down && !k->repeat)
      chq_action_toggle_dirty_overlay(state);
    return;

  case SDLK_F4:
    if (k->down && !k->repeat)
      chq_action_toggle_crt(state);
    return;

  case SDLK_F7:
  case SDLK_F8:
  case SDLK_F9:
    if (k->down && !k->repeat)
      chq_action_toggle_ay_channel(state, sym - SDLK_F7);
    return;

  case SDLK_F10:
    if (k->down && !k->repeat)
      chq_action_toggle_speaker_mute(state);
    return;

  case SDLK_F11:
    if (k->down && !k->repeat)
      chq_action_toggle_fullscreen(state);
    return;

  case SDLK_MINUS:
  case SDLK_EQUALS:
    if (k->down && !k->repeat)
      chq_action_adjust_scale(state, sym);
    return;

  case SDLK_LEFTBRACKET:
  case SDLK_RIGHTBRACKET:
  case SDLK_BACKSLASH:
    if (k->down)
      chq_action_adjust_speed(state, sym, k->mod & SDL_KMOD_SHIFT);
    return;

  case SDLK_F5:
  case SDLK_F6:
    if (k->down && !k->repeat)
      chq_action_adjust_volume(state, sym);
    return;

  /* The shader tuning keys only belong to the shader. With the plain
   * renderer up they fall through to the game like any other key.
   */
  case SDLK_TAB:
    if (state->video.crt_enabled)
    {
      if (k->down && !k->repeat)
        chq_action_crt_param_next(state, k->mod & SDL_KMOD_SHIFT);
      return;
    }
    j = zxjoystick_UNKNOWN;
    break;

  case SDLK_PAGEUP:
  case SDLK_PAGEDOWN:
    if (state->video.crt_enabled)
    {
      if (k->down)
        chq_action_crt_param_adjust(state, sym);
      return;
    }
    j = zxjoystick_UNKNOWN;
    break;

  case SDLK_R:
    if (state->video.crt_enabled)
    {
      if (k->down && !k->repeat)
        chq_action_crt_params_reset(state);
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

  chq_action_joystick_or_key(state, k, j);
}

/* Outlines the screen regions chq_draw_handler reported dirty since the last
 * frame, so refreshed areas are visible over the rendered texture. Rects are
 * in game pixel space (256x192, bottom-left origin); (x, y) is the top-left
 * of the game view within the window, already scaled.
 *
 * Called every frame whichever backend is up, because it also drains the
 * list the game thread keeps filling. Only the plain renderer can draw it;
 * under the CRT shader it just empties the list and returns.
 */
#define DIRTYOVERLAY_THICKNESS (8) // outline thickness in window pixels

/* Draws 'rect' as a filled-in outline DIRTYOVERLAY_THICKNESS pixels thick by
 * insetting and stroking it repeatedly (SDL_RenderRect has no line-width).
 */
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

  SDL_LockMutex(state->video.dirty_mutex);
  count       = state->video.dirty_count;
  full_screen = state->video.dirty_full_screen;
  for (i = 0; i < count; i++)
    rects[i] = state->video.dirty_rects[i];
  state->video.dirty_count       = 0;
  state->video.dirty_full_screen = 0;
  SDL_UnlockMutex(state->video.dirty_mutex);

  if (!state->video.show_dirty_overlay || state->video.renderer == NULL)
    return;

  if (count > 0 || full_screen)
    fprintf(stderr, "[dirty] count=%d full_screen=%d\n", count, full_screen); // TEMP debug

  scale = state->video.scale;

  if (full_screen)
  {
    SDL_FRect rect;

    rect.x = (float) x;
    rect.y = (float) y;
    rect.w = (float) (GAMEWIDTH  * scale);
    rect.h = (float) (GAMEHEIGHT * scale);

    SDL_SetRenderDrawColor(state->video.renderer, 0xFF, 0x00, 0x00, 0xFF); // red: full-screen refresh
    chq_render_thick_rect(state->video.renderer, &rect);
  }

  SDL_SetRenderDrawColor(state->video.renderer, 0x00, 0xFF, 0x00, 0xFF); // green: partial refresh
  for (i = 0; i < count; i++)
  {
    const zxbox_t *box = &rects[i];
    SDL_FRect      rect;

    // box is bottom-left origin; flip to the window's top-down space.
    rect.x = (float) (x + box->x0 * scale);
    rect.y = (float) (y + (GAMEHEIGHT - box->y1) * scale);
    rect.w = (float) ((box->x1 - box->x0) * scale);
    rect.h = (float) ((box->y1 - box->y0) * scale);

    chq_render_thick_rect(state->video.renderer, &rect);
  }
}

// type: em_arg_callback_func
static void chq_sdl_main_loop(void *opaque)
{
  chq_sdl_state_t *state = opaque;
  int              x, y, w, h; // destination rect: game view within the window
  int              ww, wh;     // actual window size (may exceed scale*game size in fullscreen)
  SDL_Event        event;

  w = GAMEWIDTH  * state->video.scale;
  h = GAMEHEIGHT * state->video.scale;
  SDL_GetWindowSize(state->video.window, &ww, &wh);
  x = (ww - w) / 2; // centred; equals BORDER*scale in windowed mode, letterboxes in fullscreen
  y = (wh - h) / 2;

  // Consume all pending events
  while (SDL_PollEvent(&event))
  {
    switch (event.type)
    {
    case SDL_EVENT_QUIT:
      CHQ_FLAG_SET(state, CHQ_FLAG_QUIT);
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

  if (CHQ_FLAG_TEST(state, CHQ_FLAG_QUIT))
    return;

  if (state->video.crt_enabled)
  {
    chq_CRT_shader_render(&state->video.crt, state->video.window, state->zx,
                          x, y, w, h, GAMEWIDTH, GAMEHEIGHT,
                          &state->video.crt_params);
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
    SDL_UpdateTexture(state->video.texture, NULL, pixels, GAMEWIDTH * 4);
    zxspectrum_release_screen(state->zx);

    /* Clear screen */
    // TODO: This ought to be the border colour, but CHQ's is always black.
    SDL_SetRenderDrawColor(state->video.renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(state->video.renderer);

    /* Offset the image */
    // Note that this will inhibit image stretching.

    SDL_RenderTexture(state->video.renderer, state->video.texture, NULL, &dstrect);
    chq_draw_dirty_overlay(state, x, y);
    SDL_RenderPresent(state->video.renderer);
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
    if (strcmp(argv[arg], "-48k") == 0) {
      mode_128k = 0;
    }
    else if (strcmp(argv[arg], "-128k") == 0) {
      mode_128k = 1;
    }
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
  state.flags     = 0;
  CHQ_FLAG_ASSIGN(&state, CHQ_FLAG_MODE_128K, mode_128k);
  state.video.scale           = SCALE_DEFAULT;
  state.speed                 = SPEED_DEFAULT;
  state.audio.volume          = VOLUME_DEFAULT;
  state.video.crt_params      = crt_default_params;
  state.video.crt_param_index = 0;

#ifdef __APPLE__
  /* Conv: disable macOS press-and-hold accent popover so held keys repeat
   * instead of opening the accent picker.
   */
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
                            chq_window_width(state.video.scale),
                            chq_window_height(state.video.scale),
                            0);
  if (window == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateWindow: %s\n", SDL_GetError());
    goto failure;
  }

  state.video.window = window;

  chq_update_window_title(&state);

  /* The GPU texture is always SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM (R in the
   * lowest memory byte), which is what Screen.c's palette_abgr table packs
   * when bgr_pixels is true - despite the name, "bgr_pixels" selects which
   * palette table to use, not which byte order it produces. See the
   * 0x00RRGGBB/0x00BBGGRR comments in Screen.c.
   *
   * Conv: this is fixed rather than matched to the renderer's preferred
   * format, as it was when the backend was chosen at compile time. The
   * backend can now change at any keypress but the conversion palette is
   * picked once, so both backends have to agree; the CRT one cannot bend, so
   * the plain renderer takes an ABGR8888 texture to suit (see
   * chq_renderer_create).
   */
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

  state.audio.ay = slopay_chip_create(AY_CLOCK_FREQ, AY_SAMPLE_RATE);
  if (state.audio.ay == NULL)
    goto failure;

  state.audio.queue_mutex = SDL_CreateMutex();
  if (state.audio.queue_mutex == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateMutex: %s\n", SDL_GetError());
    goto failure;
  }

  state.video.dirty_mutex = SDL_CreateMutex();
  if (state.video.dirty_mutex == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateMutex: %s\n", SDL_GetError());
    goto failure;
  }

  /* Conv: force mono output. The chip defaults to ABC stereo separation
   * (left = A+B, right = B+C), which sounds right-heavy or left-heavy
   * depending on which channels a given tune favours; we don't know whether
   * this game's music assumes ABC, ACB, or no separation at all, so mono
   * sidesteps the question. Volume is turned down from the chip's own
   * default (10%) as three channels plus envelope can otherwise clip loud.
   */
  slopay_chip_set_stereo_mode(state.audio.ay, SLOPAY_CHIP_STEREO_MODE_MONO);
  slopay_chip_set_volume(state.audio.ay, AY_VOLUME_PCT);

  {
    SDL_AudioSpec desired = {0};

    desired.freq     = AY_SAMPLE_RATE;
    desired.format   = SDL_AUDIO_S16;
    desired.channels = 2;

    state.audio.stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK,
                                                   &desired,
                                                   &chq_audio_callback,
                                                   &state);
    if (state.audio.stream == NULL)
    {
      fprintf(stderr, "Error: SDL_OpenAudioDeviceStream: %s\n", SDL_GetError());
      goto failure;
    }

    SDL_ResumeAudioStreamDevice(state.audio.stream);
  }

  /* Bring up the starting backend. A CRT request that cannot be met falls
   * back to the plain renderer rather than failing to start; only losing
   * both is fatal.
   */
  chq_set_crt_enabled(&state, CHQ_CRT_SHADER);
  if (!state.video.crt_enabled && state.video.renderer == NULL)
    goto failure;

  printf("CRT shader: %s (F4 toggles)\n", state.video.crt_enabled ? "on" : "off");

  state.game = chq_create(state.zx);
  if (state.game == NULL)
    goto failure;

  state.game_thread = SDL_CreateThread(chq_game_thread, "game", &state);
  if (state.game_thread == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateThread: %s\n", SDL_GetError());
    goto failure;
  }

  while (!CHQ_FLAG_TEST(&state, CHQ_FLAG_QUIT))
    chq_sdl_main_loop(&state);

  // Stop the audio device before anything else. chq_stop only signals the
  // game thread, which longjmps out mid-frame and leaves the AY holding
  // whatever tone the tune was playing; without this the callback keeps
  // sounding that tone for the whole shutdown wait below. Pause before
  // clear, or the callback refills between the two.
  SDL_PauseAudioStreamDevice(state.audio.stream);
  SDL_ClearAudioStream(state.audio.stream);

  chq_stop(state.game);

  SDL_WaitThread(state.game_thread, NULL);

  chq_destroy(state.game);
  zxspectrum_destroy(state.zx);

  SDL_DestroyAudioStream(state.audio.stream);
  slopay_chip_destroy(state.audio.ay);
  SDL_DestroyMutex(state.audio.queue_mutex);
  SDL_DestroyMutex(state.video.dirty_mutex);

  if (state.video.crt_enabled)
    chq_CRT_shader_destroy(&state.video.crt, window);
  else
    chq_renderer_destroy(&state);
  SDL_DestroyWindow(window);

  SDL_Quit();

  exit(EXIT_SUCCESS);


failure:

  exit(EXIT_FAILURE);
}
