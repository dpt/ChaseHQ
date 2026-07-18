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

// -----------------------------------------------------------------------------

// Configuration
//
#define FPS                 (15)
#define GAMEWIDTH          (256)
#define GAMEHEIGHT         (192)
#define BORDER              (32)

#define SCALE_DEFAULT        (4)
#define SCALE_MIN            (1)
#define SCALE_MAX            (4)

// Set to 0 to fall back to the plain SDL_Renderer blit (no shader, any GPU
// backend, nearest-neighbour scaling); set to 1 for the SDL3 GPU/Metal CRT
// post-effect pipeline (Metal only). Override with -DCHQ_CRT_SHADER=1.
#ifndef CHQ_CRT_SHADER
#define CHQ_CRT_SHADER       (0)
#endif

#define MAXSTAMPS            (4) // max depth of timestamps stack

#define AY_CLOCK_FREQ  (1773400) // ZX Spectrum 128K AY-3-8912 clock rate
#define AY_SAMPLE_RATE   (44100)
#define AY_VOLUME_PCT       (5) // 0..AY_MASTER_VOLUME_MAX

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

#if CHQ_CRT_SHADER

// CRT post-effect prototype. Runs the game texture through a Metal fragment
// shader via SDL's GPU API instead of the plain SDL_Renderer blit, so we can
// add scanlines/vignette. Fullscreen triangle is generated in the vertex
// shader from vertex_id, so no vertex buffer is needed.
//
// ponytail: MSL source is handed to SDL_CreateGPUShader() as
// SDL_GPU_SHADERFORMAT_MSL and compiled at runtime by Metal; no offline
// .metallib build step. This is macOS/iOS-only (Metal backend only) -
// fine for a prototype, would need SPIR-V/DXIL variants to run elsewhere.
static const char *const chq_crt_vertex_msl =
  "#include <metal_stdlib>\n"
  "using namespace metal;\n"
  "struct VSOut { float4 position [[position]]; float2 uv; };\n"
  "vertex VSOut vs_main(uint vid [[vertex_id]]) {\n"
  "  float2 pos[3] = { float2(-1,-1), float2(3,-1), float2(-1,3) };\n"
  "  float2 uv[3]  = { float2(0,1),  float2(2,1),  float2(0,-1) };\n"
  "  VSOut out;\n"
  "  out.position = float4(pos[vid], 0.0, 1.0);\n"
  "  out.uv = uv[vid];\n"
  "  return out;\n"
  "}\n";

// Port of a reference Three.js/GLSL CRTShader. Its uniforms (curvature,
// bloom threshold/intensity, brightness/contrast/saturation, scanline
// count/intensity, adaptive intensity, vignette strength) are hardcoded
// below rather than plumbed through as tunable uniforms - none of them
// change at runtime in this game; the constants have been re-tuned by eye
// against this game's screen and no longer match the reference's own
// defaults.
// ponytail: the reference's rgbShift channel-separation effect and its
// time-driven flicker are dropped - rgbShift defaults to 0.0 (a no-op in
// the source shader too), and flicker needs no game logic to justify the
// per-frame uniform push it would otherwise require.
static const char *const chq_crt_fragment_msl =
  "#include <metal_stdlib>\n"
  "using namespace metal;\n"
  "struct VSOut { float4 position [[position]]; float2 uv; };\n"
  "fragment float4 fs_main(VSOut in [[stage_in]],\n"
  "                         texture2d<float> tex [[texture(0)]],\n"
  "                         sampler samp [[sampler(0)]]) {\n"
  // curveRemapUV: barrel distortion via dot(coord,coord) radial distance.
  "  float2 coord = in.uv * 2.0 - 1.0;\n"
  "  coord *= 1.0 + dot(coord, coord) * 0.025;\n"
  "  float2 uv = coord * 0.5 + 0.5;\n"
  "  if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0)\n"
  "    return float4(0.0, 0.0, 0.0, 1.0);\n"
  "  float4 c = tex.sample(samp, uv);\n"
  // sampleBloom: threshold-gated centre + 4-tap cross sample.
  "  float2 texel = float2(1.0 / 256.0, 1.0 / 192.0);\n"
  "  float3 bloom = float3(0.0);\n"
  "  float3 bc = c.rgb;\n"
  "  float3 bn = tex.sample(samp, uv + float2(0.0, texel.y)).rgb;\n"
  "  float3 bs = tex.sample(samp, uv - float2(0.0, texel.y)).rgb;\n"
  "  float3 be = tex.sample(samp, uv + float2(texel.x, 0.0)).rgb;\n"
  "  float3 bw = tex.sample(samp, uv - float2(texel.x, 0.0)).rgb;\n"
  "  if (max(bc.r, max(bc.g, bc.b)) > 0.5) bloom += bc;\n"
  "  if (max(bn.r, max(bn.g, bn.b)) > 0.5) bloom += bn;\n"
  "  if (max(bs.r, max(bs.g, bs.b)) > 0.5) bloom += bs;\n"
  "  if (max(be.r, max(be.g, be.b)) > 0.5) bloom += be;\n"
  "  if (max(bw.r, max(bw.g, bw.b)) > 0.5) bloom += bw;\n"
  "  c.rgb += bloom * 0.025;\n"
  // brightness / contrast / saturation.
  "  c.rgb = (c.rgb - 0.5) * 1.1 + 0.5;\n"
  "  c.rgb *= 1.1;\n"
  "  float lum = dot(c.rgb, float3(0.299, 0.587, 0.114));\n"
  "  c.rgb = mix(float3(lum), c.rgb, 1.0);\n"
  // scanlines, intensity adapted to local luminance.
  "  float scan = sin(uv.y * 384.0 * 3.14159265) * 0.5 + 0.5;\n"
  "  float adaptive = mix(0.75, 0.75 * (1.0 - lum), 0.5);\n"
  "  c.rgb *= 1.0 - adaptive * scan;\n"
  // vignetteApprox: Chebyshev (max-component) distance falloff.
  "  float2 d = abs(uv - 0.5) * 2.0;\n"
  "  float vignette = 1.0 - max(d.x, d.y) * max(d.x, d.y) * 0.3;\n"
  "  c.rgb *= vignette;\n"
  "  return c;\n"
  "}\n";

