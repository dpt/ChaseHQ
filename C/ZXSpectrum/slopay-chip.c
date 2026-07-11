/* slopay-chip.c
 *
 * AY-3-8912 sound chip emulator.
 *
 * Copyright (c) David Thomas, 2026. <dave@davespace.co.uk>
 *
 * SPDX-License-Identifier: MIT
 */

/* Credit: Strongly inspired by https://github.com/ponceto/aym-js */

#include <stdint.h>

#include "slopay-chip.h"

#include <stdlib.h>

/* ----------------------------------------------------------------------- */

/* AY configuration */

#define AY_FXP                  (8) /* fixed point precision bits */
#define AY_ENABLE_FIXUP         (1) /* synchronise tone generators */
#define AY_DC_BLOCK_R_Q15       (32604) /* 0.995 in Q15 */

/* ----------------------------------------------------------------------- */

/* Register masks */

#define AY_REG_MASK_BYTE        (0xFFu)
#define AY_REG_MASK_NIBBLE      AY_COARSE_PITCH_MAX
#define AY_REG_MASK_MIXER       AY_MIXER_MASK
#define AY_REG_MASK_VOLUME      AY_VOLUME_MAX
#define AY_REG_MASK_ENVELOPE_SHAPE AY_ENVELOPE_SHAPE_MAX

/* Envelope limits */

#define AY_ENV_MIN_VOL          (0x00)
#define AY_ENV_MAX_VOL          AY_VOLUME_MAX

/* ----------------------------------------------------------------------- */

/* Macros for common operations */

#define AY_MIN(a,b) ((a) < (b) ? (a) : (b))
#define AY_MAX(a,b) ((a) > (b) ? (a) : (b))
#define AY_CLAMP(x,min,max) AY_MAX(min, AY_MIN(max, x))

/* ----------------------------------------------------------------------- */

/* Q15 fixed-point helpers */

typedef int32_t ay_q15_t;

#define AY_Q15_SHIFT            (15)
#define AY_Q15_ONE              (1 << AY_Q15_SHIFT)
#define AY_Q15_FROM_INT(v)      ((ay_q15_t)((v) << AY_Q15_SHIFT))
#define AY_Q15_TO_INT(v)        ((int)((v) >> AY_Q15_SHIFT))
#define AY_Q15_MUL(a,b)         ((ay_q15_t)((((int64_t)(a) * (int64_t)(b)) + (AY_Q15_ONE >> 1)) >> AY_Q15_SHIFT))

/* ----------------------------------------------------------------------- */

/* AY volume table (c) by V_Soft and Lion 17 */
static const uint16_t ay_dac[16] = {
      0,   513,   828,  1239,
   1923,  3238,  4926,  9110,
  10344, 17876, 24682, 30442,
  38844, 47270, 56402, 65535
};

/* ----------------------------------------------------------------------- */

/* A wave generator
 * This toggles its phase bit once per complete period. */
typedef struct {
  int           counter; /* state */
  int           period;  /* input */
  int           phase;   /* output */
} aywave_t;

/* A tone generator
 * This is just a wave generator. */
typedef aywave_t aytone_t;

/* A noise generator
 * Uses a wave generator clock plus a 17-bit LFSR and exposes the
 * current noise output bit directly. */
typedef struct {
  aywave_t      wave;
  unsigned int  lfsr; /* state, shift register */
  int           output;
} aynoise_t;

/* An envelope generator
 * Uses a wave generator and outputs a volume modulated by an
 * attack/decay shape */
typedef struct {
  aywave_t      wave;
  int           shape;  /* input */
  int           volume; /* output */
} ayenv_t;

/* A mixer */
typedef struct {
  int                       master_volume; /* 0..AY_MASTER_VOLUME_MAX */
  slopay_chip_stereo_mode_t stereo_mode;
  ay_q15_t                  dc_prev_in_l_q15;
  ay_q15_t                  dc_prev_in_r_q15;
  ay_q15_t                  dc_prev_out_l_q15;
  ay_q15_t                  dc_prev_out_r_q15;
} aymixer_t;

/* AY state */
struct slopay_chip {
  uint_least8_t regs[AY_REG_COUNT]; /* AY registers */

  /* Timing */
  int_least32_t clocks_per_sample; /* (clock_freq over sample_rate) << AY_FXP */
  int           clock_error;

  aytone_t      tone[AY_CHANNELS];
  aynoise_t     noise;
  ayenv_t       env;
  aymixer_t     mixer;
};

/* ----------------------------------------------------------------------- */

