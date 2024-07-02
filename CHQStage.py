#!/usr/bin/env python3
#
# CHQStage.py
#
# Chase H.Q. data decoder
#

# Create binary using:
# skool2bin.py ChaseHQ-128K-bank-1.skool -S 49152 -E 57344
#

# TODO
# - per-stage object names

OBJNAME = [
        [ ],
        [
            # STAGE 1
            "(nothing)",
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
            # STAGE 2
            "(nothing)",
            "TUNNEL_LIGHT",
            "(object 2 - unused)",
            "SHORT_POLE",
            "HUGE_ROCK",
            "PALM_TREE",
            "LEAVES",
            "DOUBLE_STREET_LAMP"
        ],
        [
            # STAGE 3
            "(nothing)",
            "TUNNEL_LIGHT",
            "OVERHEAD_BRIDGE",
            "SHORT_POLE",
            "TOWER_BLOCK",
            "SPEED_LIMIT_SIGN",
            "TELEGRAPH_POLE"
        ],
        [
            # STAGE 4
            "(nothing)",
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
            # STAGE 5
            "(nothing)",
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

def main(args):

    BASE = 0x5C00  # once relocated
    LEN  = 0x2000  # 8K per level

    """ Add a block at 0x5C00+ """
    def add(blocktype, addr, desc, nbytes = -1, widthbytes = -1):
        if BASE <= addr < BASE + LEN:
            if blocktype.islower():
                prefix = "[Stage %d] " % (STAGE)
            else:
                prefix = ""
            locations.append((blocktype, addr, prefix + desc, nbytes, widthbytes))
        else:
            # error
            print(hex(addr), desc)
            assert False

    """ Add a relocated address """
    def addaddr(addr, desc):
        dest = wordat(addr)
        add("W", addr, "[$%4X] %s" % (dest - BASE + 0xC000, desc))

    def addjob(kind, addr):
        if not (kind, addr) in jobs:
            jobs.append((kind, addr))

    def byteat(addr):
        assert BASE <= addr < BASE + LEN
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

    def decode_nibble_rle(base, jobname, typename, names):
        runlength = 0
        lasttype = -1
        basep = base
        commentaddr = basep
        while 1:
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
                        addjob(jobname, loopdest)
                    addaddr(commentaddr + 2, "Target")
                elif b == 1: # Fork End
                    add("B", commentaddr, "<Esc> Fork End")
                elif b == 2: # Split
                    leftdest  = wordat(basep + 0)
                    rightdest = wordat(basep + 2)
                    add("B", commentaddr, "<Esc> Split")
                    addaddr(commentaddr + 2, "Left target")
                    addaddr(commentaddr + 4, "Right target")
                    addjob(jobname, leftdest)
                    addjob(jobname, rightdest)
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

    def decode_count_rle(base, jobname, typename, names):
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
                        addjob(jobname, loopdest)
                    addaddr(commentaddr + 2, "Target")
                elif b == 1: # Fork End
                    add("B", commentaddr, "<Esc> Fork End")
                elif b == 2: # Split
                    leftdest  = wordat(basep + 0)
                    rightdest = wordat(basep + 2)
                    add("B", commentaddr, "<Esc> Split")
                    addaddr(commentaddr + 2, "Left target")
                    addaddr(commentaddr + 4, "Right target")
                    addjob(jobname, leftdest)
                    addjob(jobname, rightdest)
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

    def decode_hazards(base, jobname, typename, names):
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
                        addjob(jobname, loopdest)
                    addaddr(commentaddr + 2, "Target")
                elif b == 1: # Fork End
                    add("B", commentaddr, "<Esc> Fork End")
                elif b == 2: # Split
                    leftdest  = wordat(basep + 0)
                    rightdest = wordat(basep + 2)
                    add("B", commentaddr, "<Esc> Split")
                    addaddr(commentaddr + 2, "Left target")
                    addaddr(commentaddr + 4, "Right target")
                    addjob(jobname, leftdest)
                    addjob(jobname, rightdest)
                elif b == 3: # TBD Stop Spawning Barriers?
                    add("B", commentaddr, "Stop Spawning Barriers 3?")
                elif b == 6: # TBD Stop Spawning Barriers?
                    add("B", commentaddr, "Stop Spawning Barriers 6?")
                elif b == 7: # Start Spawning Barriers Left
                    add("B", commentaddr, "Start Spawning Barriers Left")
                elif b == 8: # Start Spawning Barriers Right
                    add("B", commentaddr, "Start Spawning Barriers Right")
                elif b == 9: # Start Spawning Two Barriers
                    add("B", commentaddr, "Start Spawning Two Barriers")
                elif b == 10: # Set Floating Arrow Off
                    add("B", commentaddr, "Set Floating Arrow Off")
                elif b == 11: # Set Floating Arrow to Left
                    add("B", commentaddr, "Set Floating Arrow to Left")
                elif b == 12: # Set Floating Arrow to Right
                    add("B", commentaddr, "Set Floating Arrow to Right")
                elif b == 13: # Enable Car Spawning
                    add("B", commentaddr, "Enable Car Spawning")
                elif b == 14: # Disable Car Spawning
                    add("B", commentaddr, "Disable Car Spawning")
                else:
                    add("B", commentaddr, "Unknown command %X" % (b))
                if b <= 2:
                    return
                commentaddr = basep
            else:
                count = b
                add("B", commentaddr, "Wait for %d units" % (count))
                commentaddr = basep
                count = 0

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
    jobs.append(("horizon_graphic", 0x5C00))
    jobs.append(("per_stage_data", 0x5CF0))
    jobs.append(("table_of_lods", 0x5D0C))
    jobs.append(("difficulty", 0x5D1A))
    jobs.append(("setup_data", 0x5D1D))
    jobs.append(("attract_data", 0x5D2B))

    # iterate over job queue
    while jobs:
        job,addr = jobs.pop(0)
        match job:
            case "horizon_graphic":
                add("b", addr, "Horizon graphic")

            case "per_stage_data":
                add("b", addr, "Per-stage data")
                addaddr(addr + 0, "Address of perp's mugshot attributes")
                addaddr(addr + 2, "Address of perp's mugshot bitmap")
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

                addjob("hittable_objects", wordat(addr + 6))
                addjob("graphics_defs",    wordat(addr + 10) + 7)
                addjob("graphics_defs",    wordat(addr + 16) + 7)
                addjob("nancy_perp_desc",  wordat(addr + 20))
                addjob("arrest_messages",  wordat(addr + 22))

            case "graphics_defs":
                add("b", addr, "Graphic definition")
                for def_addr in range(addr + 0, addr + 7*7, 7): # seems to be 7 per set
                    add("N", def_addr + 0, "Definition")
                    add("B", def_addr + 0, "Hit coord max/min (R/L)")
                    add("B", def_addr + 1, "Hit coord min/max (R/L)")
                    add("B", def_addr + 2, "?how far to push hero car away if hit")
                    add("W", def_addr + 3, "Argument for routine passed in #REGde")

                    rout = wordat(def_addr + 5)
                    if   rout == 0x9052: routname = "draw_overhead"
                    elif rout == 0x916C: routname = "draw_stretchy_object_left"
                    elif rout == 0x9171: routname = "draw_stretchy_object_right"
                    elif rout == 0x924D: routname = "draw_tunnel_light_left"
                    elif rout == 0x9252: routname = "draw_tunnel_light_right"
                    else:                routname = "TBD"

                    add("W", def_addr + 5, "Address of routine %s" % routname)

            case "table_of_lods":
                add("w", addr, "Table of addresses of LODs")
                addaddr(addr + 0, "Address of LOD of stone/dust?")
                addaddr(addr + 2, "Address of LOD of stone/dust?")
                addaddr(addr + 4, "Address of LOD of car (the perp's car)")
                addaddr(addr + 6, "Address of LOD of car (a Lambo in S1)")
                addaddr(addr + 8, "Address of LOD of car (a truck in S1)")
                addaddr(addr + 10, "Address of LOD of car (a Lambo in S1)")
                addaddr(addr + 12, "Address of LOD of car (a generic car in S1)")

                for lod_addr in range(addr + 0, addr + 14, 2):
                    lod = wordat(lod_addr)
                    if lod > 0:
                        addjob("lod", lod)

            case "difficulty":
                add("b", addr, "Per-stage difficulty settings")
                add("B", addr + 0, "How often cars spawn. Lower values spawn cars more often.")
                add("B", addr + 1, "Smash related parameter")
                add("B", addr + 2, "Smash related parameter")

            case "setup_data":
                add("w", addr, "Per-stage setup data")
                add("W", addr + 0, "road_pos")
                addaddr(addr + 2, "Address of start stretch, curvature")
                addaddr(addr + 4, "Address of start stretch, height")
                addaddr(addr + 6, "Address of start stretch, lanes")
                addaddr(addr + 8, "Address of start stretch, right-side objects")
                addaddr(addr + 10, "Address of start stretch, left-side objects")
                addaddr(addr + 12, "Address of start stretch, hazards")

                # +1 since these map data pointers point a byte earlier than the data
                addjob("map_curvature", wordat(addr +  2) + 1)
                addjob("map_height",    wordat(addr +  4) + 1)
                addjob("map_lanes",     wordat(addr +  6) + 1)
                addjob("map_rightobjs", wordat(addr +  8) + 1)
                addjob("map_leftobjs",  wordat(addr + 10) + 1)
                addjob("map_hazards",   wordat(addr + 12) + 1)

            case "attract_data":
                add("w", addr, "Per-stage attract mode data")
                add("W", addr + 0, "road_pos")
                addaddr(addr + 2, "Address of loop section, curvature")
                addaddr(addr + 4, "Address of loop section, height")
                addaddr(addr + 6, "Address of loop section, lanes")
                addaddr(addr + 8, "Address of loop section, right-side objects")
                addaddr(addr + 10, "Address of loop section, left-side objects")
                addaddr(addr + 12, "Address of loop section, hazards")

                addjob("map_curvature", wordat(addr +  2) + 1)
                addjob("map_height",    wordat(addr +  4) + 1)
                addjob("map_lanes",     wordat(addr +  6) + 1)
                addjob("map_rightobjs", wordat(addr +  8) + 1)
                addjob("map_leftobjs",  wordat(addr + 10) + 1)
                addjob("map_hazards",   wordat(addr + 12) + 1)

            case "hittable_objects":
                add("b", addr, "Hittable hazards")
                for _ in range(0,2):
                    add("B", addr + 0, "?id")
                    addaddr(addr + 1, "Address of LODs")
                    addjob("lod", wordat(addr + 1))
                    addr += 3

            case "nancy_perp_desc":
                who = byteat(addr)
                add("b", addr, "Nancy's perp description")
                add("B", addr, "Character identifier (0/1/2/3 = Pilot/Nancy/Raymond/Tony)")
                addr += 1
                while 1:
                    desc = wordat(addr)
                    if desc & 0xFF == 0xFE:  # unclear
                        add("B", addr, "terminator?")
                        break
                    addaddr(addr, "Perp description pointer")
                    addjob("string", wordat(addr))
                    addr += 2
                # TODO final pointer always random choice?

            case "string": # top bit terminated
                decodedstring = topbitstring(addr)
                add("T", addr, "\"%s\"" % decodedstring)

            case "arrest_messages":
                add("b", addr, "Arrest messages")
                add("B", addr, "?frame delay until first message")
                addr += 1
                for _ in range(1,999):
                    flags = byteat(addr + 1)
                    if flags > 0:
                        add("B", addr + 0, "?frame delay until next message")
                        add("B", addr + 1, "Flags")
                        add("B", addr + 2, "Attribute")
                        add("W", addr + 3, "Back buffer address")
                        add("W", addr + 5, "Attribute address")
                        addr += 7
                        addjob("string", addr)
                        addr += topbitstrlen(addr)
                    else:
                        add("B", addr + 0, "?frame delay until next message")
                        add("B", addr + 1, "Stop")
                        break

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
                decode_nibble_rle(addr, job, "curvature", curvemap)

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
                decode_nibble_rle(addr, job, "height", heightmap)

            case "map_rightobjs":
                add("b", addr, "Map right object data")
                decode_nibble_rle(addr, job, "right objects", OBJNAME[STAGE])

            case "map_leftobjs":
                add("b", addr, "Map left object data")
                decode_nibble_rle(addr, job, "left objects", OBJNAME[STAGE])

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
                decode_count_rle(addr, job, "lanes", lanesmap)

            case "map_hazards":
                add("b", addr, "Map hazards data")
                hmap = {0x00: "xxx"}
                decode_hazards(addr, job, "hazards", hmap)

            case "lod":
                for _ in range(0,6):
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
                        nbytes *= 2
                        masked = "(masked) "

                    add("B", bitmap, "Bitmap data %s%d bytes x %d" % (masked, widthbytes, height), nbytes, widthbytes)
                    if bitmap != shiftedbitmap:
                        add("B", shiftedbitmap, "Pre-shifted bitmap data %s%d bytes x %d" % (masked, widthbytes, height), nbytes, widthbytes)

                    addr += 7

            case _:
                print("Unhandled: " + job, file=sys.stderr)

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
        print("%s $%4X%s%s %s" % (blocktype, addr + ORIGBASE - BASE, nbytesstr, widthstr, desc))

if __name__ == '__main__':
    main(sys.argv[1:])
