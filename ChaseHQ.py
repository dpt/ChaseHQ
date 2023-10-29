# SkoolKit extension for Chase H.Q. by Ocean Software.
#
# by David Thomas, 2023
#

import string

from skoolkit.graphics import Frame, Udg
from skoolkit.skoolhtml import HtmlWriter
from skoolkit.skoolasm import AsmWriter
from skoolkit.skoolmacro import parse_ints

ZX_ATTRIBUTE_BLACK_OVER_YELLOW = 48

class ChaseHQWriter:
    def decode_nibble_rle(self, cwd, base, typename, names, showlength, follow):
        output = "Start of %s data at $%X (nibble counted)<br/>" % (typename, base)
        runlength = 0
        lasttype = -1
        totallength = 0
        basep = base
        while 1:
            b = self.snapshot[basep]
            basep = basep + 1
            if b == 0: # Escape
                if lasttype != -1:
                    output += "- %s for %d units<br/>" % (names[lasttype], runlength)
                    totallength += runlength
                b = self.snapshot[basep]
                basep = basep + 1
                if b == 0: # Jump
                    loopdest = self.snapshot[basep + 0] + self.snapshot[basep + 1] * 256
                    if loopdest == base:
                        output += "- Loop<br/>"
                        if showlength:
                            output += "+ Total length = %d<br/>" % (totallength)
                    else:
                        output += "- Jump to $%X<br/>" % (loopdest)
                        if showlength:
                            output += "+ Total length = %d<br/>" % (totallength)
                        if follow:
                            output += "<strong>Jump</strong><br/>"
                            output += self.decode_nibble_rle(cwd, loopdest, typename, names, showlength, follow)
                elif b == 1: # TBD
                    output += "- TBD<br/>"
                elif b == 2: # Split
                    leftdest  = self.snapshot[basep + 0] + self.snapshot[basep + 1] * 256
                    rightdest = self.snapshot[basep + 2] + self.snapshot[basep + 3] * 256
                    output += "- Split to; left = $%X, right = $%X<br/>" % (leftdest, rightdest)
                    if showlength:
                        output += "+ Total length = %d<br/>" % (totallength)
                    if follow:
                        output += "<strong>Left Split</strong><br/>"
                        output += self.decode_nibble_rle(cwd, leftdest, typename, names, showlength, follow)
                        output += "<strong>Right Split</strong><br/>"
                        output += self.decode_nibble_rle(cwd, rightdest, typename, names, showlength, follow)
                else:
                    output += "- Bad command!<br/>"
                return output
            else:
                count = (b & 0xF0) >> 4
                type_ = (b & 0x0F)
                if type_ == lasttype or lasttype == -1:
                    lasttype = type_
                    runlength += count
                else:
                    output += "- %s for %d units<br/>" % (names[lasttype], runlength)
                    totallength += runlength
                    runlength = count
                    lasttype = type_

    def map_curvature(self, cwd, base):
        curvemap = {0: "Curve Straight",
                    1: "Curve Right",
                    2: "Curve Hard Right",
                    3: "Curve Very Hard Right",
                    4: "Curve 4 XXX",
                    5: "Curve 5 XXX",
                    6: "Curve 6 XXX",
                    7: "Curve 7 XXX",
                    8: "Curve Straight (alt)",
                    9: "Curve Left",
                    10: "Curve Hard Left",
                    11: "Curve Very Hard Left",
                    12: "Curve 12 XXX",
                    13: "Curve 13 XXX",
                    14: "Curve 14 XXX",
                    15: "Curve 15 XXX"}
        return self.decode_nibble_rle(cwd, base, "curvature", curvemap, showlength=True, follow=True)

