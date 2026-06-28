// Compact equivalent for the multiplier algorithm
int compact_multiply(int v, int A) {
    int result = 0;

    // Process each bit of A (9 bits)
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