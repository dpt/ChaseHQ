/**
 * Types.h
 *
 * This file is part of "Chase H.Q. in C".
 *
 * This project recreates the ZX Spectrum version of the chase-and-smash game
 * "Chase H.Q." in portable C code. It is free software provided without
 * warranty in the interests of education and software preservation.
 *
 * The arcade original was created by Taito Corporation in 1988. It was then
 * ported to the ZX Spectrum by Ocean Software Limited and released in 1989.
 *
 * The original game and design is copyright (c) 1988 Taito Corporation.
 * The ZX Spectrum version is copyright (c) 1989 Ocean Software Limited.
 * The recreated version is copyright (c) 2023-2026 David Thomas.
 */

#ifndef CHASEHQ_TYPES_H
#define CHASEHQ_TYPES_H

/* ----------------------------------------------------------------------- */

#define TWOBYTES(addr) (addr) & 0xFF, (addr) >> 8

/* ZXSCREEN/ZXATTRS/CHQBACKBUF are aliases of TWOBYTES that document, at the
 * call site, whether the address pair feeds ADDRTOSCREEN($4000+), ADDRTOATTRS
 * ($5800+) or ADDRTOBACKBUF ($F000+). */
#define ZXSCREEN(addr)   TWOBYTES(addr)
#define ZXATTRS(addr)    TWOBYTES(addr)
#define CHQBACKBUF(addr) TWOBYTES(addr)

#define EOS (1<<7) // End of String - string terminator bit

/* print_character header byte bit 7: 0 = double-height shaded glyph,
 * 1 = single-height flat glyph. Not a hardware ZX attribute bit. */
#define SINGLE_HEIGHT (1<<7)

/* ----------------------------------------------------------------------- */

#endif /* CHASEHQ_TYPES_H */
