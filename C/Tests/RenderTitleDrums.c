/* RenderTitleDrums.c
 *
 * Offline, deterministic render of the 128K title tune's beeper drums.
 *
 * The running app times drum hits through two independent clocks: wall-clock
 * frame pacing (stamp/sleep in the SDL host) and the virtual T-state clock that
 * timestamps beeper edges. Uneven ("galloping") drum timing can come from
 * either. This driver removes the first: stamp/sleep do nothing, so the only
 * clock left is the virtual one the engine itself drives. If the onsets it
 * reports are even, the engine's timeline is right and any remaining gallop is
 * host-side; if they are uneven, the bug is in the engine.
 *
 * It writes a WAV rendered from that virtual timeline (so the result can be
 * listened to, or onset-detected alongside a real-hardware recording), and
 * prints the rhythm the drums were struck in and the per-frame bit-bang
 * occupancy behind it.
 *
 * Reference figures, all measured from an instruction-level trace of the real
 * game (traces/big-trace.log, 8096 title-screen frames):
 *
 *   frame period                70908 T-states
 *   bit-bang per playing frame  median 98 rows = 784 OUTs = 62524 T-states
 *   tune 0 strike intervals     1 7 8 8 1 7 8 1 7 8 1 7 8 1 7 8 8 1 7 1 7 8 ...
 *
 * Usage: RenderTitleDrums [tune] [frames] [out.wav]
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "C99/Types.h"
#include "ZXSpectrum/Macros.h"
#include "ZXSpectrum/Spectrum.h"

#include "ChaseHQ/ChaseHQ.h"
#include "ChaseHQ/Engine/Bank3State.h"
#include "ChaseHQ/Engine/State.h"
#include "ChaseHQ/Engine/Tests.h"

/* 128K CPU clock; one interrupt period is 70908 T-states at 50.021 Hz. */
#define CPU_HZ         (3546900)
#define FRAME_TSTATES  (70908)
#define SAMPLE_RATE    (44100)

/* Edges closer together than this are the same drum hit. One frame's worth of
 * bit-banging never pauses this long; the gap between hits always exceeds it. */
#define BURST_GAP_TSTATES (3000)

#define MAX_EDGES  (4 * 1024 * 1024)
#define MAX_FRAMES (16 * 1024)

typedef struct edge
{
  uint64_t tstates;
  int      level;
}
edge_t;

static edge_t   g_edges[MAX_EDGES];
static int      g_nedges;
static uint64_t g_tstates;      /* the virtual clock, as the facade drives it */
static uint64_t g_frame_start;  /* virtual clock at the current frame's start */
static uint64_t g_worst_frame;  /* most bit-bang time any one frame emitted */

static int      g_struck[MAX_FRAMES]; /* frames a drum was dispatched on */
static int      nstruck;

static uint64_t g_frame_end[MAX_FRAMES]; /* virtual clock at each frame's end */
static int      g_frame_edges[MAX_FRAMES];
static int      g_nframes;

/* ----------------------------------------------------------------------- */
/* Fake ZX Spectrum facade: virtual clock only, no wall clock at all        */
/* ----------------------------------------------------------------------- */

static uint8_t fake_in(zxspectrum_t *s, uint16_t addr)
{
  NOT_USED(s); NOT_USED(addr);
  return 0xFF; /* all keys unpressed */
}

/* Records every speaker OUT, not just level changes: a drum sample OUTs once
 * per bit whatever the bit's value, so the OUT rate is what marks a hit's
 * extent. Recording only the edges would leave gaps inside a hit wherever the
 * sample data has a run of equal bits, and the onset grouping below would then
 * split one hit into several. */
static void fake_out(zxspectrum_t *s, uint16_t addr, uint8_t byte)
{
  NOT_USED(s);

  if (addr != port_BORDER_EAR_MIC)
    return;

  /* Mirrors zx_out in libraries/ZXSpectrum/Spectrum.c: the OUT instruction
   * itself costs 11 T-states, billed here rather than by logtime. */
  g_tstates += 11;

  assert(g_nedges < MAX_EDGES);
  g_edges[g_nedges].tstates = g_tstates;
  g_edges[g_nedges].level   = (byte & port_MASK_EAR) != 0;
  g_nedges++;
}

static void fake_draw(zxspectrum_t *s, const zxbox_t *dirty)
{
  NOT_USED(s); NOT_USED(dirty);
}

static void fake_stamp(zxspectrum_t *s)
{
  NOT_USED(s);
}

static int fake_sleep(zxspectrum_t *s, int ticks)
{
  NOT_USED(s); NOT_USED(ticks);
  return 0;
}

static void fake_logtime(zxspectrum_t *s, int duration)
{
  NOT_USED(s);
  g_tstates += duration;
}

