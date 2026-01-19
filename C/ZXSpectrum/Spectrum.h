// Pinched from TGE Spectrum.h + modified

#ifndef SPECTRUM_H
#define SPECTRUM_H

#define SCREEN_WIDTH                    (256)
#define SCREEN_HEIGHT                   (192)

#define ATTR_BLACK                      (0)
#define ATTR_BLUE                       (1)
#define ATTR_RED                        (2)
#define ATTR_MAGENTA                    (3)
#define ATTR_GREEN                      (4)
#define ATTR_CYAN                       (5)
#define ATTR_YELLOW                     (6)
#define ATTR_WHITE                      (7)

#define ATTR_BRIGHT                     (1<<6)

// With [F]lash, [B]right, [P]aper, [I]nk: 0bFBPPPIII
#define MKATTR(F,B,P,I) (((F) << 7) | ((B) << 6) | ((P) << 3) | (I))

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

  attribute_BLACK_OVER_CYAN           = MKATTR(0, 0, ATTR_CYAN, ATTR_BLACK)
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
 * Bounding box.
 */
typedef struct zxbox
{
  int x0, y0, x1, y1;
}
zxbox_t;

#endif /* SPECTRUM_H */
