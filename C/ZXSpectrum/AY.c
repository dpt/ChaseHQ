/* AY.c
*
 * AY-3-8912 sound emulation.
 *
 * Copyright (c) David Thomas, 2026. <dave@davespace.co.uk>
 */

/* Credit: Strongly inspired by https://github.com/ponceto/aym-js */

// TODO
//
// When to reset phases? When registers are set?
// Is it worth using chars or shorts in the wave/env generator structs?
// Do we run the wave generators constantly or do they shut down when unused?

#include <stdint.h>

//#include "ZXSpectrum/AY.h"
#include "AY.h"

#include <assert.h>
#include <stdlib.h>

/* ----------------------------------------------------------------------- */

/* AY configuration */
#define AY_FXP                 (8) /* fixed point precision bits */
#define AY_ENABLE_FIXUP        (1) /* synchronise tone generators */
#define AY_ENABLE_STEREO       (1) /* enable stereo output */

/* ----------------------------------------------------------------------- */

/* Noise limits */
#define AY_NOISE_PITCH_MAX  (0x1F) /* 0..31 => high pitch to low */

/* Envelope limits */
#define AY_ENV_MIN_VOL      (0x00)
#define AY_ENV_MAX_VOL      (0x1F)

/* ----------------------------------------------------------------------- */

/* Macros for common operations */
#define AY_MIN(a,b) ((a) < (b) ? (a) : (b))
#define AY_MAX(a,b) ((a) > (b) ? (a) : (b))
#define AY_CLAMP(x,min,max) AY_MAX(min, AY_MIN(max, x))
#define AY_DIV_NEAREST(a,b) (((a) + (b) / 2) / (b))

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

/* A tone generator */
typedef aywave_t aytone_t;

/* A noise generator
 * Uses a wave generator and outputs its phase modulated by an LFSR
 * random bit generator */
typedef struct {
  aywave_t      wave;
  unsigned int  lfsr; /* state, shift register */
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
  int           master_volume; /* 0..100 */
} aymixer_t;

/* AY state */
struct ay {
  uint_least8_t regs[16]; /* 16 registers (0-15) */

  /* Timing */
  int_least32_t clocks_per_sample; /* (clock_freq over sample_rate) << AY_FXP */
  int           clock_error;

  aytone_t      tone[AY_CHANNELS];
  aynoise_t     noise;
  ayenv_t       env;
  aymixer_t     mixer;
};

/* ----------------------------------------------------------------------- */

ay_t *ay_create(int clock_freq, int sample_rate)
{
  ay_t *ay;

  ay = calloc(sizeof(ay_t), 1);
  if (ay == NULL)
    return NULL;

  /* Initialise timing */
  ay->clocks_per_sample = (clock_freq << AY_FXP) / sample_rate;

  /* Initialise noise (lfsr never becomes zero) */
  ay->noise.lfsr = 1;

  /* Set default configuration */
  ay->mixer.master_volume = 100;

  return ay;
}

void ay_destroy(ay_t *ay)
{
  free(ay);
}

/* ----------------------------------------------------------------------- */

/* Set a register value */
void ay_write_register(ay_t *ay, ayreg_t reg, uint8_t value)
{
  int channel;

  if (reg > 15)
    return;

  ay->regs[reg] = value;

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
                                      ay->regs[AY_REG_CHANNEL_A_COARSE_PITCH + channel * 2] * 256, 1);
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
    ay->env.wave.phase = 0;
    ay->env.shape      = value;
    ay->env.volume     = (value & 4) ? AY_ENV_MIN_VOL : AY_ENV_MAX_VOL; /* match attack/decay shape */
    break;
  }
}

uint8_t ay_read_register(ay_t *ay, ayreg_t reg)
{
  return ay->regs[reg];
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
    n->wave.phase += (n->lfsr & 1);
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
  env->volume = AY_ENV_MAX_VOL;
}

static void ay_env_off(ayenv_t *env)
{
  env->volume = AY_ENV_MIN_VOL;
}