slopay_chip_t *slopay_chip_create(int clock_freq, int sample_rate)
{
  slopay_chip_t *ay;

  ay = calloc(sizeof(slopay_chip_t), 1);
  if (ay == NULL)
    return NULL;

  /* Initialise timing */
  ay->clocks_per_sample = (clock_freq << AY_FXP) / sample_rate;

  /* Initialise noise (lfsr never becomes zero) */
  ay->noise.lfsr   = 1;
  ay->noise.output = 1;

  /* Set default configuration */
  ay->mixer.master_volume = AY_MASTER_VOLUME_MAX;
  ay->mixer.stereo_mode   = SLOPAY_CHIP_STEREO_MODE_ABC;

  for (int ch = 0; ch < AY_CHANNELS; ch++)
    ay->tone[ch].phase = 1;

  return ay;
}

void slopay_chip_destroy(slopay_chip_t *ay)
{
  free(ay);
}

/* ----------------------------------------------------------------------- */

/* Set a register value */
void slopay_chip_write_register(slopay_chip_t *ay, slopay_chip_reg_t reg, uint8_t value)
{
  static const uint8_t reg_masks[AY_REG_COUNT] = {
    [AY_REG_CHANNEL_A_FINE_PITCH]     = AY_REG_MASK_BYTE,
    [AY_REG_CHANNEL_A_COARSE_PITCH]   = AY_REG_MASK_NIBBLE,
    [AY_REG_CHANNEL_B_FINE_PITCH]     = AY_REG_MASK_BYTE,
    [AY_REG_CHANNEL_B_COARSE_PITCH]   = AY_REG_MASK_NIBBLE,
    [AY_REG_CHANNEL_C_FINE_PITCH]     = AY_REG_MASK_BYTE,
    [AY_REG_CHANNEL_C_COARSE_PITCH]   = AY_REG_MASK_NIBBLE,
    [AY_REG_NOISE_PITCH]              = AY_NOISE_PITCH_MAX,
    [AY_REG_MIXER]                    = AY_REG_MASK_MIXER,
    [AY_REG_CHANNEL_A_VOLUME]         = AY_REG_MASK_VOLUME,
    [AY_REG_CHANNEL_B_VOLUME]         = AY_REG_MASK_VOLUME,
    [AY_REG_CHANNEL_C_VOLUME]         = AY_REG_MASK_VOLUME,
    [AY_REG_ENVELOPE_FINE_DURATION]   = AY_REG_MASK_BYTE,
    [AY_REG_ENVELOPE_COARSE_DURATION] = AY_REG_MASK_BYTE,
    [AY_REG_ENVELOPE_SHAPE]           = AY_REG_MASK_ENVELOPE_SHAPE,
  };

  int channel;

  if (reg > AY_REG_ENVELOPE_SHAPE)
    return;

  /* Mask incoming values to valid ranges for each register */
  ay->regs[reg] = value & reg_masks[reg];

  /* Update internal state based on register changes */
  switch (reg) {
  case AY_REG_CHANNEL_A_FINE_PITCH:
  case AY_REG_CHANNEL_A_COARSE_PITCH:
  case AY_REG_CHANNEL_B_FINE_PITCH:
  case AY_REG_CHANNEL_B_COARSE_PITCH:
  case AY_REG_CHANNEL_C_FINE_PITCH:
  case AY_REG_CHANNEL_C_COARSE_PITCH:
    channel = (reg - AY_REG_CHANNEL_A_FINE_PITCH) >> 1;
    ay->tone[channel].period = AY_MAX(ay->regs[AY_REG_CHANNEL_A_FINE_PITCH + channel * 2] +
                                      (ay->regs[AY_REG_CHANNEL_A_COARSE_PITCH + channel * 2] & AY_REG_MASK_NIBBLE) * 256, 1);
    break;

  case AY_REG_NOISE_PITCH:
    ay->noise.wave.period = AY_MAX(value & AY_NOISE_PITCH_MAX, 1);
    break;

  case AY_REG_ENVELOPE_FINE_DURATION:
  case AY_REG_ENVELOPE_COARSE_DURATION:
    ay->env.wave.period = AY_MAX(ay->regs[AY_REG_ENVELOPE_FINE_DURATION] +
                                 ay->regs[AY_REG_ENVELOPE_COARSE_DURATION] * 256, 1);
    break;

  case AY_REG_ENVELOPE_SHAPE:
    ay->env.wave.counter = 0; /* restart envelope from the beginning of a full period */
    ay->env.wave.phase   = 0;
    ay->env.shape        = value & AY_REG_MASK_ENVELOPE_SHAPE;
    ay->env.volume       = (value & AY_ENVELOPE_SHAPE_ATTACK_BIT) ? AY_ENV_MIN_VOL : AY_ENV_MAX_VOL; /* match attack/decay shape */
    break;
  default:
    break;
  }
}

