# SkoolKit extension for Chase H.Q. by Ocean Software.
#
# by David Thomas, 2023
#

import string

from skoolkit.graphics import Frame, Udg
from skoolkit.skoolhtml import HtmlWriter
from skoolkit.skoolasm import AsmWriter
from skoolkit.skoolmacro import parse_ints

class ChaseHQWriter:
    pass

class ChaseHQHtmlWriter(HtmlWriter, ChaseHQWriter):
    def init(self):
        self.font = {}

    def graphic(self, cwd, addr, width, height, interleaved):
        """ Decode a graphic. """
        #udg1 = Udg(attr, data, mask)
        #udg_array = []
        #frame = Frame(udg_array, scale=2)
        #fname = '{ScreenshotImagePath}/object-%d' % addr
        return "POTATO" # self.handle_image(frame, fname, cwd)

class ChaseHQAsmWriter(AsmWriter, ChaseHQWriter):
    def timestamp(self):
        return time.strftime("%a %d %b %Y %H:%M:%S %Z")

# vim: ts=8 sts=4 sw=4 et