static zxspectrum_t g_speccy;

static void speccy_init(void)
{
  memset(&g_speccy, 0, sizeof(g_speccy));
  g_speccy.in            = fake_in;
  g_speccy.out           = fake_out;
  g_speccy.draw          = fake_draw;
  g_speccy.stamp         = fake_stamp;
  g_speccy.sleep         = fake_sleep;
  g_speccy.logtime       = fake_logtime;
  g_speccy.screen.width  = SCREEN_WIDTH;
  g_speccy.screen.height = SCREEN_HEIGHT;
}

/* ----------------------------------------------------------------------- */
/* WAV output                                                               */
/* ----------------------------------------------------------------------- */

static void put32(FILE *f, uint32_t v)
{
  fputc(v & 0xFF, f);
  fputc((v >> 8) & 0xFF, f);
  fputc((v >> 16) & 0xFF, f);
  fputc((v >> 24) & 0xFF, f);
}

static void put16(FILE *f, uint16_t v)
{
  fputc(v & 0xFF, f);
  fputc((v >> 8) & 0xFF, f);
}

/**
 * Render the recorded edge list to a mono 16-bit WAV on the virtual clock.
 *
 * Each output sample takes the speaker level in force at that instant. This is
 * a nearest-edge sampler, not a band-limited one: the point is onset timing,
 * not fidelity.
 */
static void write_wav(const char *path)
{
  FILE    *f;
  uint64_t total_tstates;
  uint32_t nsamples;
  uint32_t i;
  int      e;
  int      level;
  uint64_t sample_tstates;

  if (g_nedges == 0) {
    printf("no beeper edges recorded, no WAV written\n");
    return;
  }

  total_tstates = g_edges[g_nedges - 1].tstates + FRAME_TSTATES;
  nsamples      = (uint32_t) (total_tstates * SAMPLE_RATE / CPU_HZ);

  f = fopen(path, "wb");
  if (f == NULL) {
    printf("cannot write %s\n", path);
    return;
  }

  fwrite("RIFF", 1, 4, f);
  put32(f, 36 + nsamples * 2);
  fwrite("WAVEfmt ", 1, 8, f);
  put32(f, 16);              /* fmt chunk size */
  put16(f, 1);               /* PCM */
  put16(f, 1);               /* mono */
  put32(f, SAMPLE_RATE);
  put32(f, SAMPLE_RATE * 2); /* byte rate */
  put16(f, 2);               /* block align */
  put16(f, 16);              /* bits per sample */
  fwrite("data", 1, 4, f);
  put32(f, nsamples * 2);

  e     = 0;
  level = 0;
  for (i = 0; i < nsamples; i++) {
    sample_tstates = (uint64_t) i * CPU_HZ / SAMPLE_RATE;
    while (e < g_nedges && g_edges[e].tstates <= sample_tstates) {
      level = g_edges[e].level;
      e++;
    }
    put16(f, (uint16_t) (int16_t) (level ? 12000 : -12000));
  }

  fclose(f);
  printf("wrote %s (%u samples, %.2f s)\n", path, nsamples,
         (double) nsamples / SAMPLE_RATE);
}

/* ----------------------------------------------------------------------- */
/* Analysis                                                                 */
/* ----------------------------------------------------------------------- */

/**
 * Report bit-bang occupancy per playing frame, against the 62524 T-states a
 * real frame leaves after the music tick.
 */
static void report_frames(void)
{
  int      frame;
  int      playing;
  uint64_t busiest;
  uint64_t total;
  uint64_t span;
  int      prev_edge;
  int      edges;

  playing   = 0;
  busiest   = 0;
  total     = 0;
  prev_edge = 0;

  for (frame = 0; frame < g_nframes; frame++) {
    edges = g_frame_edges[frame] - prev_edge;
    if (edges > 0) {
      /* Bit-bang time in this frame: from the first edge of the frame to the
       * frame's end on the virtual clock. Each OUT is one bit of a row. */
      span = g_edges[g_frame_edges[frame] - 1].tstates -
             g_edges[prev_edge].tstates;
      playing++;
      total += span;
      if (span > busiest)
        busiest = span;
    }
    prev_edge = g_frame_edges[frame];
  }

  printf("frames run          %d\n", g_nframes);
  printf("frames with drums   %d\n", playing);
  if (playing > 0) {
    printf("bit-bang per playing frame: mean %llu T, max %llu T"
           " (real hardware: 62524 T, one frame = %d T)\n",
           (unsigned long long) (total / playing),
           (unsigned long long) busiest, FRAME_TSTATES);
  }
  printf("most a single frame emitted: %llu T\n",
         (unsigned long long) g_worst_frame);
}