uint8_t slopay_chip_read_register(slopay_chip_t *ay, slopay_chip_reg_t reg)
{
  return (reg <= AY_REG_ENVELOPE_SHAPE) ? ay->regs[reg] : 0;
}

/* Update tone channel output */
static void ay_tone_halfclock(aytone_t *t)
{
  if (++t->counter >= t->period) {
    t->counter = 0;
    t->phase++;
  }
}

/* Update noise output */
static void ay_noise_halfclock(aynoise_t *n)
{
  unsigned int lfsr;

  if (++n->wave.counter >= n->wave.period) {
    n->wave.counter = 0;

    /* 17-bit Fibonacci LFSR with taps on bits 0 and 3 */
    lfsr = n->lfsr;
    n->lfsr = (lfsr >> 1) | ((((lfsr >> 0) & 1) ^ ((lfsr >> 3) & 1)) << 16);
    n->output = (int)(n->lfsr & 1u);
  }
}

typedef void (ay_envfn_t)(ayenv_t *env);

static void ay_env_attack(ayenv_t *env)
{
  const int volume = (env->volume + 1) & AY_ENV_MAX_VOL;
  if (volume == AY_ENV_MAX_VOL)
    env->wave.phase++;
  env->volume = volume;
}

static void ay_env_decay(ayenv_t *env)
{
  const int volume = (env->volume - 1) & AY_ENV_MAX_VOL;
  if (volume == AY_ENV_MIN_VOL)
    env->wave.phase++;
  env->volume = volume;
}

static void ay_env_hold(ayenv_t *env)
{
  /* Hold at whatever volume the preceding ramp settled on.
   * Shape 13 (attack→hold) arrives here at AY_ENV_MAX_VOL;
   * shape 11 (decay→hold) arrives here at AY_ENV_MIN_VOL.
   * Forcing a fixed level here would be wrong for one of the two cases. */
  (void)env;
}

static void ay_env_off(ayenv_t *env)
{
  env->volume = AY_ENV_MIN_VOL;
}

/* Update envelope */
static void ay_env_halfclock(ayenv_t *env)
{
  static ay_envfn_t *const envfn[16][2] = {
    { ay_env_decay,  ay_env_off    }, /*  0 */
    { ay_env_decay,  ay_env_off    }, /*  1 - not defined */
    { ay_env_decay,  ay_env_off    }, /*  2 - not defined */
    { ay_env_decay,  ay_env_off    }, /*  3 - not defined */
    { ay_env_attack, ay_env_off    }, /*  4 */
    { ay_env_attack, ay_env_off    }, /*  5 - not defined */
    { ay_env_attack, ay_env_off    }, /*  6 - not defined */
    { ay_env_attack, ay_env_off    }, /*  7 - not defined */
    { ay_env_decay,  ay_env_decay  }, /*  8 */
    { ay_env_decay,  ay_env_off    }, /*  9 */
    { ay_env_decay,  ay_env_attack }, /* 10 */
    { ay_env_decay,  ay_env_hold   }, /* 11 */
    { ay_env_attack, ay_env_attack }, /* 12 */
    { ay_env_attack, ay_env_hold   }, /* 13 */
    { ay_env_attack, ay_env_decay  }, /* 14 */
    { ay_env_attack, ay_env_off    }, /* 15 */
  };

  if (++env->wave.counter >= env->wave.period) {
    env->wave.counter = 0;

    envfn[env->shape][env->wave.phase & 1](env);
  }
}

static void ay_fixup_tone(aytone_t *l, const aytone_t *r)
{
  if (l->period == r->period && l->counter != r->counter) {
    l->counter = r->counter;
    l->phase   = r->phase;
  }
}

static int ay_dc_block(ay_q15_t *prev_in_q15, ay_q15_t *prev_out_q15, int input)
{
  const ay_q15_t x_q15     = AY_Q15_FROM_INT(input);
  const ay_q15_t delta_q15 = x_q15 - *prev_in_q15;
  const ay_q15_t fb_q15    = AY_Q15_MUL(AY_DC_BLOCK_R_Q15, *prev_out_q15);
  const ay_q15_t y_q15     = delta_q15 + fb_q15;

  *prev_in_q15  = x_q15;
  *prev_out_q15 = y_q15;

  return AY_Q15_TO_INT(y_q15);
}

