/* Types.h
*
 * ISO C99 compatibility for The Great Esape.
 *
 * Copyright (c) David Thomas, 2020-2026. <dave@davespace.co.uk>
 */

#ifndef C99_TYPES_H
#define C99_TYPES_H

#include <stdint.h>

/* Number of elements in an array */
#define NELEMS(a) (sizeof(a) / sizeof((a)[0]))

/* Shorthand types */
typedef int16_t s16;
typedef int8_t s8;
typedef uint16_t u16;
typedef uint8_t u8;

/* Norcroft's 32-bit library has no native 64-bit integer type. Virtual clock
 * users perform modular subtraction on RISC OS, where this wraps safely. */
#ifdef __riscos
typedef uint32_t zxclock_t;
#else
typedef uint64_t zxclock_t;
#endif

#endif /* C99_TYPES_H */

// vim: ts=8 sts=2 sw=2 et
