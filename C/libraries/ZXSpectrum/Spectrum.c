/* Spectrum.c
 *
 * Interface to a logical ZX Spectrum.
 *
 * Copyright (c) David Thomas, 2013-2026. <dave@davespace.co.uk>
 */

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "C99/Types.h"

#include "ZXSpectrum/Screen.h"
#include "ZXSpectrum/Macros.h"

#include "ZXSpectrum/Spectrum.h"

/* ----------------------------------------------------------------------- */

#if defined(_WIN32)

#include <windows.h>

#define mutex_t          CRITICAL_SECTION
#define mutex_init(M)    InitializeCriticalSection(&M)
#define mutex_destroy(M) DeleteCriticalSection(&M)
#define mutex_lock(M)    EnterCriticalSection(&M)
#define mutex_unlock(M)  LeaveCriticalSection(&M)

#elif defined(_POSIX_THREADS) || \
      defined(_POSIX_VERSION) || \
      defined(__unix__)       || \
      defined(__unix)         || \
     (defined(__APPLE__) && defined(__MACH__))

#include <pthread.h>

#define mutex_t          pthread_mutex_t
#define mutex_init(M)    pthread_mutex_init(&M, NULL)
#define mutex_lock(M)    pthread_mutex_lock(&M)
#define mutex_unlock(M)  pthread_mutex_unlock(&M)
#define mutex_destroy(M) pthread_mutex_destroy(&M)

#else

//#warning Default threading used

#define mutex_t          int
#define mutex_init(M)
#define mutex_lock(M)
#define mutex_unlock(M)
#define mutex_destroy(M)

#endif

typedef uint32_t outputpixel_t;
#ifdef __riscos
#define OUTPUT_SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 8)
#else
#define OUTPUT_SCREEN_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT)
#endif

/* ----------------------------------------------------------------------- */

/* Set minimums smaller than maximums to invalidate. */
static void zxbox_invalidate(zxbox_t *b)
{
  b->x0 = INT_MAX;
  b->y0 = INT_MAX;
  b->x1 = INT_MIN;
  b->y1 = INT_MIN;
}

/* Return true if box is valid. */
static int zxbox_is_valid(zxbox_t *b)
{
  return (b->x0 < b->x1) && (b->y0 < b->y1);
}

/* Set largest possible valid box. */
static void zxbox_maximise(zxbox_t *b)
{
  b->x0 = INT_MIN;
  b->y0 = INT_MIN;
  b->x1 = INT_MAX;
  b->y1 = INT_MAX;
}

/* Return true if a is later than b on the virtual clock. */
static int zxclock_is_after(zxclock_t a, zxclock_t b)
{
#ifdef __riscos
  return (zxclock_t) (a - b) < (zxclock_t) 0x80000000U;
#else
  return a > b;
#endif
}

/* Return true if box can hold (width,height) at (0,0). */
static int zxbox_exceeds(const zxbox_t *b, int width, int height)
{
  return (b->x0 <= 0)     && (b->y0 <= 0)      &&
         (b->x1 >= width) && (b->y1 >= height);
}

/* Depth of the stamp()/sleep() nesting the clock tracks. Matches the host's
 * own timestamp stack (MAXSTAMPS in the SDL app); both assert rather than
 * grow, so an unbalanced stamp shows up as a crash in the offending build
 * rather than as silently wrong audio timing. */
#define MAXSTAMPS (4)

/* Return the union in 'c' of boxes 'a' and 'b'. */
static void zxbox_union(const zxbox_t *a, const zxbox_t *b, zxbox_t *c)
{
  c->x0 = MIN(a->x0, b->x0);
  c->y0 = MIN(a->y0, b->y0);
  c->x1 = MAX(a->x1, b->x1);
  c->y1 = MAX(a->y1, b->y1);
}

/* ----------------------------------------------------------------------- */