/**
 * Report where the audible bursts of beeper output start, and by how far they
 * miss a whole number of frames.
 *
 * These are bursts, not hits: a sample too long for one frame's budget resumes
 * on the following frame, so a single drum shows up here as two or three
 * bursts one frame apart. Real hardware does the same (its trace shows 1004
 * mid-sample yields against 858 completions), so a burst one frame after the
 * last is expected, not a fault. What must not appear is a fractional
 * interval: the engine's beeper timeline is built from whole frames, so
 * anything off the grid means audio time is being emitted at the wrong rate.
 * Use report_strikes for the rhythm itself.
 */
static void report_bursts(void)
{
  int      i;
  int      nbursts;
  uint64_t prev;
  double   interval;
  double   off;
  double   worst;

  nbursts = 0;
  prev    = 0;
  worst   = 0.0;

  for (i = 0; i < g_nedges; i++) {
    if (i > 0 &&
        g_edges[i].tstates - g_edges[i - 1].tstates <= BURST_GAP_TSTATES)
      continue;

    if (nbursts > 0) {
      interval = (double) (g_edges[i].tstates - prev) / FRAME_TSTATES;
      off      = interval - (int) (interval + 0.5);
      if (off < 0.0)
        off = -off;
      if (off > worst)
        worst = off;
    }
    prev = g_edges[i].tstates;
    nbursts++;
  }

  printf("beeper bursts       %d\n", nbursts);
  printf("worst deviation from a whole frame: %.3f frames\n", worst);
}

/**
 * Report the frames drums were struck on and the intervals between them.
 *
 * This is the rhythm, and it is what to compare against the real game. An
 * instruction-level trace of the original (traces/big-trace.log, strikes taken
 * at $F8B6 / $F8BD / $FA3A, the three sample-engine entry points) gives, for
 * tune 0:
 *
 *   1 7 8 8 1 7 8 1 7 8 1 7 8 1 7 8 8 1 7 1 7 8 1 7 ...
 */
static void report_strikes(void)
{
  int i;

  printf("drum strikes        %d\n", nstruck);
  printf("frames struck on (first 40):\n");
  for (i = 0; i < nstruck && i < 40; i++)
    printf(" %d", g_struck[i]);
  printf("\nintervals between strikes, in frames (first 40):\n");
  for (i = 1; i < nstruck && i < 41; i++)
    printf(" %d", g_struck[i] - g_struck[i - 1]);
  printf("\n");
}

/* ----------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
  chqstate_t *state;
  int         tune;
  int         frames;
  const char *wav;
  int         frame;
  int         edges_before;
  int         was_playing;

  tune   = (argc > 1) ? atoi(argv[1]) : 0;
  frames = (argc > 2) ? atoi(argv[2]) : 1200;
  wav    = (argc > 3) ? argv[3] : "title-drums.wav";

  if (frames > MAX_FRAMES)
    frames = MAX_FRAMES;

  speccy_init();

  state = chq_create(&g_speccy);
  assert(state != NULL);

  state->mode_128k = 1;

  chq_test_start_title_tune(state, (u8) tune);

  for (frame = 0; frame < frames; frame++) {
    g_frame_start = g_tstates;
    edges_before  = g_nedges;
    was_playing   = state->bank3->drums.sample_active;

    chq_test_run_title_tune(state);

    /* A frame that produced beeper output without a sample already in
     * progress is a fresh dispatch: the frame the drum was struck on. */
    if (g_nedges > edges_before && !was_playing && nstruck < MAX_FRAMES)
      g_struck[nstruck++] = frame;

    /* The engine only advances the virtual clock while it is bit-banging: the
     * AY tick, the drum dispatch and the idle wait cost it nothing. Real
     * hardware spends the rest of the interrupt period spinning on the $F8A8
     * frame flag ($F8A7), so close the frame out here. Without this the
     * rendered timeline would omit every silence and no interval between drum
     * hits would mean anything. */
    if (g_tstates - g_frame_start > g_worst_frame)
      g_worst_frame = g_tstates - g_frame_start;

    /* A frame that emitted more than one frame's worth of audio is the
     * "gallop" bug: the beeper timeline outruns wall time and, in the app,
     * nothing pulls it back (chq_speaker_handler only re-anchors when the
     * virtual clock falls behind). */
    assert(g_tstates - g_frame_start <= FRAME_TSTATES);

    g_tstates = g_frame_start + FRAME_TSTATES;

    g_frame_end[g_nframes]   = g_tstates;
    g_frame_edges[g_nframes] = g_nedges;
    g_nframes++;
  }

  printf("tune %d, %d frames\n", tune, frames);
  report_strikes();
  report_frames();
  report_bursts();
  write_wav(wav);

  chq_destroy(state);

  return 0;
}
