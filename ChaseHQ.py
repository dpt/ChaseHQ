# SkoolKit extension for Chase H.Q. by Ocean Software.
#
# by David Thomas, 2023-2025
#

"""
Defines the :class:`ChaseHQWriter`, :class:`ChaseHQHtmlWriter` and :class:`ChaseHQAsmWriter` classes.
"""

import time

from skoolkit.graphics import Frame, Udg
from skoolkit.skoolasm import AsmWriter
from skoolkit.skoolhtml import HtmlWriter

# Default attribute byte to use when decoding graphics.
ZX_ATTRIBUTE_BLACK_OVER_YELLOW = 48

# ZX Spectrum colour value to name mapping.
ZX_COLOUR_NAMES = [
    "Black",
    "Blue",
    "Red",
    "Magenta",
    "Green",
    "Cyan",
    "Yellow",
    "White",
    "Bright Black",
    "Bright Blue",
    "Bright Red",
    "Bright Magenta",
    "Bright Green",
    "Bright Cyan",
    "Bright Yellow",
    "Bright White",
]


class ChaseHQWriter:
    pass


class ChaseHQHtmlWriter(HtmlWriter, ChaseHQWriter):
    def init(self):
        self.font = {}

    def decode_pregame_screen(self, cwd, base):
        """Decodes the instructions used to draw the CHASE HQ MONITORING SYSTEM screen."""
        output = ""
        basep = base
        while 1:
            b = self.snapshot[basep]
            basep = basep + 1
            if b == 0x00:
                output += "Stop<br/>\n"
                return output
            elif b >= 0x01 and b <= 0x1E:
                output += "Repeat %d<br/>\n" % (b)
            elif b >= 0x1F and b <= 0x1F + 45:
                output += "Plot tile %d<br/>\n" % (b - 0x1F)
            elif b >= 0xD0 and b <= 0xDF:
                c = b - 0xD0
                output += "Set colour %d (%s)<br/>\n" % (c, ZX_COLOUR_NAMES[c])
            elif b == 0xE1:
                output += "Draw horizontally<br/>\n"
            elif b == 0xE2:
                output += "Draw vertically<br/>\n"
            elif b >= 0xF0 and b <= 0xFF:
                b = (b << 8) | self.snapshot[basep]
                basep = basep + 1
                x = b & 0x1F
                y = ((b & 0x00D0) | ((b & 0x0F00) >> 7)) >> 4
                output += "Set address to (%d,%d)<br/>\n" % (x, y)
            else:
                output += f"Unknown {b:X}<br/>\n"
        return output

    def decode_nibble_rle(self, cwd, base, typename, names, showlength, follow):
        output = f"Start of {typename} data at ${base:X} (nibble counted)<br/>"
        runlength = 0
        lasttype = -1
        totallength = 0
        basep = base
        while 1:
            b = self.snapshot[basep]
            basep = basep + 1
            if b == 0:  # Escape
                if lasttype != -1:
                    output += "- %s for %d units<br/>" % (names[lasttype], runlength)
                    totallength += runlength
                b = self.snapshot[basep]
                basep = basep + 1
                if b == 0:  # Jump
                    loopdest = self.snapshot[basep + 0] + self.snapshot[basep + 1] * 256
                    if loopdest == base:
                        output += "- Loop<br/>"
                        if showlength:
                            output += "+ Total length = %d<br/>" % (totallength)
                    else:
                        output += f"- Jump to ${loopdest:X}<br/>"
                        if showlength:
                            output += "+ Total length = %d<br/>" % (totallength)
                        if follow:
                            output += "<strong>Jump</strong><br/>"
                            output += self.decode_nibble_rle(
                                cwd, loopdest, typename, names, showlength, follow
                            )
                elif b == 1:  # Fork End
                    output += "- Fork End<br/>"
                elif b == 2:  # Split
                    leftdest = self.snapshot[basep + 0] + self.snapshot[basep + 1] * 256
                    rightdest = (
                        self.snapshot[basep + 2] + self.snapshot[basep + 3] * 256
                    )
                    output += (
                        f"- Split to; left = ${leftdest:X}, right = ${rightdest:X}<br/>"
                    )
                    if showlength:
                        output += "+ Total length = %d<br/>" % (totallength)
                    if follow:
                        output += "<strong>Left Split</strong><br/>"
                        output += self.decode_nibble_rle(
                            cwd, leftdest, typename, names, showlength, follow
                        )
                        output += "<strong>Right Split</strong><br/>"
                        output += self.decode_nibble_rle(
                            cwd, rightdest, typename, names, showlength, follow
                        )
                else:
                    output += "- Bad command!<br/>"
                return output
            else:
                count = (b & 0xF0) >> 4
                type_ = b & 0x0F
                if type_ == lasttype or lasttype == -1:
                    lasttype = type_
                    runlength += count
                else:
                    output += "- %s for %d units<br/>" % (names[lasttype], runlength)
                    totallength += runlength
                    runlength = count
                    lasttype = type_

    def map_curvature(self, cwd, base):
        curvemap = {
            0: "Curve Straight",
            1: "Curve Right",
            2: "Curve Right Hard",
            3: "Curve Right Very Hard",
            4: "Curve 4 XXX",
            5: "Curve 5 XXX",
            6: "Curve 6 XXX",
            7: "Curve 7 XXX",
            8: "Curve Straight (alt)",
            9: "Curve Left",
            10: "Curve Left Hard",
            11: "Curve Left Very Hard",
            12: "Curve 12 XXX",
            13: "Curve 13 XXX",
            14: "Curve 14 XXX",
            15: "Curve 15 XXX",
        }
        return self.decode_nibble_rle(
            cwd, base, "curvature", curvemap, showlength=True, follow=True
        )

    # 1/3/5/7 are used - why not the others?
    # "Up 5" and "Down 5" seem to be the most extreme used.
    # Level changes are smoothed in terms of map data?
    def map_height(self, cwd, base):
        heightmap = {
            0: "Going Up 8 XXX",
            1: "Going Up 7",
            2: "Going Up 6 XXX",
            3: "Going Up 5",
            4: "Going Up 4 XXX",
            5: "Going Up 3",
            6: "Going Up 2 XXX",
            7: "Going Up 1",  # up or down?
            8: "Level Road",
            9: "Going Down 1",
            10: "Going Down 2 XXX",
            11: "Going Down 3",
            12: "Going Down 4 XXX",
            13: "Going Down 5",
            14: "Going Down 6 XXX",
            15: "Going Down 7",
        }
        return self.decode_nibble_rle(
            cwd, base, "height", heightmap, showlength=True, follow=True
        )

    def decode_count_rle(self, cwd, base, typename, names, showlength, follow):
        output = f"Start of {typename} data at ${base:X} (byte counted)<br/>"
        runlength = 0
        lasttype = -1
        totallength = 0
        basep = base
        while 1:
            b = self.snapshot[basep]
            basep = basep + 1
            if b == 0:  # Escape
                if lasttype != -1:
                    output += "- %s for %d units<br/>" % (
                        names.get(lasttype, f"[{lasttype:2X}]"),
                        runlength,
                    )
                    totallength += runlength
                b = self.snapshot[basep]
                basep = basep + 1
                if b == 0:  # Jump
                    loopdest = self.snapshot[basep + 0] + self.snapshot[basep + 1] * 256
                    if loopdest == base:
                        output += "- Loop<br/>"
                        if showlength:
                            output += "+ Total length = %d<br/>" % (totallength)
                    else:
                        output += f"- Jump to ${loopdest:X}<br/>"
                        if showlength:
                            output += "+ Total length = %d<br/>" % (totallength)
                        if follow:
                            output += "<strong>Jump</strong><br/>"
                            output += self.decode_count_rle(
                                cwd, loopdest, typename, names, showlength, follow
                            )
                elif b == 1:  # Fork End
                    output += "- Fork End<br/>"
                elif b == 2:  # Split
                    leftdest = self.snapshot[basep + 0] + self.snapshot[basep + 1] * 256
                    rightdest = (
                        self.snapshot[basep + 2] + self.snapshot[basep + 3] * 256
                    )
                    output += (
                        f"- Split to; left = ${leftdest:X}, right = ${rightdest:X}<br/>"
                    )
                    if showlength:
                        output += "+ Total length = %d<br/>" % (totallength)
                    if follow:
                        output += "<strong>Left Split</strong><br/>"
                        output += self.decode_count_rle(
                            cwd, leftdest, typename, names, showlength, follow
                        )
                        output += "<strong>Right Split</strong><br/>"
                        output += self.decode_count_rle(
                            cwd, rightdest, typename, names, showlength, follow
                        )
                else:
                    output += "- Bad command!<br/>"
                return output
            else:
                count = b
                type_ = self.snapshot[basep]
                basep = basep + 1
                if type_ == lasttype or lasttype == -1:
                    lasttype = type_
                    runlength += count
                else:
                    output += "- %s for %d units<br/>" % (
                        names.get(lasttype, f"[{lasttype:2X}]"),
                        runlength,
                    )
                    totallength += runlength
                    runlength = count
                    lasttype = type_

    def map_lanes(self, cwd, base):
        # L/M/R is the left/middle/right alignment of the road with respect to
        # the default four-lane road.
        #
        # If not noted then the respective byte is used by Stage 1. If "Poke"
        # then it was discovered by altering the game.
        #
        #                                  [  * ] is the car's default position
        lanesmap = {
            0x00: "4 Lanes              [||||] {00}",
            0x01: "2 Lanes L            [||]   {01}",  # Poke
            0x02: "2 Lanes M             [||]  {02}",  # Poke
            0x03: "2 Lanes R              [||] {03}",  # Stage 5
            0x06: "3-2 Narrowing L      [/||]  {06}",  # Poke
            0x0D: "3-2 Narrowing X     [/||]   {0D}",  # Poke - Invalid: left side flickers
            0x0F: "3-2 Narrowing R       [/||] {0F}",
            0x1F: "2-3 Widening R        [`||] {1F}",
            0x2D: "2-3 Widening L       [`||]  {2D}",  # used in fork exit
            0x45: "Tunnel start                {45}",  # tunnels always two lanes?
            0x59: "Tunnel cont/end?            {59}",  # TBD
            0x81: "3 Lanes L            [|||]  {81}",
            0x82: "3 Lanes R             [|||] {82}",
            0x8E: "4-3 Narrowing R      [/|||] {8E}",
            0x9E: "3-4 Widening R       [`|||] {9E}",
            0xAD: "3-4 Widening L       [|||/] {AD}",
            0xBD: "4-3 Narrowing L      [|||`] {BD}",
            0xC1: "4 Lanes dirt track   [||||] {C1}",
            0xC2: "3 Lanes dirt track R  [|||] {C2}",  # Poke
            0xC3: "2 Lanes dirt track R   [||] {C3}",
        }  # Poke (stones on verge)
        return self.decode_count_rle(
            cwd, base, "lanes", lanesmap, showlength=True, follow=True
        )

    def decode_hazards(self, cwd, base, typename, names, showlength, follow):

        output = f"Start of {typename} data at ${base:X} (bytes)<br/>"
        count = 0
        totallength = 0
        basep = base
        while 1:
            b = self.snapshot[basep]
            basep = basep + 1
            if b == 0:  # Escape
                if count > 0:
                    output += "- Wait for %d units<br/>" % (count)
                    totallength += count
                b = self.snapshot[basep]
                basep = basep + 1
                if b == 0:  # Jump
                    loopdest = self.snapshot[basep + 0] + self.snapshot[basep + 1] * 256
                    if loopdest == base:
                        output += "- Loop<br/>"
                        if showlength:
                            output += "+ Total length = %d<br/>" % (totallength)
                    else:
                        output += f"- Jump to ${loopdest:X}<br/>"
                        if showlength:
                            output += "+ Total length = %d<br/>" % (totallength)
                        if follow:
                            output += "<strong>Jump</strong><br/>"
                            output += self.decode_hazards(
                                cwd, loopdest, typename, names, showlength, follow
                            )
                elif b == 1:  # Fork End
                    output += "- Fork End<br/>"
                elif b == 2:  # Split
                    leftdest = self.snapshot[basep + 0] + self.snapshot[basep + 1] * 256
                    rightdest = (
                        self.snapshot[basep + 2] + self.snapshot[basep + 3] * 256
                    )
                    output += (
                        f"- Split to; left = ${leftdest:X}, right = ${rightdest:X}<br/>"
                    )
                    if showlength:
                        output += "+ Total length = %d<br/>" % (totallength)
                    if follow:
                        output += "<strong>Left Split</strong><br/>"
                        output += self.decode_hazards(
                            cwd, leftdest, typename, names, showlength, follow
                        )
                        output += "<strong>Right Split</strong><br/>"
                        output += self.decode_hazards(
                            cwd, rightdest, typename, names, showlength, follow
                        )
                else:
                    map = {
                        3: "TBD Stop Spawning Barriers?",
                        6: "TBD Start Spawning Barriers?",
                        7: "Start Spawning Barriers Left",
                        8: "Start Spawning Barriers Right",
                        9: "Start Spawning Two Barriers",
                        10: "Set Floating Arrow Off",
                        11: "Set Floating Arrow to Left",
                        12: "Set Floating Arrow to Right",
                        13: "Enable Car Spawning",
                        14: "Disable Car Spawning",
                    }

                    output += f"- {map.get(b, f"Unknown command ${b:X}")}<br/>"

                if b <= 2:
                    return output
            else:
                count = b
                output += "- Wait for %d units<br/>" % (count)
                totallength += count
                count = 0
        return output

    def map_hazards(self, cwd, base):
        hmap = {0x00: "xxx"}
        return self.decode_hazards(
            cwd, base, "hazards", hmap, showlength=True, follow=True
        )

    def map_left_objects(self, cwd, base):
        omap = {
            0: "Nothing",
            1: "Tunnel Light",
            2: "Unknown (2)",
            3: "Short Pole",
            4: "Tree",
            5: "Bush",
            6: "Street Lamp",
            7: "Telegraph Pole",
            8: "Turn Sign, Pointing Left",
            9: "Turn Sign, Pointing Right",
            10: "Unknown (10)",
            11: "Unknown (11)",
            12: "Unknown (12)",
            13: "Unknown (13)",
            14: "Unknown (14)",
            15: "Unknown (15)",
        }
        return self.decode_nibble_rle(
            cwd, base, "left objects", omap, showlength=True, follow=True
        )

    def map_right_objects(self, cwd, base):
        omap = {
            0: "Nothing",
            1: "Tunnel Light",
            2: "Unknown (2)",
            3: "Short Pole",
            4: "Tree",
            5: "Bush",
            6: "Street Lamp",
            7: "Telegraph Pole",
            8: "Turn Sign, Pointing Left",
            9: "Turn Sign, Pointing Right",
            10: "Unknown (10)",
            11: "Unknown (11)",
            12: "Unknown (12)",
            13: "Unknown (13)",
            14: "Unknown (14)",
            15: "Unknown (15)",
        }
        return self.decode_nibble_rle(
            cwd, base, "right objects", omap, showlength=True, follow=True
        )

    def _make_empty_udg_array(self, width_udgs: int, height_udgs: int):
        return [
            [
                Udg(
                    attr=7, data=self.snapshot[32768 : 32768 + 8]
                )  # perhaps temp attr and address
                for x in range(width_udgs)
            ]
            for y in range(height_udgs)
        ]

    def _decode_snapshot_to_udgs(
        self,
        cwd,
        bitmapbase,
        attrbase,
        width,
        height,
        stride=None,
        interleaved=False,
        invert=False,
    ):
        """
        Decode snapshot memory to UDGs.

        :param int cwd: Current working directory
        :param int database: Address of bitmap data
        :param int attrbase: Address of attribute data
        :param int width: Width of graphic in pixels
        :param int height: Height of graphic in rows
        :param int stride: Stride of graphic in bytes (calculated if not given)
        :param bool interleaved: Whether graphic is stored interleaved
        :param bool invert: Whether graphic is stored inverted
        :return: (List of UDGs, Next bitmap base, Next attribute base)
        :rtype: tuple
        """

        # The first byte is mask; data comes second.
        if interleaved:
            mask_offset = 0
            data_offset = 1
            mask_bytes = 2
        else:
            mask_offset = None
            data_offset = 0
            mask_bytes = 1
        width_bytes = (width + 7) // 8
        if not stride:
            stride = width_bytes * mask_bytes
        height_udgs = (height + 7) // 8
        attr = ZX_ATTRIBUTE_BLACK_OVER_YELLOW
        udg_array = []
        for y in range(height_udgs):
            udg_array.append([])
            for x in range(width_bytes):
                addr = bitmapbase + (y * stride * 8) + (x * mask_bytes)
                udg_data = self.snapshot[
                    addr + data_offset : addr + data_offset + stride * 8 : stride
                ]
                if interleaved:
                    udg_mask = self.snapshot[
                        addr + mask_offset : addr + mask_offset + stride * 8 : stride
                    ]
                else:
                    udg_mask = None
                if attrbase:
                    attr = self.snapshot[attrbase + y * stride + x]
                udg = Udg(attr=attr, data=udg_data, mask=udg_mask)
                if invert:
                    udg.flip(flip=2)
                udg_array[-1].append(udg)
        if invert:
            udg_array.reverse()
        return (
            udg_array,
            bitmapbase + height * stride,
            attrbase + height / 8 * stride / 8 if attrbase else None,
        )

    def _build_frame(
        self, cwd, bitmapbase, attrbase, width, height, interleaved, invert, nframes
    ):
        """
        Decode snapshot memory to a static or animated graphic.

        :param int cwd: Current working directory
        :param int database: Address of bitmap data
        :param int attrbase: Address of attribute data
        :param int width: Width of graphic in pixels
        :param int height: Height of graphic in rows
        :param bool interleaved: Whether graphic is stored interleaved
        :param bool invert: Whether graphic is stored inverted
        :param int nframes: Number of frames in the animation
        :return: (Decoded image)
        :rtype: (skoolkit image)
        """
        scale = 2
        if interleaved:
            mask_type = 2
        else:
            mask_type = 0
        frames = []

        tbitmapbase = bitmapbase
        tattrbase = attrbase
        for f in range(nframes):
            (udg_array, tbitmapbase, tattrbase) = self._decode_snapshot_to_udgs(
                cwd, tbitmapbase, tattrbase, width, height, None, interleaved, invert
            )
            y = len(udg_array) * 8 - height if invert else 0
            frame = Frame(
                udg_array,
                scale=scale,
                mask=mask_type,
                y=y * scale,
                width=width * scale,
                height=height * scale,
            )
            frames.append(frame)
        if nframes == 1:
            fname = f"{{ScreenshotImagePath}}/graphic-{bitmapbase:4x}"
        else:
            fname = f"{{ScreenshotImagePath}}/anim-{bitmapbase:4x}"
        return self.handle_image(frames, fname, cwd)

    def anim(self, cwd, bitmapbase, width, height, interleaved, invert, nframes):
        """Decode an n-frame animation at the snapshot specified address."""
        return self._build_frame(
            cwd, bitmapbase, None, width, height, interleaved, invert, nframes
        )

    def graphic(self, cwd, bitmapbase, width, height, interleaved, invert):
        """Decode a static graphic at the specified snapshot address."""
        return self._build_frame(
            cwd, bitmapbase, None, width, height, interleaved, invert, nframes=1
        )

    def face(self, cwd, bitmapbase: int):
        """Decode a mugshot at the specified snapshot address."""
        return self._build_frame(
            cwd,
            bitmapbase,
            bitmapbase + (32 // 8 * 40),
            width=32,
            height=40,
            interleaved=0,
            invert=0,
            nframes=1,
        )

    def _carpart(self, cwd, partbase: int, width: int, stride: int):
        """
        Decode the hero car part at the specified snapshot address.
        """
        y_offset = self.snapshot[partbase + 0]  # to understand
        n_rows = self.snapshot[
            partbase + 1
        ]  # number of rows to use? actual graphic may be larger
        bitmapbase = self.snapshot[partbase + 2] + self.snapshot[partbase + 3] * 256
        bitmapbase += (
            y_offset * stride
        )  # not sure which direction we're offsetting from
        return (
            (width, n_rows),
            self._decode_snapshot_to_udgs(
                cwd,
                bitmapbase,
                None,
                width=width,
                height=n_rows,
                stride=stride,
                interleaved=False,
                invert=False,
            ),
        )

    def _carparts(self, cwd, partsbase: int):
        """
        Decode the set of five hero car parts at the specified snapshot address.
        """
        centrewidth = 32
        centrestride = 40 // 8
        middle = self._carpart(
            cwd, partsbase + 0 * 4, width=centrewidth, stride=centrestride
        )
        top = self._carpart(
            cwd, partsbase + 1 * 4, width=centrewidth, stride=centrestride
        )
        bottom = self._carpart(
            cwd, partsbase + 2 * 4, width=centrewidth, stride=centrestride
        )

        sidewidth = 8
        sidestride = 8 // 8
        left = self._carpart(cwd, partsbase + 3 * 4, width=sidewidth, stride=sidestride)
        right = self._carpart(
            cwd, partsbase + 4 * 4, width=sidewidth, stride=sidestride
        )

        # Get total dimensions
        width = sum(x[0][0] for x in [left, middle, right])  # pixels
        height = sum(x[0][1] for x in [top, middle, bottom])  # pixels

        return ((width, height), (middle, top, bottom, left, right))

    def herocar(self, cwd, partsbase: int):
        """
        Decode a hero car graphic from the parts list at the specified snapshot address.
        """
        scale = 2  # probably should be a global
        mask_type = 0  # might need to vary with car part
        invert = True

        dimensions, parts = self._carparts(cwd, partsbase)

        output_width, output_height = dimensions  # pixels
        width_udgs, height_udgs = (output_width // 8, output_height // 8)

        udg_array = self._make_empty_udg_array(width_udgs, height_udgs)

        x = y = 0
        for part in parts:
            for yy in range(height_udgs):
                for xx in range(width_udgs):
                    src_udg_array = part[1][0]
                    udg_array[y + yy][x + xx] = src_udg_array[0][0]

        y = len(udg_array) * 8 - output_height if invert else 0
        frame = Frame(
            udg_array,
            scale=scale,
            mask=mask_type,
            y=y * scale,
            width=output_width * scale,
            height=output_height * scale,
        )
        fname = f"{{ScreenshotImagePath}}/herocar-{partsbase:4x}"
        return self.handle_image(frame, fname, cwd)


class ChaseHQAsmWriter(AsmWriter, ChaseHQWriter):
    def timestamp(self):
        return time.strftime("%a %d %b %Y %H:%M:%S %Z")


# vim: ts=8 sts=4 sw=4 et
