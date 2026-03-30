/* AY.h
*
 * AY-3-8912 sound emulation.
 *
 * Copyright (c) David Thomas, 2026. <dave@davespace.co.uk>
 */

#ifndef ZXSPECTRUM_AY_H
#define ZXSPECTRUM_AY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ----------------------------------------------------------------------- */

/* AY constants */
#define AY_CHANNELS                      (3)

/* Register addresses */
#define AY_REG_CHANNEL_A_FINE_PITCH      (0) /* 0-255 */
#define AY_REG_CHANNEL_A_COARSE_PITCH    (1) /* 0-15 */
#define AY_REG_CHANNEL_B_FINE_PITCH      (2) /* 0-255 */
#define AY_REG_CHANNEL_B_COARSE_PITCH    (3) /* 0-15 */
#define AY_REG_CHANNEL_C_FINE_PITCH      (4) /* 0-255 */
#define AY_REG_CHANNEL_C_COARSE_PITCH    (5) /* 0-15 */
#define AY_REG_NOISE_PITCH               (6) /* 0-31 */
#define AY_REG_MIXER                     (7)
#define AY_REG_CHANNEL_A_VOLUME          (8) /* 0-15 + override in bit 4 */
#define AY_REG_CHANNEL_B_VOLUME          (9) /* 0-15 + override in bit 4 */
#define AY_REG_CHANNEL_C_VOLUME         (10) /* 0-15 + override in bit 4 */
#define AY_REG_ENVELOPE_FINE_DURATION   (11) /* 0-255 */
#define AY_REG_ENVELOPE_COARSE_DURATION (12) /* 0-255 */
#define AY_REG_ENVELOPE_SHAPE           (13) /* 0-15 */

/* Mixer flags. These are active low! */
#define AY_MIXER_NO_TONE_A            (1<<0)
#define AY_MIXER_NO_TONE_B            (1<<1)
#define AY_MIXER_NO_TONE_C            (1<<2)
#define AY_MIXER_NO_NOISE_A           (1<<3)
#define AY_MIXER_NO_NOISE_B           (1<<4)
#define AY_MIXER_NO_NOISE_C           (1<<5)
#define AY_MIXER_ALL_ON                  (0)
#define AY_MIXER_TONE_ON                 (7)
#define AY_MIXER_ALL_OFF              (0x3F)

/* ----------------------------------------------------------------------- */

typedef struct ay ay_t;
typedef uint8_t ayreg_t;
typedef uint32_t ay_sample_t; /* 16-bit stereo pair as 0xRRRRLLLL */

/* ----------------------------------------------------------------------- */

ay_t *ay_create(int clock_freq, int sample_rate);
void ay_destroy(ay_t *ay);

void ay_write_register(ay_t *ay, ayreg_t reg, uint8_t value);
uint8_t ay_read_register(ay_t *ay, ayreg_t reg);

ay_sample_t ay_get_sample(ay_t *ay);

/* Set master volume (0-100) */
void ay_set_volume(ay_t *ay, int volume);

/* ----------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* ZXSPECTRUM_AY_H */

// vim: ts=8 sts=2 sw=2 et