typedef struct zxspectrum_private
{
  zxspectrum_t    pub;
  zxconfig_t      config;

  unsigned int    prev_border;

  zxclock_t       tstates; // virtual Z80 clock; game thread only (see logtime)
  zxclock_t       stamp_tstates[MAXSTAMPS]; // clock at each open stamp()
  int             nstamps;

  int             monochrome;

  mutex_t         lock;
  zxbox_t         dirty;
  zxscreen_t      screen_copy; // most recent 'complete' screen
  outputpixel_t   converted[OUTPUT_SCREEN_SIZE];
  zx_frame_t      frame;
}
zxspectrum_private_t;

/* ----------------------------------------------------------------------- */

/* Callbacks called on game thread */

static uint8_t zx_in(zxspectrum_t *state, uint16_t address)
{
  zxspectrum_private_t *prv = (zxspectrum_private_t *) state;

  switch (address)
  {
  case port_KEYBOARD_12345:
  case port_KEYBOARD_09876:
  case port_KEYBOARD_QWERT:
  case port_KEYBOARD_POIUY:
  case port_KEYBOARD_ASDFG:
  case port_KEYBOARD_ENTERLKJH:
  case port_KEYBOARD_SHIFTZXCV:
  case port_KEYBOARD_SPACESYMSHFTMNB:
  case port_KEMPSTON_JOYSTICK:
    return prv->config.key(address, prv->config.opaque);

  case port_BORDER_EAR_MIC:
    {
      /* Address 0x00FE: all row-select lines active simultaneously.
       * On real hardware this returns the AND of every keyboard row. */
      static const uint16_t rows[] = {
        port_KEYBOARD_SHIFTZXCV,       port_KEYBOARD_ASDFG,
        port_KEYBOARD_QWERT,           port_KEYBOARD_12345,
        port_KEYBOARD_09876,           port_KEYBOARD_POIUY,
        port_KEYBOARD_ENTERLKJH,       port_KEYBOARD_SPACESYMSHFTMNB,
      };
      int result;
      int i;

      result = 0xFF;
      for (i = 0; i < (int) NELEMS(rows); i++)
        result &= prv->config.key(rows[i], prv->config.opaque);
      return result;
    }

  default:
    assert(!"zx_in not implemented for that port");
    return 0x00;
  }
}

static void zx_out(zxspectrum_t *state, uint16_t address, uint8_t byte)
{
  zxspectrum_private_t *prv = (zxspectrum_private_t *) state;

  switch (address)
  {
  case port_BORDER_EAR_MIC:
    {
      unsigned int border;
      unsigned int ear;

      border = byte & port_MASK_BORDER;
      ear    = byte & port_MASK_EAR;

      if (border != prv->prev_border)
      {
        if (prv->config.border)
          prv->config.border(border, prv->config.opaque);
        prv->prev_border = border;
      }

      if (prv->config.speaker)
      {
        prv->tstates += 11; /* OUT (n),A itself costs 11 T-states */
        prv->config.speaker(ear != 0, prv->tstates, prv->config.opaque);
      }
    }
    break;

  case port_128K_PAGING:
    /* Memory bank selection: no-op in the C port (no physical paging). */
    break;

  case port_AY_REGISTER:
  case port_AY_DATA:
    /* OUT (C),A costs 12 T-states. Billed for the same reason the speaker's
     * OUT is: the host timestamps both streams from this one clock, so an
     * unbilled write would land on top of its neighbour. */
    prv->tstates += 12;
    if (prv->config.ay_out)
      prv->config.ay_out(address, byte, prv->tstates, prv->config.opaque);
    break;

  default:
    assert(!"zx_out not implemented for that port");
    break;
  }
}

