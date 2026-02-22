/* Z80 instruction simulator macros. */

#ifndef Z80_H
#define Z80_H

#include <stdint.h>

/**
 * Shift left arithmetic.
 */
#define SLA(r)        \
  do {                \
    carry = (r) >> 7; \
    (r) <<= 1;        \
  } while (0)

/**
 * Shift right logical.
 */
#define SRL(r)        \
  do {                \
    carry = (r) & 1;  \
    (r) >>= 1;        \
  } while (0)

/**
 * Rotate left through carry.
 */
#define RL(r)                   \
  do {                          \
    int carry_out;              \
                                \
    carry_out = (r) >> 7;       \
    (r) = ((r) << 1) | (carry); \
    carry = carry_out;          \
  } while (0)

/**
 * Rotate left
 */
#define RLC(r)                        \
  do {                                \
    carry = (r) >> 7;                 \
    (r) = ((r) << 1) | (carry);       \
  } while (0)

/**
 * Rotate right through carry.
 */
#define RR(r)                         \
  do {                                \
    int carry_out;                    \
                                      \
    carry_out = (r) & 1;              \
    (r) = ((r) >> 1) | (carry << 7);  \
    carry = carry_out;                \
  } while (0)

/**
 * Rotate right.
 */
#define RRC(r)                        \
  do {                                \
    carry = (r) & 1;                  \
    (r) = ((r) >> 1) | (carry << 7);  \
  } while (0)

/**
 * Rotate right (BCD) digit.
 */
#define RRD(acc, addr)                          \
  do {                                          \
    tmp = *addr & 0x0F;                         \
    *addr = (*addr >> 4) | ((acc & 0x0F) << 4); \
    acc = (acc & 0xF0) | tmp;                   \
  } while (0)

/// Minimal equivalent of Z80 BCD correct operation
/// Additional
static uint8_t DAA(uint8_t v, int *carry_out)
{
  int lo, hi;
  int carry = 0;

  lo = (v >> 0) & 0x0F;
  hi = (v >> 4) & 0x0F;
  if (lo >= 10) { lo -= 10; hi++; }
  if (hi >= 10) { hi -= 10; carry++; }

  if (carry_out)
    *carry_out = carry;
  return (hi << 4) | (lo << 0);
}

#endif /* Z80_H */

