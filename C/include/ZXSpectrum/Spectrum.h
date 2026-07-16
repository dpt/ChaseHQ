/* Spectrum.h
 *
 * Interface to a logical ZX Spectrum.
 *
 * Copyright (c) David Thomas, 2013-2026. <dave@davespace.co.uk>
 */

#ifndef ZXSPECTRUM_H
#define ZXSPECTRUM_H

#include "C99/Types.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Constants */

#define SCREEN_WIDTH                    (256)
#define SCREEN_HEIGHT                   (192)

/**
 * Identifiers of screen attribute colours.
 */
#define ATTR_BLACK                      (0)
#define ATTR_BLUE                       (1)
#define ATTR_RED                        (2)
#define ATTR_MAGENTA                    (3)
#define ATTR_GREEN                      (4)
#define ATTR_CYAN                       (5)
#define ATTR_YELLOW                     (6)
#define ATTR_WHITE                      (7)

#define ATTR_BRIGHT                     (1<<6)

/**
 * Forms an attribute byte from its components.
 *
 * With [F]lash, [B]right, [P]aper, [I]nk: 0bFBPPPIII
 */
#define MKATTR(F,B,P,I) (((F) << 7) | ((B) << 6) | ((P) << 3) | (I))

/**
 * Identifiers of screen attributes.
 */
enum {
  attribute_BLACK_OVER_BLACK          = MKATTR(0, 0, ATTR_BLACK, ATTR_BLACK),

  attribute_BLUE_OVER_BLACK           = MKATTR(0, 0, ATTR_BLACK, ATTR_BLUE),
  attribute_RED_OVER_BLACK            = MKATTR(0, 0, ATTR_BLACK, ATTR_RED),
  attribute_MAGENTA_OVER_BLACK        = MKATTR(0, 0, ATTR_BLACK, ATTR_MAGENTA),
  attribute_GREEN_OVER_BLACK          = MKATTR(0, 0, ATTR_BLACK, ATTR_GREEN),
  attribute_CYAN_OVER_BLACK           = MKATTR(0, 0, ATTR_BLACK, ATTR_CYAN),
  attribute_YELLOW_OVER_BLACK         = MKATTR(0, 0, ATTR_BLACK, ATTR_YELLOW),
  attribute_WHITE_OVER_BLACK          = MKATTR(0, 0, ATTR_BLACK, ATTR_WHITE),

  attribute_BRIGHT_BLUE_OVER_BLACK    = MKATTR(0, 1, ATTR_BLACK, ATTR_BLUE),
  attribute_BRIGHT_RED_OVER_BLACK     = MKATTR(0, 1, ATTR_BLACK, ATTR_RED),
  attribute_BRIGHT_MAGENTA_OVER_BLACK = MKATTR(0, 1, ATTR_BLACK, ATTR_MAGENTA),
  attribute_BRIGHT_GREEN_OVER_BLACK   = MKATTR(0, 1, ATTR_BLACK, ATTR_GREEN),
  attribute_BRIGHT_CYAN_OVER_BLACK    = MKATTR(0, 1, ATTR_BLACK, ATTR_CYAN),
  attribute_BRIGHT_YELLOW_OVER_BLACK  = MKATTR(0, 1, ATTR_BLACK, ATTR_YELLOW),
  attribute_BRIGHT_WHITE_OVER_BLACK   = MKATTR(0, 1, ATTR_BLACK, ATTR_WHITE),

  attribute_BRIGHT_BLACK_OVER_BLUE    = MKATTR(0, 1, ATTR_BLUE, ATTR_BLACK),
  attribute_BRIGHT_BLACK_OVER_RED     = MKATTR(0, 1, ATTR_RED, ATTR_BLACK),
  attribute_BRIGHT_BLACK_OVER_MAGENTA = MKATTR(0, 1, ATTR_MAGENTA, ATTR_BLACK),
  attribute_BRIGHT_BLACK_OVER_GREEN   = MKATTR(0, 1, ATTR_GREEN, ATTR_BLACK),
  attribute_BRIGHT_BLACK_OVER_CYAN    = MKATTR(0, 1, ATTR_CYAN, ATTR_BLACK),
  attribute_BRIGHT_BLACK_OVER_YELLOW  = MKATTR(0, 1, ATTR_YELLOW, ATTR_BLACK),
  attribute_BRIGHT_BLACK_OVER_WHITE   = MKATTR(0, 1, ATTR_WHITE, ATTR_BLACK),