slopay_chip_sample_t slopay_chip_get_sample(slopay_chip_t *ay)
{
  int     total_clocks_fxp;
  int     whole_clocks;
  int     t;
  int     ch;
  uint8_t mixer_reg;
  int     mixed[AY_CHANNELS];
  int     output_l, output_r; /* these hold multiple channels */
  int     final_l, final_r;

  total_clocks_fxp = ay->clocks_per_sample + ay->clock_error;
  whole_clocks = total_clocks_fxp >> (AY_FXP + 4); /* +4 is the 16 divider */
  ay->clock_error = total_clocks_fxp - (whole_clocks << (AY_FXP + 4));

  /* All generators run unconditionally, matching real AY-3-8912 behaviour.
   * The mixer register and volume registers gate the *output* only; stopping
   * a generator here would corrupt phase continuity and LFSR state when a
   * channel is re-enabled. */
  for (t = 0; t < whole_clocks; t++) {
    if (AY_ENABLE_FIXUP) {
      /* If two tone generators have the same period, but different counter and phase, then synchronise them. */
      ay_fixup_tone(&ay->tone[0], &ay->tone[1]);
      ay_fixup_tone(&ay->tone[0], &ay->tone[2]);
      ay_fixup_tone(&ay->tone[1], &ay->tone[2]);
    }

    for (ch = 0; ch < AY_CHANNELS; ch++) {
      ay_tone_halfclock(&ay->tone[ch]);
      ay_tone_halfclock(&ay->tone[ch]);
    }
    ay_noise_halfclock(&ay->noise);
    ay_noise_halfclock(&ay->noise);
    ay_env_halfclock(&ay->env);
    ay_env_halfclock(&ay->env);
  }

  mixer_reg = ay->regs[AY_REG_MIXER];

  for (ch = 0; ch < AY_CHANNELS; ch++) {
    /*
     * AY mixer bits are active-low masks. A disabled tone/noise source does
     * not mute the channel; it forces that gate high. This matters for
     * sampled speech and other DAC-style tricks that drive output primarily
     * through rapid volume-register updates.
     */
    const int tone_disabled  = (mixer_reg & (AY_MIXER_NO_TONE_A  << ch)) != 0;
    const int noise_disabled = (mixer_reg & (AY_MIXER_NO_NOISE_A << ch)) != 0;
    const int tone_high      = (ay->tone[ch].phase & 1) != 0;
    const int noise_high     = (ay->noise.output)       != 0;

    int output;
    int vol;
    int amplitude;

    output = ((tone_disabled || tone_high) && (noise_disabled || noise_high)) ? +1 : -1;

    vol = ay->regs[AY_REG_CHANNEL_A_VOLUME + ch];
    if ((vol & AY_VOLUME_USE_ENVELOPE_BIT) == 0)
      /* Reduce 16-bit DAC table values to 15-bit */
      amplitude = ay_dac[vol] / 2;
    else
      /* Apply envelope-generated volume */
      amplitude = ay->env.volume * 32767 / AY_ENV_MAX_VOL;

    mixed[ch] = (output > 0) ? amplitude : 0;
  }

  if (ay->mixer.stereo_mode != SLOPAY_CHIP_STEREO_MODE_MONO) {
    int left_sum;
    int right_sum;

    /* Right channel is always B + C in both stereo modes. */
    right_sum = mixed[1] + mixed[2];

    if (ay->mixer.stereo_mode == SLOPAY_CHIP_STEREO_MODE_ACB) {
      /* ACB: left = A + C */
      left_sum = mixed[0] + mixed[2];
    } else {
      /* ABC: left = A + B */
      left_sum = mixed[0] + mixed[1];
    }

    output_l = left_sum / 2;
    output_r = right_sum / 2;
  } else {
    const int mono_sum = mixed[0] + mixed[1] + mixed[2];
    output_l = output_r = mono_sum / 3;
  }

  /* Unipolar channel summing introduces DC; remove it before volume/clamp. */
  output_l = ay_dc_block(&ay->mixer.dc_prev_in_l_q15, &ay->mixer.dc_prev_out_l_q15, output_l);
  output_r = ay_dc_block(&ay->mixer.dc_prev_in_r_q15, &ay->mixer.dc_prev_out_r_q15, output_r);

  /* Apply master volume */
  output_l = (output_l * ay->mixer.master_volume) / AY_MASTER_VOLUME_MAX;
  output_r = (output_r * ay->mixer.master_volume) / AY_MASTER_VOLUME_MAX;

  /* Clamp output to 16-bit range */
  final_l = AY_CLAMP(output_l, -32768, 32767);
  final_r = AY_CLAMP(output_r, -32768, 32767);

  return (final_r << 16) | (final_l & 0xFFFF);
}

void slopay_chip_set_volume(slopay_chip_t *ay, int volume)
{
  ay->mixer.master_volume = AY_CLAMP(volume, 0, AY_MASTER_VOLUME_MAX);
}

void slopay_chip_set_stereo_mode(slopay_chip_t *ay, slopay_chip_stereo_mode_t mode)
{
  ay->mixer.stereo_mode = (mode > SLOPAY_CHIP_STEREO_MODE_ACB) ? SLOPAY_CHIP_STEREO_MODE_ABC : mode;
}
