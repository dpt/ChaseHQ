// Compact equivalent for the multiplier algorithm in build_height_table
// This implements multiplication by a 9-bit value using bit-by-bit multiplication
static inline int fast_multiply_by_constant(int v, int A) {
    int result = 0;

    // Unrolled bit-by-bit multiplication (9 bits total)
    // Each iteration processes one bit of A and accumulates the result
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

// Even more compact version using a loop for the bit processing
static inline int fast_multiply_loop(int v, int A) {
    int result = 0;

    // Process 9 bits of A
    for (int i = 0; i < 9; i++) {
        if (A & 1) result += v;
        A >>= 1;
        if (i < 8) result <<= 1;  // Don't shift after the last bit
    }

    return result >> 8;
}