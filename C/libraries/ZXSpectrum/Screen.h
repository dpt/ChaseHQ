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

/**
 * Convert the given ZX Spectrum format screen into output pixels.
 *
 * \param[in] screen ZX Spectrum screen data.
 * \param[in] output Output screen pixels.
 * \param[in] dirty  Dirty rectangle in cartesian space - (0,0) is bottom left.
 * \param[in] bgr    Non-zero to output 0x00BBGGRR (e.g. ABGR8888),
 *                   zero to output 0x00RRGGBB (e.g. ARGB8888).
 * \param[in] mono   Non-zero to output a monochrome (green phosphor-style)
 *                   palette instead of the normal ZX Spectrum colours.
 */
void zxscreen_convert(const void    *screen,
                      unsigned int  *output,
                      const zxbox_t *dirty,
                      int            bgr,
                      int            mono);

// 4bpp variant
void zxscreen_convert16(const void    *vscr,
                        unsigned int  *poutput,
                        const zxbox_t *dirty);

#ifdef __cplusplus
}
#endif

#endif /* ZXSPECTRUM_SCREEN_H */

// vim: ts=8 sts=2 sw=2 et
