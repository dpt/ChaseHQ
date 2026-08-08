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

#include "Host.h"

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
    unsigned int actions;
    zxkey_t spectrum_key;
    zxjoystick_t joystick_key;
    unsigned char source_rows[6];
    unsigned char sprite_rows[6];
    chq_host_scale_factors_t factors;
    chq_host_clock_t host_clock;

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

    actions = chq_host_defer(0, CHQ_ACTION_START_GAME);
    actions = chq_host_defer(actions, CHQ_ACTION_QUIT_APP);
    check((actions & CHQ_ACTION_START_GAME) != 0 &&
          (actions & CHQ_ACTION_QUIT_APP) != 0,
          "deferred host action dispatch");

    check(chq_host_map_key(48, &spectrum_key, &joystick_key) &&
          spectrum_key == zxkey_1 && joystick_key == zxjoystick_UNKNOWN,
          "Spectrum key mapping");
    check(chq_host_map_key(121, &spectrum_key, &joystick_key) &&
          spectrum_key == zxkey_UNKNOWN && joystick_key == zxjoystick_RIGHT,
          "Kempston key mapping");
    check(!chq_host_map_key(127, &spectrum_key, &joystick_key),
          "unmapped RISC OS key");

    check(chq_host_iconbar_action(-2, 7, 7, 4) == CHQ_ICONBAR_OPEN,
          "iconbar Select decoding");
    check(chq_host_iconbar_action(-2, 7, 7, 2) == CHQ_ICONBAR_MENU,
          "iconbar Menu decoding");
    check(chq_host_iconbar_action(-2, 6, 7, 4) == CHQ_ICONBAR_NONE &&
          chq_host_iconbar_action(1, 7, 7, 4) == CHQ_ICONBAR_NONE,
          "iconbar source filtering");
    check(chq_host_iconbar_menu_y(44, 11) == 580,
          "iconbar menu positioning");

    chq_host_scale_factors(1, &factors);
    check(factors.xmag == 1 && factors.ymag == 1 &&
          factors.xdiv == 1 && factors.ydiv == 1,
          "one-to-one sprite scale factors");
    chq_host_scale_factors(3, &factors);
    check(factors.xmag == 3 && factors.ymag == 3 &&
          factors.xdiv == 1 && factors.ydiv == 1,
          "integer sprite scale factors");

    source_rows[0] = 1;
    source_rows[1] = 2;
    source_rows[2] = 3;
    source_rows[3] = 4;
    source_rows[4] = 5;
    source_rows[5] = 6;
    memset(sprite_rows, 0, sizeof(sprite_rows));
    chq_host_copy_frame(sprite_rows, source_rows, 3, 2);
    check(memcmp(sprite_rows, source_rows, sizeof(source_rows)) == 0,
          "sprite row order");

    check(chq_host_fullscreen_depth(0) == 2 &&
          chq_host_fullscreen_depth(1) == 3 &&
          chq_host_fullscreen_depth(2) == 5 &&
          chq_host_fullscreen_depth(3) == -1,
          "fullscreen depth fallback order");
    check(chq_host_sprite_action(2) == 0 &&
          chq_host_sprite_action(3) == 0 &&
          chq_host_sprite_action(5) == 32,
          "translation-table plot action");

    memset(&host_clock, 0, sizeof(host_clock));
    check(chq_host_advance_clock(&host_clock, CHQ_CLOCK_128K / 100,
                                 CHQ_CLOCK_128K, 1000) == 1 &&
          host_clock.valid && host_clock.deadline == 1001,
          "host clock first deadline");
    host_clock.deadline = 900;
    check(chq_host_advance_clock(&host_clock, CHQ_CLOCK_128K / 100,
                                 CHQ_CLOCK_128K, 1000) == 1 &&
          host_clock.deadline == 996,
          "host clock lag cap");
    check(chq_host_time_is_before(0xFFFFFFF0U, 0x00000020U),
          "host clock wrap comparison");

    if (failures != 0)
        return EXIT_FAILURE;

    printf("ChaseHQ native tests passed\n");
    return EXIT_SUCCESS;
}
