#!/usr/bin/env python3
#
# CHQStage.py
#
# Chase H.Q. data decoder
#
# by dpt, June 2024
#

# Create binary using:
# skool2bin.py ChaseHQ-128K-bank-1.skool -S 49152 -E 57344
#

OBJECT_NAMES = [
        [ ],
        [
            # Stage 1
            "EMPTY",
            "TUNNEL_LIGHT",
            "(object 2 - unused)",
            "SHORT_POLE",
            "TREE",
            "BUSH",
            "STREET_LAMP",
            "TELEGRAPH_POLE",
            "TURN_SIGN_POINTING_LEFT",
            "TURN_SIGN_POINTING_RIGHT",
        ],
        [
            # Stage 2
            "EMPTY",
            "TUNNEL_LIGHT",
            "(object 2 - unused)",
            "SHORT_POLE",
            "HUGE_ROCK",
            "PALM_TREE",
            "LEAVES",
            "DOUBLE_STREET_LAMP"
        ],
        [
            # Stage 3
            "EMPTY",
            "TUNNEL_LIGHT",
            "OVERHEAD_BRIDGE",
            "SHORT_POLE",
            "TOWER_BLOCK",
            "SPEED_LIMIT_SIGN",
            "TELEGRAPH_POLE"
        ],
        [
            # Stage 4
            "EMPTY",
            "TUNNEL_LIGHT",
            "(object 2 - unused)",
            "SHORT_POLE",
            "NEAR_COLUMN",
            "FAR_COLUMN",
            "PILE_OF_ROCKS",
            "STREET_LAMP", # single lamp - same as Stage 1?
            "TURN_SIGN_POINTING_LEFT",
            "TURN_SIGN_POINTING_RIGHT"
        ],
        [
            # Stage 5
            "EMPTY",
            "TUNNEL_LIGHT",
            "OVERHEAD_BRIDGE",
            "(object 3 - unused)",
            "CACTUS",
            "DOUBLE_STREET_LAMP", # different from Stage 2?
            "HUGE_ROCK",
            "TELEGRAPH_POLE"
        ]
        ]

import getopt
import os
import sys

def usage():
    print('Usage: CHQStage.py --stage STAGE --base BASEADDR LEVELDATA')
    sys.exit(1)

def warn(message):
    print("warning: " + message, file=sys.stderr)

