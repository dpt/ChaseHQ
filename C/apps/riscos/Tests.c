/*******************************************************************
 * File:        Tests
 * Purpose:     Native portability tests for ChaseHQ.
 * Author:      Gerph
 ******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "C99/Types.h"
#include "ZXSpectrum/Screen.h"
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
    zxconfig_t config;
    zxspectrum_t *zx;
    const zx_frame_t *frame;
    unsigned char screen[SCREEN_LENGTH];
    unsigned int pixels[SCREEN_WIDTH * SCREEN_HEIGHT / 8];
    zxbox_t dirty;

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

    memset(&config, 0, sizeof(config));
    config.width = SCREEN_WIDTH / 8;
    config.height = SCREEN_HEIGHT / 8;
    config.pixel_format = ZX_PIXEL_INDEXED4;
    zx = zxspectrum_create(&config);
    check(zx != NULL, "Spectrum allocation");
    if (zx != NULL)
    {
        frame = zxspectrum_claim_screen(zx);
        check(frame->format == ZX_PIXEL_INDEXED4, "indexed frame format");
        check(frame->stride == SCREEN_WIDTH / 2, "indexed frame stride");
        zxspectrum_release_screen(zx);
        zxspectrum_destroy(zx);
    }

    memset(screen, 0, sizeof(screen));
    memset(pixels, 0, sizeof(pixels));
    screen[0] = 0x80;
    screen[SCREEN_BITMAP_LENGTH] = ATTR_WHITE;
    dirty.x0 = 0;
    dirty.y0 = 0;
    dirty.x1 = SCREEN_WIDTH;
    dirty.y1 = SCREEN_HEIGHT;
    zxscreen_convert16(screen, pixels, &dirty);
    check(pixels[0] != 0 && pixels[1] == 0,
          "indexed 4-bpp conversion");

    if (failures != 0)
        return EXIT_FAILURE;

    printf("ChaseHQ native tests passed\n");
    return EXIT_SUCCESS;
}