/* The game is telling us that the screen it draws to has been modified.
 *
 * Only the game (thread) writes to pub.screen. This entry point is called
 * when the game modifies it and wants us to know that. We're not obligated
 * to cause a screen/window refresh immediately, so may maintain an overall
 * dirty rectangle coalesced from multiple updates, but doing it in a timely
 * manner will improve the game's latency.
 *
 * We try to avoid copying the whole screen buffer on every update, where
 * practical, for speed. We do a partial copy then update our dirty rectangle
 * ready for zxspectrum_claim_screen.
 *
 * It's tempting to not copy the pub.screen here but instead wait for a
 * redraw message from the OS and do it then. However, the duration of this
 * call is the only window we have for legal access to pub.screen, so we have
 * to quickly copy out into our own buffer from where zxspectrum_claim_screen
 * can have unimpeded access.
 */
static void zx_draw(zxspectrum_t *state, const zxbox_t *dirty)
{
  zxspectrum_private_t *prv = (zxspectrum_private_t *) state;

  mutex_lock(prv->lock);

  /* If no dirty rectangle was specified then assume the full screen. */
  if (dirty == NULL || zxbox_exceeds(dirty, SCREEN_WIDTH, SCREEN_HEIGHT))
  {
    /* Entire screen has been modified - copy it all. */
    memcpy(&prv->screen_copy, &prv->pub.screen, sizeof(prv->screen_copy));

    /* Maximise the overall dirty box that zxspectrum_claim_screen() will
     * use. */
    zxbox_maximise(&prv->dirty);
  }
  else
  {
    /* Copy pub.screen's dirty region into the screen copy. */

    zxbox_t box;
    int     width;
    int     height;
    int     linear_y;

    /* Clamp the dirty rectangle to the screen dimensions. */
    box.x0 = CLAMP(dirty->x0, 0, 255);
    box.y0 = CLAMP(dirty->y0, 0, 191);
    box.x1 = CLAMP(dirty->x1, 1, 256);
    box.y1 = CLAMP(dirty->y1, 1, 192);

    /* Divide down the x coordinates to get byte-sized quantities. */
    box.x0 = (box.x0    ) >> 3; /* divide to 0..31 rounding down */
    box.x1 = (box.x1 + 7) >> 3; /* divide to 0..31 rounding up */

    width = box.x1 - box.x0;

    /* Convert y coordinates into screen space - (0,0) is top left. */
    height = box.y1 - box.y0;
    box.y0 = 192 - box.y1;
    box.y1 = box.y0 + height;

    for (linear_y = box.y0; linear_y < box.y1; linear_y++)
    {
      /* Transpose fields using XOR */
      unsigned int tmp = (linear_y ^ (linear_y >> 3)) & 7;
      int          y   = linear_y ^ (tmp | (tmp << 3));

      memcpy(&prv->screen_copy.pixels[y * 32 + box.x0],
             &prv->pub.screen.pixels[y * 32 + box.x0],
             width);
    }

    /* Divide down the y coordinates to get attribute-sized quantities. */
    box.y0 = (box.y0    ) >> 3;
    box.y1 = (box.y1 + 7) >> 3;

    for (linear_y = box.y0; linear_y < box.y1; linear_y++)
    {
      memcpy(&prv->screen_copy.attributes[linear_y * 32 + box.x0],
             &prv->pub.screen.attributes[linear_y * 32 + box.x0],
             width);
    }

    /* Union the overall dirty box that zxspectrum_claim_screen() will use
     * with the outstanding one. */
    zxbox_union(&prv->dirty, dirty, &prv->dirty);
  }

  mutex_unlock(prv->lock);

  prv->config.draw(dirty, prv->config.opaque);
}

static void zx_stamp(zxspectrum_t *state)
{
  zxspectrum_private_t *prv = (zxspectrum_private_t *) state;

  /* Remember where the virtual clock stood, so the matching zx_sleep can
   * close the segment out on it. Stamps nest, so this is a stack, the same
   * shape and depth as the host's own wall-clock one. */
  assert(prv->nstamps < MAXSTAMPS);
  if (prv->nstamps < MAXSTAMPS)
    prv->stamp_tstates[prv->nstamps++] = prv->tstates;

  prv->config.stamp(prv->config.opaque);
}