def main(args):

    BASE = 0x5C00  # once relocated
    LEN  = 0x2000  # 8K per level

    """ Is the address valid? (when in-place at 0x5C00) """
    def validaddr(addr):
        return addr >= BASE and addr < BASE + LEN

    """ Is the address valid? (when relocated to 0xC000 etc.) """
    def validrelocaddr(addr):
        return addr >= ORIGBASE and addr < ORIGBASE + LEN

    """ Add a block at 0x5C00+ """
    def add(blocktype, addr, desc, nbytes = -1, widthbytes = -1):
        if validaddr(addr):
            if blocktype.islower():
                prefix = "[Stage %d] " % (STAGE)
            else:
                prefix = ""
            locations.append((blocktype, addr, prefix + desc, nbytes, widthbytes))
        else:
            warn("add: $%04X is not in range [%s]" % (addr, desc))

    """ Add a relocated address """
    def addaddr(addr, desc):
        if validaddr(addr):
            dest = wordat(addr)
            relocaddr = dest - BASE + ORIGBASE
            if validrelocaddr(relocaddr):
                add("W", addr, "[$%04X] %s" % (relocaddr, desc))
            else:
                add("W", addr, "[out-of-bounds] %s" % (desc))
        else:
            warn("addaddr: $%04X is not in range [%s]" % (addr, desc))

    def byteat(addr):
        assert validaddr(addr)
        return ram[addr - BASE]

    def wordat(addr):
        return byteat(addr + 0) + byteat(addr + 1) * 256

    def topbitstring(addr):
        s = ""
        while (byteat(addr) < 0x80):
            s += chr(byteat(addr))
            addr += 1
        s += chr(byteat(addr) - 0x80)
        return s

    def topbitstrlen(addr):
        c = 0
        while (byteat(addr) < 0x80):
            assert c < 100
            c += 1
            addr += 1
        return c + 1

    def decode_nibble_rle(base, jobkind, typename, names):
        runlength = 0
        lasttype = -1
        basep = base
        commentaddr = basep
        while 1:
            oldbasep = basep

            # Find sequences of alternating pairs
            run = 2
            b0 = byteat(basep + 0)
            b1 = byteat(basep + 1)
            basep = basep + 2
            if (b0 & 0xF0) == 0x10 and (b1 & 0xF0) == 0x10:
                while True:
                    c0 = byteat(basep + 0)
                    c1 = byteat(basep + 1)
                    basep = basep + 2
                    if (c0,c1) == (b0,b1):
                        run = run + 2
                    else:
                        basep = basep - 2
                        break
            if run > 2:
                add("B", commentaddr, "Alternating (%s, %s) for %d units" % (names[b0 & 0x0F], names[b1 & 0x0F], run))
                commentaddr = basep
                lasttype = -1
                continue
            else:
                basep = oldbasep

            b = byteat(basep)
            basep = basep + 1
            if b == 0: # Escape
                if lasttype != -1:
                    add("B", commentaddr, "%s for %d units" % (names[lasttype], runlength))
                    commentaddr = basep - 1
                b = byteat(basep)
                basep = basep + 1
                if b == 0: # Jump
                    loopdest = wordat(basep)
                    if loopdest == base:
                        add("B", commentaddr, "<Esc> Loop")
                    else:
                        add("B", commentaddr, "<Esc> Jump")
                        addjob(job(jobkind, loopdest))
                    addaddr(commentaddr + 2, "Target")
                elif b == 1: # Fork End
                    add("B", commentaddr, "<Esc> Fork End")
                elif b == 2: # Split
                    leftdest  = wordat(basep + 0)
                    rightdest = wordat(basep + 2)
                    add("B", commentaddr, "<Esc> Split")
                    addaddr(commentaddr + 2, "Left target")
                    addaddr(commentaddr + 4, "Right target")
                    addjob(job(jobkind, leftdest))
                    addjob(job(jobkind, rightdest))
                else:
                    assert False
                return
            else:
                count = (b & 0xF0) >> 4
                type_ = (b & 0x0F)
                if type_ == lasttype or lasttype == -1:
                    lasttype = type_
                    runlength += count
                else:
                    add("B", commentaddr, "%s for %d units" % (names[lasttype], runlength))
                    commentaddr = basep - 1
                    runlength = count
                    lasttype = type_

    def decode_count_rle(base, jobkind, typename, names):
        runlength = 0
        lasttype = -1
        basep = base
        commentaddr = basep
        while 1:
            b = byteat(basep)
            basep = basep + 1
            if b == 0: # Escape
                if lasttype != -1:
                    add("B", commentaddr, "%s for %d units" % (names.get(lasttype, "[%2X]" % lasttype), runlength))
                    commentaddr = basep - 1
                b = byteat(basep)
                basep = basep + 1
                if b == 0: # Jump
                    loopdest = wordat(basep)
                    if loopdest == base:
                        add("B", commentaddr, "<Esc> Loop")
                    else:
                        add("B", commentaddr, "<Esc> Jump")
                        addjob(job(jobkind, loopdest))
                    addaddr(commentaddr + 2, "Target")
                elif b == 1: # Fork End
                    add("B", commentaddr, "<Esc> Fork End")
                elif b == 2: # Split
                    leftdest  = wordat(basep + 0)
                    rightdest = wordat(basep + 2)
                    add("B", commentaddr, "<Esc> Split")
                    addaddr(commentaddr + 2, "Left target")
                    addaddr(commentaddr + 4, "Right target")
                    addjob(job(jobkind, leftdest))
                    addjob(job(jobkind, rightdest))
                else:
                    assert False
                return
            else:
                count = b
                type_ = byteat(basep)
                basep = basep + 1
                if type_ == lasttype or lasttype == -1:
                    lasttype = type_
                    runlength += count
                else:
                    add("B", commentaddr, "%s for %d units" % (names.get(lasttype, "[%2X]" % lasttype), runlength))
                    commentaddr = basep - 2
                    runlength = count
                    lasttype = type_

    def decode_hazards(base, jobkind, typename, names):
        count = 0
        basep = base
        commentaddr = basep
        while 1:
            b = byteat(basep)
            basep = basep + 1
            if b == 0: # Escape
                if count > 0:
                    add("B", commentaddr, "Wait for %d units" % (count))
                    commentaddr = basep + 1
                b = byteat(basep)
                basep = basep + 1
                if b == 0: # Jump
                    loopdest = wordat(basep)
                    if loopdest == base:
                        add("B", commentaddr, "<Esc> Loop")
                    else:
                        add("B", commentaddr, "<Esc> Jump")
                        addjob(job(jobkind, loopdest))
                    addaddr(commentaddr + 2, "Target")
                elif b == 1: # Fork End
                    add("B", commentaddr, "<Esc> Fork End")
                elif b == 2: # Split
                    leftdest  = wordat(basep + 0)
                    rightdest = wordat(basep + 2)
                    add("B", commentaddr, "<Esc> Split")
                    addaddr(commentaddr + 2, "Left target")
                    addaddr(commentaddr + 4, "Right target")
                    addjob(job(jobkind, leftdest))
                    addjob(job(jobkind, rightdest))

                # 3..9 COMMAND

                elif b == 3:
                    add("B", commentaddr, "Stop Spawning Hazards")
                elif b == 4: # Used on Stages 2, 3 and 4
                    add("B", commentaddr, "Start Spawning HAZARD_1 Left")
                elif b == 5: # Used on Stages 2 and 4
                    add("B", commentaddr, "Start Spawning HAZARD_1 Right")
                elif b == 6:
                    add("B", commentaddr, "Start Spawning HAZARD_1 Both Sides")
                elif b == 7:
                    add("B", commentaddr, "Start Spawning HAZARD_2 Left")
                elif b == 8:
                    add("B", commentaddr, "Start Spawning HAZARD_2 Right")
                elif b == 9:
                    add("B", commentaddr, "Start Spawning HAZARD_2 Both Sides")

                # ARROW COMMANDS 10..12

                elif b == 10:
                    add("B", commentaddr, "Set Floating Arrow Off")
                elif b == 11:
                    add("B", commentaddr, "Set Floating Arrow to Left")
                elif b == 12:
                    add("B", commentaddr, "Set Floating Arrow to Right")

                # SPAWNING COMMAND 13..14

                elif b == 13:
                    add("B", commentaddr, "Enable Car Spawning")
                elif b == 14:
                    add("B", commentaddr, "Disable Car Spawning")

                # HELICOPTER CONTROL 15+

                elif b == 15: # Used on Stages 2 and 4
                    add("B", commentaddr, "Stop helicopter")
                # 16 not used
                elif b == 17: # Used on Stage 2 only
                    add("B", commentaddr, "Start helicopter (pilot says turn left)")
                elif b == 18: # Used on Stage 4 only
                    add("B", commentaddr, "Start helicopter (pilot says turn right)")
                else:
                    add("B", commentaddr, "Unknown command $%X" % (b))
                if b <= 2:
                    return
                commentaddr = basep
            else:
                count = b
                add("B", commentaddr, "Wait for %d units" % (count))
                commentaddr = basep
                count = 0

    def routinename(addr):
        if   addr == 0x9052: return "draw_overhead"
        elif addr == 0x916C: return "draw_stretchy_object_left"
        elif addr == 0x9171: return "draw_stretchy_object_right"
        elif addr == 0x924D: return "draw_tunnel_light_left"
        elif addr == 0x9252: return "draw_tunnel_light_right"
        elif addr == 0x9278: return "draw_object_left"
        elif addr == 0x92E1: return "draw_object_right"
        else:                return "(null)"

    def stretchyname(addr):
        if   addr == 0x7E05: return "stretchy_shortpole"
        elif addr == 0xE1E9: return "tunnellight"
        else:                return "(null)"

    class job(object):
        def __init__(self, kind, addr):
            self.kind = kind
            self.addr = addr

        def get_kind(self):
            return self.kind

        def get_addr(self):
            return self.addr

    class lod_table_job(job):
        def __init__(self, nentries, addr):
            super().__init__("lod_table", addr)
            self.nentries = nentries

        def get_nentries(self):
            return self.nentries

    def addjob(job):
        assert(type(job.kind) == type(""))
        if not job in jobs:
            jobs.append(job)
            # don't jobs.sort()

    try:
        opts,files = getopt.getopt(args, 's:b:', ['stage=', 'base='])
    except getopt.GetoptError:
        usage()

    STAGE = -1
    ORIGBASE = -1

    for o, a in opts:
        if o in ("-s", "--stage"):
            STAGE = eval(a)
        elif o in ("-b", "--base"):
            ORIGBASE = eval(a)
        else:
            assert False, "unhandled option " + o

    if STAGE < 0 or ORIGBASE < 0:
        usage()

    if len(files) != 1:
        usage()

    jobs = []
    locations = []

    # load 8K blob of stage data
    with open(files[0], mode='rb') as file:
        ram = file.read()

    # check length
    if len(ram) != 8192:
        usage()

    # insert the root entries in the job queue
    addjob(job("horizon_graphic", 0x5C00))
    addjob(job("per_stage_data", 0x5CF0))
    addjob(job("table_of_lods", 0x5D0C))
    addjob(job("difficulty", 0x5D1A))
    addjob(job("setup_data", 0x5D1D))
    addjob(job("attract_data", 0x5D2B))

    # iterate over job queue
    while jobs:
        curjob = jobs.pop(0)
        kind = curjob.get_kind()
        addr = curjob.get_addr()
        match kind:
            case "horizon_graphic":
                add("b", addr, "Horizon graphic")

            case "per_stage_data":
                add("b", addr, "Per-stage data")
                addaddr(addr + 0, "Address of perp's mugshot (attributes)")
                addaddr(addr + 2, "Address of pilot's mugshot (bitmap)")
                addaddr(addr + 4, "Screen attributes used for the ground colour (a pair of matching bytes)")
                addaddr(addr + 6, "Address of table of LODs for tumbleweeds, barriers.")
                addaddr(addr + 8, "(points at a handler address)")
                addaddr(addr + 10, "Address of right hand graphics entry/entries (-7 bytes)")
                addaddr(addr + 12, "(points at a handler address)")
                addaddr(addr + 14, "(points at a handler address)")
                addaddr(addr + 16, "Address of left hand graphics entry/entries (-7 bytes)")
                addaddr(addr + 18, "(points at a handler address)")
                addaddr(addr + 20, "Address of Nancy's perp description")
                addaddr(addr + 22, "Address of arrest messages")
                addaddr(addr + 24, "Helicopter related 1")
                addaddr(addr + 26, "Helicopter related 2")

                addjob(job("mugshot_perp",          wordat(addr + 0)))
                addjob(job("mugshot_pilot",         wordat(addr + 2)))
                addjob(job("hazard_graphics",       wordat(addr + 6)))
                addjob(job("object_graphics_right", wordat(addr + 10) + 7))
                addjob(job("object_graphics_left",  wordat(addr + 16) + 7))
                addjob(job("nancy_perp_desc",       wordat(addr + 20)))
                addjob(job("arrest_messages",       wordat(addr + 22)))
                addjob(job("helicopter_data_1",     wordat(addr + 24)))
                addjob(job("helicopter_data_2",     wordat(addr + 26)))

            case "mugshot_perp":
                realbase = addr - 4*40
                add("b", realbase, "Perp's mugshot")
                add("B", realbase, "Bitmap data for the perp's mugshot (32x40). Stored top-down.", 4*40, 4)
                add("B", addr, "Attribute data for the perp's mugshot (4x5). Stored top-down.", 4*5, 4)

            case "mugshot_pilot":
                if addr != 0:
                    add("b", addr, "Pilot's mugshot")
                    add("B", addr, "Bitmap data for the pilot's mugshot (32x40). Stored top-down.", 4*40, 4)
                    add("B", addr + 4*40, "Attribute data for the pilot's mugshot (4x5). Stored top-down.", 4*5, 4)

            case "object_graphics_right":
                objs = OBJECT_NAMES[STAGE]
                add("b", addr, "Object graphic definitions (right)")
                i = 0
                for def_addr in range(addr + 0, addr + (len(objs) - 1) * 7, 7):
                    localgraphic = False
                    arg = wordat(def_addr + 3)
                    rname = routinename(wordat(def_addr + 5))
                    if rname == "draw_stretchy_object_right":
                        if arg not in [0x7E05]:
                            addjob(job("stretchy", arg))
                            localgraphic = True
                        else:
                            localgraphic = False
                    elif rname == "draw_object_right":
                        addjob(job("draw_object_data", arg))
                        localgraphic = True

                    add("N", def_addr + 0, "Graphic definition for object %d - %s" % (i + 1, objs[i + 1]))
                    add("B", def_addr + 0, "Hit coord max")
                    add("B", def_addr + 1, "Hit coord min")
                    add("B", def_addr + 2, "?how far to push hero car away if hit")
                    if localgraphic:
                        addaddr(def_addr + 3, "Argument for routine passed in #REGde")
                    else:
                        add("W", def_addr + 3, "Argument - %s" % stretchyname(arg))
                    addaddr(def_addr + 5, "Address of routine %s" % rname)

                    i += 1

            case "object_graphics_left":
                objs = OBJECT_NAMES[STAGE]
                add("b", addr, "Object graphic definitions (left)")
                i = 0
                for def_addr in range(addr + 0, addr + (len(objs) - 1) * 7, 7):
                    localgraphic = False
                    arg = wordat(def_addr + 3)
                    rname = routinename(wordat(def_addr + 5))
                    if rname == "draw_stretchy_object_left":
                        if arg not in [0x7E05]:
                            addjob(job("stretchy", arg))
                            localgraphic = True
                        else:
                            localgraphic = False
                    elif rname == "draw_object_left":
                        addjob(job("draw_object_data", arg))
                        localgraphic = True

                    add("N", def_addr + 0, "Graphic definition for object %d - %s" % (i + 1, objs[i + 1]))
                    add("B", def_addr + 0, "Hit coord min")
                    add("B", def_addr + 1, "Hit coord max")
                    add("B", def_addr + 2, "?how far to push hero car away if hit")
                    if localgraphic:
                        addaddr(def_addr + 3, "Argument for routine passed in #REGde")
                    else:
                        add("W", def_addr + 3, "Argument - %s" % stretchyname(arg))
                    addaddr(def_addr + 5, "Address of routine %s" % rname)

                    i += 1

            case "table_of_lods":
                add("w", addr, "Table of addresses of LODs")
                addaddr(addr + 0, "Address of LOD of hazard (stone/dust?)")
                addaddr(addr + 2, "Address of LOD of hazard (stone/dust?)")
                addaddr(addr + 4, "Address of LOD of car (the perp's car)")
                addaddr(addr + 6, "Address of LOD of car (a Lambo in S1)")
                addaddr(addr + 8, "Address of LOD of car (a truck in S1)")
                addaddr(addr + 10, "Address of LOD of car (a Lambo in S1)")
                addaddr(addr + 12, "Address of LOD of car (a generic car in S1)")

                for lod_addr in range(addr + 0, addr + 14, 2):
                    lod = wordat(lod_addr)
                    if lod != 0:
                        addjob(lod_table_job(6, lod))

            case "difficulty":
                add("b", addr, "Per-stage difficulty settings")
                add("B", addr + 0, "How often cars spawn. Lower values spawn cars more often.")
                add("B", addr + 1, "Smash config parameter TBD")
                add("B", addr + 2, "Smash config parameter TBD")

            case "setup_data":
                add("w", addr, "Per-stage setup data")
                add("W", addr + 0, "road_pos")
                addaddr(addr + 2, "Address of start stretch, curvature")
                addaddr(addr + 4, "Address of start stretch, height")
                addaddr(addr + 6, "Address of start stretch, lanes")
                addaddr(addr + 8, "Address of start stretch, right-side objects")
                addaddr(addr + 10, "Address of start stretch, left-side objects")
                addaddr(addr + 12, "Address of start stretch, hazards")

                # We +1 since these map data pointers point a byte earlier than the data
                addjob(job("map_curvature", wordat(addr +  2) + 1))
                addjob(job("map_height",    wordat(addr +  4) + 1))
                addjob(job("map_lanes",     wordat(addr +  6) + 1))
                addjob(job("map_rightobjs", wordat(addr +  8) + 1))
                addjob(job("map_leftobjs",  wordat(addr + 10) + 1))
                addjob(job("map_hazards",   wordat(addr + 12) + 1))

            case "attract_data":
                add("w", addr, "Per-stage attract mode data")
                add("W", addr + 0, "road_pos")
                addaddr(addr + 2, "Address of loop section, curvature")
                addaddr(addr + 4, "Address of loop section, height")
                addaddr(addr + 6, "Address of loop section, lanes")
                addaddr(addr + 8, "Address of loop section, right-side objects")
                addaddr(addr + 10, "Address of loop section, left-side objects")
                addaddr(addr + 12, "Address of loop section, hazards")

                addjob(job("map_curvature", wordat(addr +  2) + 1))
                addjob(job("map_height",    wordat(addr +  4) + 1))
                addjob(job("map_lanes",     wordat(addr +  6) + 1))
                addjob(job("map_rightobjs", wordat(addr +  8) + 1))
                addjob(job("map_leftobjs",  wordat(addr + 10) + 1))
                addjob(job("map_hazards",   wordat(addr + 12) + 1))

            case "hazard_graphics":
                add("b", addr, "Hittable hazards")
                for _ in range(0,2):
                    add("B", addr + 0, "?id")
                    addaddr(addr + 1, "Address of LODs")
                    addjob(lod_table_job(5, wordat(addr + 1)))  # CHECK
                    addr += 3

            case "nancy_perp_desc":
                who = byteat(addr)
                add("b", addr, "Nancy's perp description")
                add("B", addr, "Character identifier (0/1/2/3 = Pilot/Nancy/Raymond/Tony)")
                addr += 1
                while 1:
                    byte = byteat(addr)
                    if byte == 0xFE: # jump
                        # TODO final pointer always random choice?
                        add("B", addr, "Escape: Jump")
                        add("W", addr + 1, "Address of next message (always $98BD)")
                        break
                    addaddr(addr, "Perp description pointer")
                    addjob(job("string", wordat(addr)))
                    addr += 2

            case "string": # top bit terminated
                decodedstring = topbitstring(addr)
                add("T", addr, "\"%s\"" % decodedstring)

            case "arrest_messages":
                add("b", addr, "Arrest messages")
                add("B", addr, "?frame delay until first message")
                addr += 1
                for _ in range(1,999):
                    flags = byteat(addr + 1)
                    if flags != 0:
                        add("B", addr + 0, "?frame delay until next message")
                        add("B", addr + 1, "Flags")
                        add("B", addr + 2, "Attribute")
                        add("W", addr + 3, "Back buffer address")
                        add("W", addr + 5, "Attribute address")
                        addr += 7
                        addjob(job("string", addr))
                        addr += topbitstrlen(addr)
                    else:
                        add("B", addr + 0, "?frame delay until next message")
                        add("B", addr + 1, "Stop")
                        break

            case "helicopter_data_1":
                if addr != 0:
                    add("b", addr, "Helicopter data 1")
                    for i in range(0,6):
                        addaddr(addr, "ptr")
                        w = wordat(addr)
                        addr += 2
                        add("B", w, "Pointed to by helicopter data 1")
                        lod = w + 2
                        if i == 5: lod = w + 1  # bodge to get shadows pointed at
                        addjob(job("lod", lod))

            case "helicopter_data_2":
                if addr != 0x000C:
                    add("b", addr, "Helicopter data 2")
                    for i in range(0,6):
                        addaddr(addr, "ptr")
                        w = wordat(addr)
                        addr += 2
                        add("B", w, "Pointed to by helicopter data 2")
                        lod = w + 2
                        if i == 5: lod = w + 1  # bodge to get shadows pointed at
                        addjob(job("lod", lod))

            case "map_curvature":
                add("b", addr, "Map curvature data")
                curvemap = {0: "Curve Straight",
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
                            15: "Curve 15 XXX"}
                decode_nibble_rle(addr, kind, "curvature", curvemap)

            case "map_height":
                add("b", addr, "Map height data")
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
                decode_nibble_rle(addr, kind, "height", heightmap)

            case "map_rightobjs":
                add("b", addr, "Map right object data")
                decode_nibble_rle(addr, kind, "right objects", OBJECT_NAMES[STAGE])

            case "map_leftobjs":
                add("b", addr, "Map left object data")
                decode_nibble_rle(addr, kind, "left objects", OBJECT_NAMES[STAGE])

            case "map_lanes":
                add("b", addr, "Map lanes data")
                # L/M/R is the left/middle/right alignment of the road with respect to
                # the default four-lane road.
                #
                # If not noted then the respective byte is used by Stage 1. If "Poke"
                # then it was discovered by altering the game.
                #
                #                                  [  * ] is the car's default position
                lanesmap = {0x00: r'4 Lanes              [||||] {00}',
                            0x01: r'2 Lanes L            [||]   {01}', # Poke
                            0x02: r'2 Lanes M             [||]  {02}', # Poke
                            0x03: r'2 Lanes R              [||] {03}', # Stage 5
                            0x06: r'3-2 Narrowing L      [/||]  {06}', # Poke
                            0x0D: r'3-2 Narrowing X     [/||]   {0D}', # Poke - Invalid: left side flickers
                            0x0F: r'3-2 Narrowing R       [/||] {0F}',
                            0x1F: r'2-3 Widening R        [\||] {1F}',
                            0x2D: r'2-3 Widening L       [\||]  {2D}', # used in fork exit
                            0x45: r'Tunnel start                {45}', # tunnels always two lanes?
                            0x59: r'Tunnel cont/end?            {59}', # TBD
                            0x81: r'3 Lanes L            [|||]  {81}',
                            0x82: r'3 Lanes R             [|||] {82}',
                            0x8E: r'4-3 Narrowing R      [/|||] {8E}',
                            0x9E: r'3-4 Widening R       [\|||] {9E}',
                            0xAD: r'3-4 Widening L       [|||/] {AD}',
                            0xBD: r'4-3 Narrowing L      [|||\] {BD}',
                            0xC1: r'4 Lanes dirt track   [||||] {C1}',
                            0xC2: r'3 Lanes dirt track R  [|||] {C2}', # Poke
                            0xC3: r'2 Lanes dirt track R   [||] {C3}'} # Poke (stones on verge)
                decode_count_rle(addr, kind, "lanes", lanesmap)

            case "map_hazards":
                add("b", addr, "Map hazards data")
                hmap = {0x00: "xxx"}
                decode_hazards(addr, kind, "hazards", hmap)

            case "lod":
                add("N", addr + 0, "LOD")
                add("B", addr + 0, "Width (bytes)")
                add("B", addr + 1, "Flags")
                add("B", addr + 2, "Height (pixels)")
                addaddr(addr + 3, "Bitmap address")
                addaddr(addr + 5, "Pre-shifted bitmap address")

                widthbytes    = byteat(addr + 0)
                flags         = byteat(addr + 1)
                height        = byteat(addr + 2)
                bitmap        = wordat(addr + 3)
                shiftedbitmap = wordat(addr + 5)
                nbytes        = widthbytes * height

                masked = ""
                if flags & 1:
                    widthbytes *= 2
                    nbytes *= 2
                    masked = "(masked) "

                add("B", bitmap, "Bitmap data %s%d bytes x %d" % (masked, widthbytes, height), nbytes, widthbytes)
                if bitmap != shiftedbitmap:
                    add("B", shiftedbitmap, "Pre-shifted bitmap data %s%d bytes x %d" % (masked, widthbytes, height), nbytes, widthbytes)

            case "lod_table":
                nentries = curjob.get_nentries()
                add("N", addr, "LOD table")
                for _ in range(0,nentries):
                    addjob(job("lod", addr))
                    addr += 7

            case "stretchy":
                add("N", addr, "Stretchy graphic")
                while 1:
                    b = byteat(addr)
                    if b == 1:
                        add("B", addr, "Terminator")
                        break
                    else:
                        add("B", addr, "?index")
                    
                    addaddr(addr + 1, "pointer to ?")

                    sgp = wordat(addr + 1)
                    if validaddr(sgp):
                        addjob(job("stretchy_graphic_part", sgp))

                    addr += 3

            case "stretchy_graphic_part":
                add("N", addr, "Stretchy graphic part")
                addaddr(addr, "LOD ptr")
                addjob(lod_table_job(5, wordat(addr + 0)))  # probably just 5 long?
                addr += 2
                for i in range(0,10):  # always ten?
                    add("W", addr + 0, "TBD")  # ought to be a pair?
                    addr += 2

            case "draw_object_data":
                add("N", addr, "draw_object_left/right graphic data")
                addaddr(addr, "LOD ptr")
                addjob(lod_table_job(5, wordat(addr + 0)))  # probably just 5 long?
                addr += 2
                for i in range(0,10):  # always ten?
                    add("W", addr + 0, "TBD")  # ought to be a pair?
                    addr += 2

            case _:
                warn("Unhandled: " + kind)

    # sort locations
    locations.sort(key=lambda entry: entry[1])

    # at end dump all the locations out (correcting the addresses)
    for loc in locations:
        blocktype, addr, desc, nbytes, widthbytes = loc
        nbytesstr = ""
        if nbytes >= 0:
            nbytesstr = ",%d" % (nbytes)

        widthstr = ""
        if widthbytes < 0:
            if blocktype == "B":
                widthstr = ",1"
            elif blocktype == "W":
                widthstr = ",2"
        else:
            widthstr = ",%d" % widthbytes
        print("%s $%04X%s%s %s" % (blocktype, addr + ORIGBASE - BASE, nbytesstr, widthstr, desc))

if __name__ == '__main__':
    main(sys.argv[1:])
