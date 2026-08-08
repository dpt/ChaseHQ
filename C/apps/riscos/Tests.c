/*******************************************************************
 * File:        Tests
 * Purpose:     Native portability tests for ChaseHQ.
 * Author:      Gerph
 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "C99/Types.h"
#include "ZXSpectrum/Spectrum.h"

static int failures;

/*******************************************************************
 Function:      check
 Description:   Record a failed native assumption.
 Parameters:    condition = non-zero when the assumption holds
                name = description printed on failure
 Returns:       none
 ******************************************************************/
static void check(int condition, const char *name)
{
    if (!condition)
    {
        printf("FAIL: %s\n", name);
        failures++;
    }
}

/*******************************************************************
 Function:      main
 Description:   Check data layout and wrapping clock arithmetic.
 Parameters:    argc = argument count
                argv = argument vector
 Returns:       process status
 ******************************************************************/
int main(int argc, char **argv)
{
    zxclock_t before;
    zxclock_t after;
    zxclock_t elapsed;

    (void) argc;
    (void) argv;

    check(sizeof(u8) == 1, "u8 width");
    check(sizeof(u16) == 2, "u16 width");
    check(sizeof(uint32_t) == 4, "uint32_t width");
    check(sizeof(zxclock_t) == 4, "native clock width");
    check(SCREEN_WIDTH == 256 && SCREEN_HEIGHT == 192, "screen dimensions");

    before = (zxclock_t) 0xFFFFFFF0U;
    after = (zxclock_t) 0x00000020U;
    elapsed = after - before;
    check(elapsed == (zxclock_t) 0x30U, "modular elapsed time");

    if (failures != 0)
        return EXIT_FAILURE;

    printf("ChaseHQ native tests passed\n");
    return EXIT_SUCCESS;
}
