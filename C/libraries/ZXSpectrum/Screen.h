/* Screen.h
 *
 * ZX Spectrum screen decoding.
 *
 * Copyright (c) David Thomas, 2013-2018. <dave@davespace.co.uk>
 */

#ifndef ZXSPECTRUM_SCREEN_H
#define ZXSPECTRUM_SCREEN_H

#include "ZXSpectrum/Spectrum.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define ZXSCREEN_BGR    (1u << 0) /* output 0x00BBGGRR instead of 0x00RRGGBB */
#define ZXSCREEN_MONO   (1u << 1) /* monochrome (greyscale) palette */
#define ZXSCREEN_MELLOW (1u << 2) /* dimmed, desaturated CRT-style palette;
                                   * ignored if ZXSCREEN_MONO is also set */

/**
 * Convert the given ZX Spectrum format screen into output pixels.
 *
 * \param[in] screen ZX Spectrum screen data.
 * \param[in] output Output screen pixels.
 * \param[in] dirty  Dirty rectangle in cartesian space - (0,0) is bottom left.
 * \param[in] flags  Bitwise OR of ZXSCREEN_BGR, ZXSCREEN_MONO, ZXSCREEN_MELLOW.
 */
void zxscreen_convert(const void    *screen,
                      unsigned int  *output,
                      const zxbox_t *dirty,
                      unsigned int   flags);

// 4bpp variant
void zxscreen_convert16(const void    *vscr,
                        unsigned int  *poutput,
                        const zxbox_t *dirty);

#ifdef __cplusplus
}
#endif

#endif /* ZXSPECTRUM_SCREEN_H */

// vim: ts=8 sts=2 sw=2 et
