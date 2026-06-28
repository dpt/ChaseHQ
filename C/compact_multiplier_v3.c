// Even more compact equivalent for the multiplier algorithm
// This is a bit-by-bit multiplication with 9-bit multiplier (A) and multiplicand (v)
static inline int multiply_by_constant(int v, int A) {
    int result = 0;

    // Bit-by-bit multiplication with accumulation
    // The pattern: test bit, add if set, shift result left
    for (int i = 0; i < 9; i++) {
        if (A & 1) result += v;
        A >>= 1;
        if (i < 8) result <<= 1;
    }

    return result >> 8;
}

// Or even more compact - using the fact that we know it's exactly 9 bits
static inline int multiply_by_constant_compact(int v, int A) {
    // Unrolled version for clarity and performance
    int result = 0;

    // Process all 9 bits of A (LSB to MSB)
    if (A & 1) result += v; A >>= 1;
    if (A & 1) result += v; result <<= 1; A >>= 1;
    if (A & 1) result += v; result <<= 1; A >>= 1;
    if (A & 1) result += v; result <<= 1; A >>= 1;
    if (A & 1) result += v; result <<= 1; A >>= 1;
    if (A & 1) result += v; result <<= 1; A >>= 1;
    if (A & 1) result += v; result <<= 1; A >>= 1;
    if (A & 1) result += v; result <<= 1; A >>= 1;
    if (A & 1) result += v; result <<= 1;

    return result >> 8;
}