# 1/3/5/7 are used - why not the others?
# "Up 5" and "Down 5" seem to be the most extreme used.
# Level changes are smoothed in terms of map data?
    def map_height(self, cwd, base):
        heightmap = {0: "Going Up 8 XXX",
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
                     15: "Going Down 7"}
        return self.decode_nibble_rle(cwd, base, "height", heightmap, showlength=True, follow=True)

    def decode_count_rle(self, cwd, base, typename, names, showlength, follow):
        output = "Start of %s data at $%X (byte counted)<br/>" % (typename, base)
        runlength = 0
        lasttype = -1
        totallength = 0
        basep = base
        while 1:
            b = self.snapshot[basep]
            basep = basep + 1
            if b == 0: # Escape
                if lasttype != -1:
                    output += "- %s for %d units<br/>" % (names.get(lasttype, "[%2X]" % lasttype), runlength)
                    totallength += runlength
                b = self.snapshot[basep]
                basep = basep + 1
                if b == 0: # Jump
                    loopdest = self.snapshot[basep + 0] + self.snapshot[basep + 1] * 256
                    if loopdest == base:
                        output += "- Loop<br/>"
                        if showlength:
                            output += "+ Total length = %d<br/>" % (totallength)
                    else:
                        output += "- Jump to $%X<br/>" % (loopdest)
                        if showlength:
                            output += "+ Total length = %d<br/>" % (totallength)
                        if follow:
                            output += "<strong>Jump</strong><br/>"
                            output += self.decode_count_rle(cwd, loopdest, typename, names, showlength, follow)
                elif b == 1: # TBD
                    output += "- TBD<br/>"
                elif b == 2: # Split
                    leftdest  = self.snapshot[basep + 0] + self.snapshot[basep + 1] * 256
                    rightdest = self.snapshot[basep + 2] + self.snapshot[basep + 3] * 256
                    output += "- Split to; left = $%X, right = $%X<br/>" % (leftdest, rightdest)
                    if showlength:
                        output += "+ Total length = %d<br/>" % (totallength)
                    if follow:
                        output += "<strong>Left Split</strong><br/>"
                        output += self.decode_count_rle(cwd, leftdest, typename, names, showlength, follow)
                        output += "<strong>Right Split</strong><br/>"
                        output += self.decode_count_rle(cwd, rightdest, typename, names, showlength, follow)
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
                    output += "- %s for %d units<br/>" % (names.get(lasttype, "[%2X]" % lasttype), runlength)
                    totallength += runlength
                    runlength = count
                    lasttype = type_

    def map_lanes(self, cwd, base):
        lanesmap = {0x00: "Four lanes      [||||] {00}",
                    0x45: "Tunnel start           {45}",
                    0x59: "Tunnel cont/end?       {59}",
                    0x81: "Three lanes     [|||_] {81}",
                    0x82: "Three lanes     [_|||] {82}",
                    0x8E: "Narrowing 4-3 L [/|||] {8E}",
                    0x9E: "Widening 3-4 L  [\|||] {9E}",
                    0xAD: "Widening 3-4 R  [|||/] {AD}",
                    0xBD: "Narrowing 4-3 R [|||\] {BD}",
                    0xC1: "Dirt track             {C1}"}
        return self.decode_count_rle(cwd, base, "lanes", lanesmap, showlength=True, follow=True)

    def decode_hazards(self, cwd, base, typename, names, showlength, follow):
        output = "Start of %s data at $%X (bytes)<br/>" % (typename, base)
        count = 0
        totallength = 0
        basep = base
        while 1:
            b = self.snapshot[basep]
            basep = basep + 1
            if b == 0: # Escape
                if count > 0:
                    output += "- Wait for %d units<br/>" % (count)
                    totallength += count
                b = self.snapshot[basep]
                basep = basep + 1
                if b == 0: # Jump
                    loopdest = self.snapshot[basep + 0] + self.snapshot[basep + 1] * 256
                    if loopdest == base:
                        output += "- Loop<br/>"
                        if showlength:
                            output += "+ Total length = %d<br/>" % (totallength)
                    else:
                        output += "- Jump to $%X<br/>" % (loopdest)
                        if showlength:
                            output += "+ Total length = %d<br/>" % (totallength)
                        if follow:
                            output += "<strong>Jump</strong><br/>"
                            output += self.decode_hazards(cwd, loopdest, typename, names, showlength, follow)
                elif b == 1: # TBD
                    output += "- TBD<br/>"
                elif b == 2: # Split
                    leftdest  = self.snapshot[basep + 0] + self.snapshot[basep + 1] * 256
                    rightdest = self.snapshot[basep + 2] + self.snapshot[basep + 3] * 256
                    output += "- Split to; left = $%X, right = $%X<br/>" % (leftdest, rightdest)
                    if showlength:
                        output += "+ Total length = %d<br/>" % (totallength)
                    if follow:
                        output += "<strong>Left Split</strong><br/>"
                        output += self.decode_hazards(cwd, leftdest, typename, names, showlength, follow)
                        output += "<strong>Right Split</strong><br/>"
                        output += self.decode_hazards(cwd, rightdest, typename, names, showlength, follow)
                elif b == 3: # TBD Stop Spawning Barriers?
                    output += "- Stop Spawning Barriers 3?<br/>"
                elif b == 6: # TBD Stop Spawning Barriers?
                    output += "- Stop Spawning Barriers 6?<br/>"
                elif b == 7: # Start Spawning Barriers Left
                    output += "- Start Spawning Barriers Left<br/>"
                elif b == 8: # Start Spawning Barriers Right
                    output += "- Start Spawning Barriers Right<br/>"
                elif b == 9: # Start Spawning Two Barriers
                    output += "- Start Spawning Two Barriers<br/>"
                elif b == 10: # Set Floating Arrow Off
                    output += "- Set Floating Arrow Off<br/>"
                elif b == 11: # Set Floating Arrow to Left
                    output += "- Set Floating Arrow to Left<br/>"
                elif b == 12: # Set Floating Arrow to Right
                    output += "- Set Floating Arrow to Right<br/>"
                elif b == 13: # Enable Car Spawning
                    output += "- Enable Car Spawning<br/>"
                elif b == 14: # Disable Car Spawning
                    output += "- Disable Car Spawning<br/>"
                else:
                    output += "- Unknown command %X<br/>" % (b)
                if b <= 2:
                    return output
            else:
                count = b
                output += "- Wait for %d units<br/>" % (count)
                totallength += count
                count = 0

    def map_hazards(self, cwd, base):
        hmap = {0x00: "xxx"}
        return self.decode_hazards(cwd, base, "hazards", hmap, showlength=True, follow=True)

    def map_left_objects(self, cwd, base):
        omap = {0: "Nothing",
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
                15: "Unknown (15)"}
        return self.decode_nibble_rle(cwd, base, "left objects", omap, showlength=True, follow=True)

    def map_right_objects(self, cwd, base):
        omap = {0: "Nothing",
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
                15: "Unknown (15)"}
        return self.decode_nibble_rle(cwd, base, "right objects", omap, showlength=True, follow=True)

