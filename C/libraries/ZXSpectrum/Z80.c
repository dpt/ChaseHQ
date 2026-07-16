/* Z80 instruction simulator functions. */

#include "ZXSpectrum/Z80.h"

uint8_t DAA_add(uint8_t v, int *carry_out)
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

uint8_t DAA_sub(uint8_t v, int half_borrow, int *carry_out)
{
  int carry = 0;

  if (half_borrow) v -= 0x06;
  if ((v >> 4) > 9) { v -= 0x60; carry = 1; }

  if (carry_out)
    *carry_out = carry;
  return v;
}
