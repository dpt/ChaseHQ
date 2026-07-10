/**
 * RenderStretchyObject.c
 *
 * Standalone visual debug tool: draws one stretchy object (a table entry
 * from a stage's addrof_right_hand_objects / addrof_left_hand_objects) into
 * a real chqstate_t's back buffer and dumps it as a PGM image, without
 * running the SDL front-end or the game loop.
 *
 * Reuses the same fake-Spectrum + state setup pattern as TestDrawRoad.c so
 * that height_table / xpos_road_centre are populated with real numbers from
 * a real stage, rather than hand-crafted arrays (see docs/stretchy-objects.md
 * on why IYheight/IXxpos must point into a real chqstate_t).
 *
 * Usage:
 *   StretchyRenderer [--stage 1-5] [--side left|right] [--index 1-6]
 *                     [--row 0-19] [--out path.pgm]
 *
 * Building:
 *   cmake --build cmake-build-debug --target StretchyRenderer
 *
 * ponytail: --index is capped at 6, not the nominal 9 the assert in
 * draw_scene_objects allows (ChaseHQ.c:3929). addrof_{left,right}_hand_
 * objects arrays are sized per stage with no length field, and several
 * stages' tables are shorter than 9 entries (checked empirically: 7-9
 * overflows on stage 2-5 under ASan). Raise the cap only alongside a real
 * per-stage length, not by guessing.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ChaseHQ/ChaseHQ.h"
#include "ChaseHQ/ChaseHQ-Internal.h"
#include "ChaseHQ/ChaseHQ-State.h"
#include "ChaseHQ/ChaseHQ-Stages.h"
#include "ChaseHQ/ChaseHQ-Tests.h"
#include "ZXSpectrum/Spectrum.h"

/* ----------------------------------------------------------------------- */
/* Minimal fake ZX Spectrum (same shape as TestDrawRoad.c)                  */
/* ----------------------------------------------------------------------- */

static uint8_t fake_in(zxspectrum_t *s, uint16_t addr)
{
  (void)s; (void)addr;
  return 0xFF;
}

static void fake_out(zxspectrum_t *s, uint16_t addr, uint8_t byte)
{
  (void)s; (void)addr; (void)byte;
}

static void fake_draw(zxspectrum_t *s, const zxbox_t *dirty)
{
  (void)s; (void)dirty;
}

static void fake_stamp(zxspectrum_t *s)
{
  (void)s;
}

static int fake_sleep(zxspectrum_t *s, int ticks)
{
  (void)s; (void)ticks;
  return 0;
}

static zxspectrum_t g_speccy;

static void speccy_init(void)
{
  memset(&g_speccy, 0, sizeof(g_speccy));
  g_speccy.in    = fake_in;
  g_speccy.out   = fake_out;
  g_speccy.draw  = fake_draw;
  g_speccy.stamp = fake_stamp;
  g_speccy.sleep = fake_sleep;
  g_speccy.screen.width  = SCREEN_WIDTH;
  g_speccy.screen.height = SCREEN_HEIGHT;
}

/* Build a state with real, populated height_table / xpos_road_centre for
 * the given stage — no hand-crafted geometry. */
static chqstate_t *make_state(int stage)
{
  chqstate_t *state;

  state = chq_create(&g_speccy);
  if (state == NULL) {
    fprintf(stderr, "chq_create failed\n");
    exit(1);
  }

  state->wanted_stage_number  = stage;
  state->current_stage_number = 0; /* force load */
  chq_test_load_stage(state);
  chq_test_set_up_stage(state);
  chq_test_build_height_table(state);
  chq_test_layout_road(state);

  return state;
}

/* Pick the first row (0..19) satisfying the same "object present" gate
 * draw_scene_objects checks at ChaseHQ.c:3933 before drawing: the high byte
 * of the s16 xpos_road_centre entry for this row must be zero. Returns -1
 * if none qualify. */
static int pick_row(const chqstate_t *state)
{
  int row;
  const u8 *high_byte;

  for (row = 0; row <= 19; row++) {
    high_byte = (const u8 *)&state->xpos_road_centre[88 + row];
    if (high_byte[1] == 0)
      return row;
  }

  return -1;
}

/*
 * Untouched sentinel bytes (0xFF, still whole from the initial memset) render
 * as mid-grey rather than solid black; without this the vast majority of the
 * canvas (nothing was drawn there — draw_road never ran) would swamp the
 * handful of bytes the object actually touched.
 */
/*
 * state->backbuffer uses its own interleave, distinct from the real screen's
 * — see the format comment above update_screen() in ChaseHQ.c:
 *   screen format: 0b010BBLLLRRRCCCCC (B=band, L=scanline, R=row-group)
 *   buffer format: 0b1111LLLLRRRCCCCC (L=scanline, R=row-group)
 * i.e. the buffer's low nibble is the fine scanline (*256) and the next 3
 * bits are the row-group (*32) — no band component, since the whole 128-row
 * buffer is one contiguous $F000 page. This matches the (y&0x0F)<<8 |
 * (y&0x70)<<1 address formula in draw_object_clipped exactly.
 */