static int zx_sleep(zxspectrum_t *state, int duration)
{
  zxspectrum_private_t *prv = (zxspectrum_private_t *) state;
  zxclock_t             segment_end;

  /* A real Z80 spends the whole interrupt period either working or waiting
   * on the frame flag; either way the clock has moved on by the segment's
   * full duration by the time the segment ends. The C port does the work in
   * a fraction of that and sleeps off the rest, so advance the clock here to
   * match -- but never rewind it, since a nested outer segment may end after
   * inner ones have already carried the clock past its own start+duration. */
  assert(prv->nstamps > 0);
  if (prv->nstamps > 0)
  {
    segment_end = prv->stamp_tstates[--prv->nstamps] + (zxclock_t) duration;
    if (zxclock_is_after(segment_end, prv->tstates))
      prv->tstates = segment_end;
  }

  return prv->config.sleep(duration, prv->config.opaque);
}

static void zx_addtime(zxspectrum_t *state, int duration)
{
  zxspectrum_private_t *prv = (zxspectrum_private_t *) state;

  prv->tstates += duration;
}

/* ----------------------------------------------------------------------- */

zxspectrum_t *zxspectrum_create(const zxconfig_t *config)
{
  zxspectrum_private_t *prv;

  prv = calloc(1, sizeof(*prv));
  if (prv == NULL)
    return NULL;

  prv->pub.in            = zx_in;
  prv->pub.out           = zx_out;
  prv->pub.draw          = zx_draw;
  prv->pub.stamp         = zx_stamp;
  prv->pub.sleep         = zx_sleep;
  prv->pub.logtime       = zx_addtime;
  prv->pub.screen.width  = config->width;
  prv->pub.screen.height = config->height;

  prv->config = *config;

  mutex_init(prv->lock);

  zxbox_invalidate(&prv->dirty);

  prv->prev_border  = ~0;
  prv->tstates      = 0;
  prv->frame.pixels = prv->converted;
  prv->frame.format = config->pixel_format;
  prv->frame.width  = SCREEN_WIDTH;
  prv->frame.height = SCREEN_HEIGHT;
  prv->frame.stride = (config->pixel_format == ZX_PIXEL_INDEXED4) ?
                      SCREEN_WIDTH / 2 : SCREEN_WIDTH * 4;

  return &prv->pub;
}

void zxspectrum_destroy(zxspectrum_t *doomed)
{
  zxspectrum_private_t *prv = (zxspectrum_private_t *) doomed;

  if (doomed == NULL)
    return;

  mutex_destroy(prv->lock);

  free(prv);
}

const zx_frame_t *zxspectrum_claim_screen(zxspectrum_t *state)
{
  zxspectrum_private_t *prv = (zxspectrum_private_t *) state;

  mutex_lock(prv->lock);

  /* Check for any changes */
  if (zxbox_is_valid(&prv->dirty))
  {
    /* Convert the screen only when it's asked for */
    if (prv->config.pixel_format == ZX_PIXEL_INDEXED4)
      zxscreen_convert16(prv->screen_copy.pixels, prv->converted, &prv->dirty);
    else
      zxscreen_convert(prv->screen_copy.pixels, prv->converted, &prv->dirty,
                       prv->config.pixel_format == ZX_PIXEL_ABGR8888,
                       prv->monochrome);

    /* Invalidate the dirty region once complete */
    zxbox_invalidate(&prv->dirty);
  }

  return &prv->frame;
}

void zxspectrum_set_monochrome(zxspectrum_t *state, int mono)
{
  zxspectrum_private_t *prv = (zxspectrum_private_t *) state;

  mutex_lock(prv->lock);
  prv->monochrome = mono;
  zxbox_maximise(&prv->dirty); /* force a full reconvert on the next claim */
  mutex_unlock(prv->lock);
}

void zxspectrum_release_screen(zxspectrum_t *state)
{
  zxspectrum_private_t *prv = (zxspectrum_private_t *) state;

  mutex_unlock(prv->lock);
}

// vim: ts=8 sts=2 sw=2 et