  attribute_BLACK_OVER_GREEN          = MKATTR(0, 0, ATTR_GREEN,  ATTR_BLACK),
  attribute_BLACK_OVER_CYAN           = MKATTR(0, 0, ATTR_CYAN,   ATTR_BLACK),
  attribute_BLACK_OVER_YELLOW         = MKATTR(0, 0, ATTR_YELLOW, ATTR_BLACK),
  attribute_BLACK_OVER_WHITE          = MKATTR(0, 0, ATTR_WHITE,  ATTR_BLACK)
};

/**
 * A screen attribute.
 */
typedef uint8_t attribute_t;

/**
 * Identifiers of port numbers.
 */
enum
{
  port_KEMPSTON_JOYSTICK        = 0x001F, /* 000FUDLR / active bits high */

  port_BORDER_EAR_MIC           = 0x00FE, /* Border, Ear, Mic */

  port_128K_PAGING              = 0x7FFD, /* 128K memory bank select */
  port_AY_REGISTER              = 0xFFFD, /* AY-3-8912 register select */
  port_AY_DATA                  = 0xBFFD, /* AY-3-8912 register write */

  port_KEYBOARD_SHIFTZXCV       = 0xFEFE, /* 11111110 */
  port_KEYBOARD_ASDFG           = 0xFDFE, /* 11111101 */
  port_KEYBOARD_QWERT           = 0xFBFE, /* 11111011 */
  port_KEYBOARD_12345           = 0xF7FE, /* 11110111 */
  port_KEYBOARD_09876           = 0xEFFE, /* 11101111 */
  port_KEYBOARD_POIUY           = 0xDFFE, /* 11011111 */
  port_KEYBOARD_ENTERLKJH       = 0xBFFE, /* 10111111 */
  port_KEYBOARD_SPACESYMSHFTMNB = 0x7FFE  /* 01111111 */
};

/**
 * Masks for port $FE.
 */
enum
{
  port_MASK_BORDER  = 7 << 0,
  port_MASK_MIC     = 1 << 3,
  port_MASK_EAR     = 1 << 4
};

/* Memory map */

#define ROM_START_ADDRESS               (0x0000)
#define ROM_LENGTH                      (0x4000)
#define ROM_END_ADDRESS                 (ROM_START_ADDRESS + ROM_LENGTH - 1)

#define SCREEN_START_ADDRESS            (ROM_END_ADDRESS + 1)
#define SCREEN_BITMAP_ROWBYTES          (SCREEN_WIDTH / 8)
#define SCREEN_BITMAP_LENGTH            (SCREEN_BITMAP_ROWBYTES * SCREEN_HEIGHT)
#define SCREEN_ATTRIBUTES_START_ADDRESS (SCREEN_START_ADDRESS + SCREEN_BITMAP_LENGTH)
#define SCREEN_ATTRIBUTES_WIDTH         (SCREEN_WIDTH / 8)
#define SCREEN_ATTRIBUTES_ROWBYTES      SCREEN_ATTRIBUTES_WIDTH
#define SCREEN_ATTRIBUTES_HEIGHT        (SCREEN_HEIGHT / 8)
#define SCREEN_ATTRIBUTES_LENGTH        (SCREEN_ATTRIBUTES_ROWBYTES * SCREEN_ATTRIBUTES_HEIGHT)
#define SCREEN_LENGTH                   (SCREEN_BITMAP_LENGTH + SCREEN_ATTRIBUTES_LENGTH)
#define SCREEN_END_ADDRESS              (SCREEN_START_ADDRESS + SCREEN_LENGTH - 1)
#define SCREEN_ATTRIBUTES_END_ADDRESS   SCREEN_END_ADDRESS

/**
 * The current state of the machine.
 */
typedef struct zxspectrum zxspectrum_t;

/**
 * Bounding box.
 */
typedef struct zxbox
{
  int x0, y0, x1, y1;
}
zxbox_t;

/**
 * Screen pixels and attributes.
 */
