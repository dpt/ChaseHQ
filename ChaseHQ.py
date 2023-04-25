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

    def graphic(self, cwd, baseaddr, width, height, interleaved):
        """ Decode a graphic. """
        # The first byte is mask; data comes second.
        scale = 2
        if interleaved:
            mask_offset = 0
            data_offset = 1
            mask_type = 2
            mask_bytes = 2
        else:
            mask_offset = None
            data_offset = 0
            mask_type = 0
            mask_bytes = 1
        width_bytes = (width + 7) // 8
        stride = width_bytes * mask_bytes
        height_udgs = (height + 7) // 8
        attr = ZX_ATTRIBUTE_BLACK_OVER_YELLOW
        udg_array = []
        for y in range(height_udgs):
            udg_array.append([])
            for x in range(width_bytes):
                addr = baseaddr + (y*stride*8) + (x*mask_bytes)
                udg_data = self.snapshot[addr+data_offset: addr+data_offset+stride*8: stride]
                if interleaved:
                    udg_mask = self.snapshot[addr+mask_offset: addr+mask_offset+stride*8: stride]
                else:
                    udg_mask = None
                udg = Udg(attr=attr, data=udg_data, mask=udg_mask)
                udg.flip(flip=2)
                udg_array[-1].append(udg)
        udg_array.reverse()
        y = height_udgs * 8 - height
        frame = Frame(udg_array, scale=scale, mask=mask_type, y=y*scale, width=width*scale, height=height*scale)
        fname = '{ScreenshotImagePath}/object-%d' % baseaddr
        return self.handle_image(frame, fname, cwd)

class ChaseHQAsmWriter(AsmWriter, ChaseHQWriter):
    def timestamp(self):
        return time.strftime("%a %d %b %Y %H:%M:%S %Z")

# vim: ts=8 sts=4 sw=4 et
