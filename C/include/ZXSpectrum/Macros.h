/* Macros.h
 *
 * Utility macros.
 *
 * Copyright (c) David Thomas, 2018. <dave@davespace.co.uk>
 */

#ifndef ZXSPECTRUM_MACROS_H
#define ZXSPECTRUM_MACROS_H

/**
 * Return the minimum of (a,b).
 */
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

/**
 * Return the maximum of (a,b).
 */
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

/**
 * Return 'a' clamped to the range [b..c].
 */
#define CLAMP(a,b,c) MIN(MAX(a,b),c)

/**
 * Mark a variable as deliberately unused, suppressing compiler warnings.
 */
#define NOT_USED(v) ((void) (v))

/**
 * Standard ZX Spectrum pixel address: x in [0,255], y in [0,191].
 */
#define XYTOSCREEN(x, y) \
  (0x4000 | (((y) & 0xC0) << 5) | (((y) & 0x07) << 8) | \
   (((y) & 0x38) << 2) | ((x) >> 3))

/**
 * Standard ZX Spectrum attribute address: x in [0,255], y in [0,191].
 */
#define XYTOATTRS(x, y) \
  (0x5800 | (((y) & 0xF8) << 2) | ((x) >> 3))

#endif /* ZXSPECTRUM_MACROS_H */