class ChaseHQHtmlWriter(HtmlWriter, ChaseHQWriter):
    def init(self):
        self.font = {}

    def _decode(self, cwd, database, attrbase, width, height, interleaved, invert):
        """ Decode a graphic. """

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
        stride = width_bytes * mask_bytes
        height_udgs = (height + 7) // 8
        attr = ZX_ATTRIBUTE_BLACK_OVER_YELLOW
        udg_array = []
        for y in range(height_udgs):
            udg_array.append([])
            for x in range(width_bytes):
                addr = database + (y*stride*8) + (x*mask_bytes)
                udg_data = self.snapshot[addr+data_offset: addr+data_offset+stride*8: stride]
                if interleaved:
                    udg_mask = self.snapshot[addr+mask_offset: addr+mask_offset+stride*8: stride]
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
        return (udg_array, database + height * stride, attrbase + height / 8 * stride / 8 if attrbase else None)

    def _generate(self, cwd, database, attrbase, width, height, interleaved, invert, nframes):
        scale = 2
        if interleaved:
            mask_type = 2
        else:
            mask_type = 0
        frames = []

        tdatabase = database
        tattrbase = attrbase
        for f in range(nframes):
            (udg_array, tdatabase, tattrbase) = self._decode(cwd, tdatabase, tattrbase, width, height, interleaved, invert)
            y = len(udg_array) * 8 - height if invert else 0
            frame = Frame(udg_array, scale=scale, mask=mask_type, y=y*scale, width=width*scale, height=height*scale)
            frames.append(frame)
        if nframes == 1:
            fname = '{ScreenshotImagePath}/graphic-%4x' % database
        else:
            fname = '{ScreenshotImagePath}/anim-%4x' % database
        return self.handle_image(frames, fname, cwd)

    def anim(self, cwd, database, width, height, interleaved, invert, nframes):
        return self._generate(cwd, database, None, width, height, interleaved, invert, nframes)

    def graphic(self, cwd, database, width, height, interleaved, invert):
        return self._generate(cwd, database, None, width, height, interleaved, invert, nframes=1)

    def face(self, cwd, database):
        return self._generate(cwd, database, database + (32 // 8 * 40), width=32, height=40, interleaved=0, invert=0, nframes=1)

class ChaseHQAsmWriter(AsmWriter, ChaseHQWriter):
    def timestamp(self):
        return time.strftime("%a %d %b %Y %H:%M:%S %Z")

# vim: ts=8 sts=4 sw=4 et