static int deinterleave_row(int linear_y)
{
  return (linear_y & 0x0F) * 256 + ((linear_y >> 4) & 0x07) * 32;
}

static void write_pgm(const chqstate_t *state, FILE *out)
{
  int row, col, bit;
  const u8 *src;
  u8 byte;

  fprintf(out, "P5\n%d %d\n255\n", BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT);

  for (row = 0; row < BACKBUFFER_HEIGHT; row++) {
    src = &state->backbuffer[deinterleave_row(row)];
    for (col = 0; col < BACKBUFFER_ROWBYTES; col++) {
      byte = src[col];
      if (byte == 0xFF) {
        for (bit = 0; bit < 8; bit++)
          fputc(0x80, out);
        continue;
      }
      for (bit = 7; bit >= 0; bit--)
        fputc((byte & (1 << bit)) ? 0x00 : 0xFF, out);
    }
  }
}

static void usage(const char *prog)
{
  fprintf(stderr,
          "Usage: %s [--stage 1-5] [--side left|right] [--index 1-6] "
          "[--row 0-19] [--out path.pgm]\n",
          prog);
}

int main(int argc, char **argv)
{
  int          stage;
  const char  *side;
  int          index;
  int          row;
  const char  *out_path;
  int          i;
  chqstate_t  *state;
  const obj_t *obj;
  FILE        *out;

  stage    = 1;
  side     = "right";
  index    = 1;
  row      = -1; /* -1 means "auto-pick" */
  out_path = NULL;

  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--stage") == 0 && i + 1 < argc)
      stage = atoi(argv[++i]);
    else if (strcmp(argv[i], "--side") == 0 && i + 1 < argc)
      side = argv[++i];
    else if (strcmp(argv[i], "--index") == 0 && i + 1 < argc)
      index = atoi(argv[++i]);
    else if (strcmp(argv[i], "--row") == 0 && i + 1 < argc)
      row = atoi(argv[++i]);
    else if (strcmp(argv[i], "--out") == 0 && i + 1 < argc)
      out_path = argv[++i];
    else {
      usage(argv[0]);
      return 1;
    }
  }

  if (stage < 1 || stage > 5 || index < 1 || index > 6 ||
      (strcmp(side, "left") != 0 && strcmp(side, "right") != 0)) {
    usage(argv[0]);
    return 1;
  }

  speccy_init();
  state = make_state(stage);

  if (row == -1) {
    row = pick_row(state);
    if (row == -1) {
      fprintf(stderr,
              "no row 0-19 satisfies the object-present gate for stage %d; "
              "try a different stage or pass --row explicitly\n",
              stage);
      chq_destroy(state);
      return 1;
    }
    fprintf(stderr, "auto-picked row %d\n", row);
  } else if (row < 0 || row > 19) {
    usage(argv[0]);
    chq_destroy(state);
    return 1;
  }

  obj = strcmp(side, "right") == 0
      ? &state->stage->addrof_right_hand_objects[index]
      : &state->stage->addrof_left_hand_objects[index];

  if (obj->handler != draw_stretchy_object_left &&
      obj->handler != draw_stretchy_object_right) {
    /* obj_t.handler also covers non-stretchy objects (e.g. draw_overhead
     * for tunnels/bridges) with the same pointer signature but different
     * expectations about state and arg; calling those here crashes. */
    fprintf(stderr,
            "stage %d %s-hand object %d is not a stretchy object "
            "(handler %p) — skipping\n",
            stage, side, index, (void *)obj->handler);
    chq_destroy(state);
    return 1;
  }

  memset(state->backbuffer, 0xFF, BACKBUFFER_LENGTH);

  obj->handler(state, row + 1, obj->arg,
              &state->xpos_road_centre[88 + row],
              &state->height_table[21 - row]);

  {
    int written = 0;
    int i;
    for (i = 0; i < BACKBUFFER_LENGTH; i++)
      if (state->backbuffer[i] != 0xFF)
        written++;
    fprintf(stderr, "%d/%d backbuffer bytes changed from sentinel\n",
            written, BACKBUFFER_LENGTH);
  }

  if (out_path != NULL) {
    out = fopen(out_path, "wb");
    if (out == NULL) {
      fprintf(stderr, "cannot open %s for writing\n", out_path);
      chq_destroy(state);
      return 1;
    }
  } else {
    out = stdout;
  }

  write_pgm(state, out);

  if (out != stdout)
    fclose(out);

  chq_destroy(state);
  return 0;
}