#endif // CHQ_CRT_SHADER

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

  int                quit; // bool
  int                paused; // bool
  // int                  menu; // bool

  int                scale; // window/render scale, SCALE_MIN..SCALE_MAX

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

  SDL_Window              *window;
#if CHQ_CRT_SHADER
  SDL_GPUDevice           *gpu;
  SDL_GPUTexture          *gpu_texture; // holds the game's converted screen
  SDL_GPUTransferBuffer   *transfer_buffer; // staging buffer for the upload above
  SDL_GPUSampler          *sampler;
  SDL_GPUGraphicsPipeline *pipeline;
#else
  SDL_Renderer            *renderer;
  SDL_Texture             *texture;
#endif
  SDL_Thread              *game_thread;
  SDL_AudioStream         *audio_stream;
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

  (void) total_amount;

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
      left   = (int16_t) (sample & 0xFFFF)         + beeper;
      right  = (int16_t) ((sample >> 16) & 0xFFFF) + beeper;
      if (state->audio_muted)
        left = right = 0;
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

  chq_setup(state->game);
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

  sym = k->key;

  if (sym == SDLK_F1)
  {
    if (k->down && !k->repeat)
      state->paused = !state->paused;
    return;
  }

  if (sym == SDLK_F2)
  {
    if (k->down && !k->repeat)
      state->audio_muted = !state->audio_muted;
    return;
  }

  if (sym == SDLK_MINUS || sym == SDLK_EQUALS)
  {
    if (k->down && !k->repeat)
    {
      int scale;

      scale = state->scale + (sym == SDLK_MINUS ? -1 : 1);
      if (scale < SCALE_MIN)
        scale = SCALE_MIN;
      if (scale > SCALE_MAX)
        scale = SCALE_MAX;

      if (scale != state->scale)
      {
        state->scale = scale;
        SDL_SetWindowSize(state->window,
                          chq_window_width(scale),
                          chq_window_height(scale));
      }
    }
    return;
  }

  switch (sym)
  {
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

// type: em_arg_callback_func
static void chq_sdl_main_loop(void *opaque)
{
  chq_sdl_state_t *state = opaque;
#if CHQ_CRT_SHADER
  SDL_GPUViewport   viewport;

  viewport.x         = BORDER     * state->scale;
  viewport.y         = BORDER     * state->scale;
  viewport.w         = GAMEWIDTH  * state->scale;
  viewport.h         = GAMEHEIGHT * state->scale;
  viewport.min_depth = 0.0f;
  viewport.max_depth = 1.0f;
#else
  SDL_FRect         dstrect;

  dstrect.x = BORDER     * state->scale;
  dstrect.y = BORDER     * state->scale;
  dstrect.w = GAMEWIDTH  * state->scale;
  dstrect.h = GAMEHEIGHT * state->scale;
#endif

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

#if CHQ_CRT_SHADER
    /* Upload the game's converted screen buffer to the GPU texture. */
    {
      uint32_t              *pixels;
      void                   *mapped;
      SDL_GPUCommandBuffer   *upload_cmdbuf;
      SDL_GPUCopyPass        *copy_pass;
      SDL_GPUTextureTransferInfo src;
      SDL_GPUTextureRegion    dst;

      pixels = zxspectrum_claim_screen(state->zx);
      mapped = SDL_MapGPUTransferBuffer(state->gpu, state->transfer_buffer, true);
      memcpy(mapped, pixels, GAMEWIDTH * GAMEHEIGHT * 4);
      SDL_UnmapGPUTransferBuffer(state->gpu, state->transfer_buffer);
      zxspectrum_release_screen(state->zx);

      memset(&src, 0, sizeof(src));
      src.transfer_buffer = state->transfer_buffer;
      src.pixels_per_row   = GAMEWIDTH;
      src.rows_per_layer   = GAMEHEIGHT;

      memset(&dst, 0, sizeof(dst));
      dst.texture = state->gpu_texture;
      dst.w       = GAMEWIDTH;
      dst.h       = GAMEHEIGHT;
      dst.d       = 1;

      upload_cmdbuf = SDL_AcquireGPUCommandBuffer(state->gpu);
      copy_pass     = SDL_BeginGPUCopyPass(upload_cmdbuf);
      SDL_UploadToGPUTexture(copy_pass, &src, &dst, true);
      SDL_EndGPUCopyPass(copy_pass);

      /* Render the uploaded texture through the CRT shader pipeline. */
      {
        SDL_GPUTexture           *swapchain_texture;
        SDL_GPUColorTargetInfo    color_target;
        SDL_GPURenderPass        *render_pass;
        SDL_GPUTextureSamplerBinding tex_binding;

        if (SDL_WaitAndAcquireGPUSwapchainTexture(upload_cmdbuf, state->window,
                                                  &swapchain_texture, NULL, NULL) &&
            swapchain_texture != NULL)
        {
          memset(&color_target, 0, sizeof(color_target));
          color_target.texture     = swapchain_texture;
          color_target.load_op     = SDL_GPU_LOADOP_CLEAR;
          color_target.store_op    = SDL_GPU_STOREOP_STORE;
          color_target.clear_color.r = 0.0f;
          color_target.clear_color.g = 0.0f;
          color_target.clear_color.b = 0.0f;
          color_target.clear_color.a = 1.0f;

          render_pass = SDL_BeginGPURenderPass(upload_cmdbuf, &color_target, 1, NULL);

          SDL_BindGPUGraphicsPipeline(render_pass, state->pipeline);
          SDL_SetGPUViewport(render_pass, &viewport);

          tex_binding.texture = state->gpu_texture;
          tex_binding.sampler = state->sampler;
          SDL_BindGPUFragmentSamplers(render_pass, 0, &tex_binding, 1);

          SDL_DrawGPUPrimitives(render_pass, 3, 1, 0, 0);

          SDL_EndGPURenderPass(render_pass);
        }
      }

      SDL_SubmitGPUCommandBuffer(upload_cmdbuf);
    }
#else
    /* Update the texture from the game's converted screen buffer. */
    {
      uint32_t *pixels;

      pixels = zxspectrum_claim_screen(state->zx);
      SDL_UpdateTexture(state->texture, NULL, pixels, GAMEWIDTH * 4);
      zxspectrum_release_screen(state->zx);
    }

    /* Clear screen */
    // TODO: This ought to be the border colour, but CHQ's is always black.
    SDL_SetRenderDrawColor(state->renderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(state->renderer);

    /* Offset the image */
    // Note that this will inhibit image stretching.

    SDL_RenderTexture(state->renderer, state->texture, NULL, &dstrect);
    SDL_RenderPresent(state->renderer);
#endif

    SDL_Delay(1000 / FPS);
  }
}

int main(void)
{
  chq_sdl_state_t         state;
  zxconfig_t              zxconfig;
  SDL_Window             *window;
#if !CHQ_CRT_SHADER
  SDL_PropertiesID        renderer_props;
  const SDL_PixelFormat  *texture_formats;
  SDL_PixelFormat         native_fmt;
  Uint32                  Rmask, Gmask, Bmask, Amask;
  int                     bpp;
#endif

  printf("CHASE H.Q.\n");
  printf("==========\n");

  printf("Initialising...\n");

  memset(&state, 0, sizeof(state));

  zxkeyset_clear(&state.keys);
  state.kempston  = 0;
  state.paused    = 0;
  state.quit      = 0;
  state.scale     = SCALE_DEFAULT;
  // state.menu      = 1;

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

#if CHQ_CRT_SHADER
  state.gpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_MSL, false, NULL);
  if (state.gpu == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateGPUDevice: %s\n", SDL_GetError());
    goto failure;
  }

  if (!SDL_ClaimWindowForGPUDevice(state.gpu, window))
  {
    fprintf(stderr, "Error: SDL_ClaimWindowForGPUDevice: %s\n", SDL_GetError());
    goto failure;
  }
#else
  state.renderer = SDL_CreateRenderer(window, NULL);
  if (state.renderer == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateRenderer: %s\n", SDL_GetError());
    goto failure;
  }

  SDL_SetRenderVSync(state.renderer, 1);

  renderer_props  = SDL_GetRendererProperties(state.renderer);
  texture_formats = SDL_GetPointerProperty(renderer_props,
                                           SDL_PROP_RENDERER_TEXTURE_FORMATS_POINTER,
                                           NULL);
  native_fmt = (texture_formats != NULL) ? texture_formats[0]
                                          : SDL_PIXELFORMAT_RGBA8888;
  SDL_GetMasksForPixelFormat(native_fmt, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
#endif

  // The GPU texture is always SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM (R in the
  // lowest memory byte), which is what Screen.c's palette_abgr table packs
  // when bgr_pixels is true - despite the name, "bgr_pixels" selects which
  // palette table to use, not which byte order it produces. See the
  // 0x00RRGGBB/0x00BBGGRR comments in Screen.c.
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
#if CHQ_CRT_SHADER
  zxconfig.bgr_pixels = true;
#else
  zxconfig.bgr_pixels = (Rmask < Bmask); /* R in lower byte = BGR format */
#endif

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

#if CHQ_CRT_SHADER
  {
    SDL_GPUTextureCreateInfo         texture_info;
    SDL_GPUTransferBufferCreateInfo  transfer_info;
    SDL_GPUSamplerCreateInfo         sampler_info;
    SDL_GPUShaderCreateInfo          shader_info;
    SDL_GPUColorTargetDescription    color_target_desc;
    SDL_GPUGraphicsPipelineCreateInfo pipeline_info;
    SDL_GPUShader                    *vertex_shader;
    SDL_GPUShader                    *fragment_shader;

    memset(&texture_info, 0, sizeof(texture_info));
    texture_info.type   = SDL_GPU_TEXTURETYPE_2D;
    texture_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    texture_info.usage  = SDL_GPU_TEXTUREUSAGE_SAMPLER;
    texture_info.width  = GAMEWIDTH;
    texture_info.height = GAMEHEIGHT;
    texture_info.layer_count_or_depth = 1;
    texture_info.num_levels           = 1;
    texture_info.sample_count         = SDL_GPU_SAMPLECOUNT_1;

    state.gpu_texture = SDL_CreateGPUTexture(state.gpu, &texture_info);
    if (state.gpu_texture == NULL)
    {
      fprintf(stderr, "Error: SDL_CreateGPUTexture: %s\n", SDL_GetError());
      goto failure;
    }

    memset(&transfer_info, 0, sizeof(transfer_info));
    transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transfer_info.size  = GAMEWIDTH * GAMEHEIGHT * 4;

    state.transfer_buffer = SDL_CreateGPUTransferBuffer(state.gpu, &transfer_info);
    if (state.transfer_buffer == NULL)
    {
      fprintf(stderr, "Error: SDL_CreateGPUTransferBuffer: %s\n", SDL_GetError());
      goto failure;
    }

    // Linear filtering: the CRT shader (curvature, bloom, scanlines) reads
    // this softer look as part of the effect rather than the crisp pixels
    // nearest-neighbour gave the plain blit.
    memset(&sampler_info, 0, sizeof(sampler_info));
    sampler_info.min_filter     = SDL_GPU_FILTER_LINEAR;
    sampler_info.mag_filter     = SDL_GPU_FILTER_LINEAR;
    sampler_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    sampler_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

    state.sampler = SDL_CreateGPUSampler(state.gpu, &sampler_info);
    if (state.sampler == NULL)
    {
      fprintf(stderr, "Error: SDL_CreateGPUSampler: %s\n", SDL_GetError());
      goto failure;
    }

    memset(&shader_info, 0, sizeof(shader_info));
    shader_info.code       = (const Uint8 *) chq_crt_vertex_msl;
    shader_info.code_size  = strlen(chq_crt_vertex_msl);
    shader_info.entrypoint = "vs_main";
    shader_info.format     = SDL_GPU_SHADERFORMAT_MSL;
    shader_info.stage      = SDL_GPU_SHADERSTAGE_VERTEX;

    vertex_shader = SDL_CreateGPUShader(state.gpu, &shader_info);
    if (vertex_shader == NULL)
    {
      fprintf(stderr, "Error: SDL_CreateGPUShader (vertex): %s\n", SDL_GetError());
      goto failure;
    }

    memset(&shader_info, 0, sizeof(shader_info));
    shader_info.code         = (const Uint8 *) chq_crt_fragment_msl;
    shader_info.code_size    = strlen(chq_crt_fragment_msl);
    shader_info.entrypoint   = "fs_main";
    shader_info.format       = SDL_GPU_SHADERFORMAT_MSL;
    shader_info.stage        = SDL_GPU_SHADERSTAGE_FRAGMENT;
    shader_info.num_samplers = 1;

    fragment_shader = SDL_CreateGPUShader(state.gpu, &shader_info);
    if (fragment_shader == NULL)
    {
      fprintf(stderr, "Error: SDL_CreateGPUShader (fragment): %s\n", SDL_GetError());
      goto failure;
    }

    memset(&color_target_desc, 0, sizeof(color_target_desc));
    color_target_desc.format = SDL_GetGPUSwapchainTextureFormat(state.gpu, window);

    memset(&pipeline_info, 0, sizeof(pipeline_info));
    pipeline_info.vertex_shader   = vertex_shader;
    pipeline_info.fragment_shader = fragment_shader;
    pipeline_info.primitive_type  = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    pipeline_info.target_info.color_target_descriptions = &color_target_desc;
    pipeline_info.target_info.num_color_targets          = 1;

    state.pipeline = SDL_CreateGPUGraphicsPipeline(state.gpu, &pipeline_info);

    // Conv: pipelines don't retain the shader modules internally past this
    // call, so these can be released immediately rather than kept alive
    // for the lifetime of the app.
    SDL_ReleaseGPUShader(state.gpu, vertex_shader);
    SDL_ReleaseGPUShader(state.gpu, fragment_shader);

    if (state.pipeline == NULL)
    {
      fprintf(stderr, "Error: SDL_CreateGPUGraphicsPipeline: %s\n", SDL_GetError());
      goto failure;
    }
  }
#else
  state.texture = SDL_CreateTexture(state.renderer,
                                    native_fmt,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    GAMEWIDTH, GAMEHEIGHT);
  if (state.texture == NULL)
  {
    fprintf(stderr, "Error: SDL_CreateTexture: %s\n", SDL_GetError());
    goto failure;
  }

  if (!SDL_SetTextureBlendMode(state.texture, SDL_BLENDMODE_NONE))
  {
    fprintf(stderr, "Error: SDL_SetTextureBlendMode: %s\n", SDL_GetError());
    goto failure;
  }

  // Conv: nearest-neighbour keeps ZX Spectrum pixels crisp when the window
  // is scaled up; SDL3's default is linear, which blurs them.
  SDL_SetTextureScaleMode(state.texture, SDL_SCALEMODE_NEAREST);
#endif

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

#if CHQ_CRT_SHADER
  SDL_ReleaseGPUGraphicsPipeline(state.gpu, state.pipeline);
  SDL_ReleaseGPUSampler(state.gpu, state.sampler);
  SDL_ReleaseGPUTransferBuffer(state.gpu, state.transfer_buffer);
  SDL_ReleaseGPUTexture(state.gpu, state.gpu_texture);
  SDL_ReleaseWindowFromGPUDevice(state.gpu, window);
  SDL_DestroyGPUDevice(state.gpu);
#else
  SDL_DestroyTexture(state.texture);
  SDL_DestroyRenderer(state.renderer);
#endif
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
