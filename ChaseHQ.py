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
    pass

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