typedef struct zxscreen
{
  int         width, height; /* columns, rows */
  uint8_t     pixels[SCREEN_BITMAP_LENGTH];
  attribute_t attributes[SCREEN_ATTRIBUTES_LENGTH];
}
zxscreen_t;

/**
 * The current state of the machine.
 *
 * FIXME: Ought to make this structure private and expose .screen via an
 *        accessor function.
 */
struct zxspectrum
{
  /**
   * The game calls this to simulate an IN instruction.
   */
  uint8_t (*in)(zxspectrum_t *state, uint16_t address);

  /**
   * The game calls this to simulate an OUT instruction.
   */
  void (*out)(zxspectrum_t *state, uint16_t address, uint8_t byte);

  /**
   * The game calls this when screen memory has changed.
   *
   * \param[in] dirty Dirty region. (NULL => whole screen)
   */
  void (*draw)(zxspectrum_t *state, const zxbox_t *dirty);

  /**
   * The game calls this at the start of a timed segment.
   */
  void (*stamp)(zxspectrum_t *state);

  /**
   * The game calls this when a timed segment ends, to sleep if required.
   *
   * \param[in] duration Sleep duration in T-states.
   *
   * \return Non-zero if the thread should terminate, zero otherwise.
   */
  int (*sleep)(zxspectrum_t *state, int duration);

  /**
   * The game calls this to account for Z80 time consumed by delay loops.
   *
   * It advances a virtual T-state clock which timestamps speaker output,
   * letting bit-banged audio retain its original timing even though the C
   * code runs the "delay" in no time at all.
   *
   * \param[in] duration Elapsed time in T-states.
   */
  void (*addtime)(zxspectrum_t *state, int duration);

  zxscreen_t screen;
};

/**
 * A configuration and handler specifier for app (host environment) code.
 */
typedef struct zxconfig
{
  /** Screen dimensions. */
  int width, height;

  /** An opaque pointer passed into app callbacks. */
  void *opaque;

  /** App callback called when screen updates are ready. */
  void (*draw)(const zxbox_t *dirty, void *opaque);

  /** App callback called at the start of a timed segment. */
  void (*stamp)(void *opaque);

  /** App callback called when a timed segment ends, to sleep if required. */
  int (*sleep)(int duration, void *opaque);

  /** App callback called to test a key. */
  int (*key)(uint16_t port, void *opaque);

  /** App callback called to set the border colour. */
  void (*border)(int colour, void *opaque);

  /** App callback called to sound the speaker. 'tstates' is the virtual
   *  T-state clock at the moment of the OUT, advanced by the game via
   *  addtime; it gives the callback the inter-toggle spacing that
   *  wall-clock time cannot (the C "delay loops" run in no time). */
  void (*speaker)(int on_off, uint64_t tstates, void *opaque);

  /** App callback for AY-3-8912 register select (port_AY_REGISTER) and
   *  data write (port_AY_DATA). May be NULL. */
  void (*ay_out)(uint16_t port, uint8_t byte, void *opaque);

  /** Non-zero to output 0x00BBGGRR pixels (ABGR8888); zero for 0x00RRGGBB (ARGB8888). */
  int bgr_pixels;
}
zxconfig_t;

/**
 * Create a logical ZX Spectrum.
 *
 * \return New ZXSpectrum.
 */
zxspectrum_t *zxspectrum_create(const zxconfig_t *config);

/**
 * Destroy a logical ZX Spectrum.
 *
 * \param[in] doomed Doomed ZXSpectrum.
 */
void zxspectrum_destroy(zxspectrum_t *doomed);

/**
 * Lock the screen then return a converted screen buffer.
 *
 * Call this at the last moment when you're ready to use the screen pixels.
 *
 * \param[in] state ZXSpectrum state.
 *
 * \return Pixels.
 */
uint32_t *zxspectrum_claim_screen(zxspectrum_t *state);

/**
 * Unlock the screen.
 *
 * \param[in] state ZXSpectrum state.
 */
void zxspectrum_release_screen(zxspectrum_t *state);

#ifdef __cplusplus
}
#endif

#endif /* ZXSPECTRUM_H */

// vim: ts=8 sts=2 sw=2 et
