/* Z80 instruction simulator macros. */

#ifndef Z80_H
#define Z80_H

#include <stdint.h>

/* Z80 opcode bytes used as self-modifying instruction identifiers. */
#define Z80_CALL_NN 0xCD /* CALL nn */
#define Z80_INC_DE  0x13 /* INC DE  */
#define Z80_DEC_DE  0x1B /* DEC DE  */

/**
 * Shift left arithmetic.
 */
#define SLA(r)              \
  do {                      \
    carry = ((r) >> 7) & 1; \
    (r) <<= 1;              \
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
    carry_out = ((r) >> 7) & 1; \
    (r) = ((r) << 1) | (carry); \
    carry = carry_out;          \
  } while (0)

/**
 * Rotate left
 */
#define RLC(r)                        \
  do {                                \
    carry = ((r) >> 7) & 1;           \
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

/**
 * Minimal equivalent of Z80 BCD correct operation (after addition, N=0)
 */
uint8_t DAA_add(uint8_t v, int *carry_out);

/**
 * Minimal equivalent of Z80 BCD correct operation (after subtraction, N=0)
 *
 * half_borrow: 1 if the low nibble borrowed (i.e. original low nibble was 0).
 */
uint8_t DAA_sub(uint8_t v, int half_borrow, int *carry_out);

#endif /* Z80_H */

