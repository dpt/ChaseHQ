// Pinched from TGE Spectrum.h + modified

#ifndef SPECTRUM_H
#define SPECTRUM_H

#define SCREEN_WIDTH                    (256)
#define SCREEN_HEIGHT                   (192)

#define ATTRIBUTE_BRIGHT                (1<<6)

enum {
  attribute_BLACK_OVER_BLACK          = 0,
  attribute_BLUE_OVER_BLACK           = 1,
  attribute_RED_OVER_BLACK            = 2,
  attribute_PURPLE_OVER_BLACK         = 3,
  attribute_GREEN_OVER_BLACK          = 4,
  attribute_CYAN_OVER_BLACK           = 5,
  attribute_YELLOW_OVER_BLACK         = 6,
  attribute_WHITE_OVER_BLACK          = 7,
  attribute_BRIGHT_BLUE_OVER_BLACK    = 65,
  attribute_BRIGHT_RED_OVER_BLACK     = 66,
  attribute_BRIGHT_PURPLE_OVER_BLACK  = 67,
  attribute_BRIGHT_GREEN_OVER_BLACK   = 68,
  attribute_BRIGHT_CYAN_OVER_BLACK    = 69,
  attribute_BRIGHT_YELLOW_OVER_BLACK  = 70,
  attribute_BRIGHT_WHITE_OVER_BLACK   = 71,
  attribute_BLACK_OVER_BRIGHT_RED     = 0x50,
  attribute_BLACK_OVER_BRIGHT_MAGENTA = 0x58,
  attribute_BLACK_OVER_BRIGHT_GREEN   = 0x60,
  attribute_BLACK_OVER_BRIGHT_CYAN    = 0x68,
  attribute_BLACK_OVER_BRIGHT_WHITE   = 0x78
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

#endif /* SPECTRUM_H */