/* Update envelope */
static void ay_env_halfclock(ayenv_t *env)
{
  static ay_envfn_t *const envfn[16][2] = {
    { ay_env_decay,  ay_env_off },    /*  0 */
    { ay_env_decay,  ay_env_off },    /*  1 - not defined */
    { ay_env_decay,  ay_env_off },    /*  2 - not defined */
    { ay_env_decay,  ay_env_off },    /*  3 - not defined */
    { ay_env_attack, ay_env_off },    /*  4 */
    { ay_env_attack, ay_env_off },    /*  5 - not defined */
    { ay_env_attack, ay_env_off },    /*  6 - not defined */
    { ay_env_attack, ay_env_off },    /*  7 - not defined */
    { ay_env_decay,  ay_env_decay },  /*  8 */
    { ay_env_decay,  ay_env_off },    /*  9 */
    { ay_env_decay,  ay_env_attack }, /* 10 */
    { ay_env_decay,  ay_env_hold },   /* 11 */
    { ay_env_attack, ay_env_attack }, /* 12 */
    { ay_env_attack, ay_env_hold },   /* 13 */
    { ay_env_attack, ay_env_decay },  /* 14 */
    { ay_env_attack, ay_env_off },    /* 15 */
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

ay_sample_t ay_get_sample(ay_t *ay)
{
  int      wholeclocks;
  int      t;
  int      ch;
  uint8_t  mixer_reg;
  int      channel_enabled;
  uint16_t mixed_l[AY_CHANNELS];
  uint16_t mixed_r[AY_CHANNELS];
  int      output_l, output_r; /* these hold multiple channels */

  wholeclocks = (ay->clocks_per_sample + ay->clock_error) >> (AY_FXP + 4); /* + 4 is the 16 divider */
  ay->clock_error = ay->clocks_per_sample - (wholeclocks << (AY_FXP + 4));

  /* Generate two half waves for everything */
  for (t = 0; t < wholeclocks; t++) {
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

  channel_enabled = ~mixer_reg;
  channel_enabled = (channel_enabled & 7) | (channel_enabled >> 3);
  if (channel_enabled == 0) {
    /* No tone or noise channels are enabled - output silence */
    return 0;
  }

  for (ch = 0; ch < AY_CHANNELS; ch++) {
    int output;
    int vol;
    int amplitude;

    output = 0;
    if ((mixer_reg & (AY_MIXER_NO_TONE_A << ch)) == 0)
      output |= (ay->tone[ch].phase   & 1) ? +1 : -1;
    if ((mixer_reg & (AY_MIXER_NO_NOISE_A << ch)) == 0)
      output |= (ay->noise.wave.phase & 1) ? +1 : -1;

    vol = ay->regs[AY_REG_CHANNEL_A_VOLUME + ch];
    if (vol < 16)
      /* Reduce 16-bit DAC table values to 15-bit */
      amplitude = ay_dac[vol] / 2;
    else
      /* Apply envelope-generated volume */
      amplitude = ay->env.volume * 32767 / AY_ENV_MAX_VOL;

    /* Use rounding division to preserve precision */
    mixed_l[ch] = AY_DIV_NEAREST(output * amplitude, AY_CHANNELS);
    mixed_r[ch] = AY_DIV_NEAREST(output * amplitude, AY_CHANNELS);
  }

  if (AY_ENABLE_STEREO) {
    output_l = output_r = 0;
    if (channel_enabled & 1) {
      output_l += mixed_l[0] * 1 / 4;
      output_r += mixed_r[0] * 1 / 4;
    }
    if (channel_enabled & 2) {
      output_l += mixed_l[1] * 2 / 4;
      output_r += mixed_r[1] * 1 / 4;
    }
    if (channel_enabled & 4) {
      output_l += mixed_l[2] * 1 / 4;
      output_r += mixed_r[2] * 2 / 4;
    }
  } else {
    output_l = (mixed_l[0] + mixed_l[1] + mixed_l[2]) / 3;
    output_r = (mixed_r[0] + mixed_r[1] + mixed_r[2]) / 3;
  }

  /* Apply master volume */
  output_l = (output_l * ay->mixer.master_volume) / 100;
  output_r = (output_r * ay->mixer.master_volume) / 100;

  /* Clamp output to 16-bit range */
  output_l = AY_CLAMP(output_l, -32768, 32767);
  output_r = AY_CLAMP(output_r, -32768, 32767);

  return (output_r << 16) | output_l;
}

void ay_set_volume(ay_t *ay, int volume)
{
  ay->mixer.master_volume = AY_CLAMP(volume, 0, 100);
}
